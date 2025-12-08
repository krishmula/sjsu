#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static char *doc_root            = NULL;
static int   active_connections  = 0;
static int   total_requests      = 0;

static pthread_mutex_t conn_mtx  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t stat_mtx  = PTHREAD_MUTEX_INITIALIZER;

static const char *content_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";

    if (!strcasecmp(ext, ".html") || !strcasecmp(ext, ".htm") ||
        !strcasecmp(ext, ".shtml"))
        return "text/html";
    if (!strcasecmp(ext, ".txt"))
        return "text/plain";
    if (!strcasecmp(ext, ".css"))
        return "text/css";
    if (!strcasecmp(ext, ".js"))
        return "application/javascript";
    if (!strcasecmp(ext, ".jpg") || !strcasecmp(ext, ".jpeg"))
        return "image/jpeg";
    if (!strcasecmp(ext, ".gif"))
        return "image/gif";
    if (!strcasecmp(ext, ".png"))
        return "image/png";
    if (!strcasecmp(ext, ".svg"))
        return "image/svg+xml";
    if (!strcasecmp(ext, ".ico"))
        return "image/x-icon";
    if (!strcasecmp(ext, ".pdf"))
        return "application/pdf";
    if (!strcasecmp(ext, ".json"))
        return "application/json";
    if (!strcasecmp(ext, ".xml"))
        return "application/xml";

    return "application/octet-stream";
}

static void http_date(char *buf, size_t len) {
    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now);
    strftime(buf, len, "%a, %d %b %Y %H:%M:%S GMT", gmt);
}

static int wants_keep_alive(const char *req, const char *ver) {
    int http11 = (strcmp(ver, "HTTP/1.1") == 0);
    const char *h = strcasestr(req, "Connection:");
    if (h) {
        if (strcasestr(h, "close"))      return 0;
        if (strcasestr(h, "keep-alive")) return 1;
    }
    return http11;
}

static int dyn_timeout(void) {
    pthread_mutex_lock(&conn_mtx);
    int n = active_connections;
    pthread_mutex_unlock(&conn_mtx);
    if (n <= 5)   return 10;
    if (n <= 20)  return 5;
    if (n <= 50)  return 2;
    return 1;
}

static void send_error(int fd, int code, const char *version) {
    const char *msg, *body;
    switch (code) {
        case 400:
            msg = "Bad Request";
            body = "<html><body><h1>400 Bad Request</h1></body></html>";
            break;
        case 403:
            msg = "Forbidden";
            body = "<html><body><h1>403 Forbidden</h1></body></html>";
            break;
        case 404:
            msg = "Not Found";
            body = "<html><body><h1>404 Not Found</h1></body></html>";
            break;
        case 500:
        default:
            msg = "Internal Server Error";
            body =
                "<html><body><h1>500 Internal Server Error</h1></body></html>";
            break;
    }
    char date[128], hdr[512];
    http_date(date, sizeof(date));
    size_t blen = strlen(body);
    int len = snprintf(hdr, sizeof(hdr),
                       "%s %d %s\r\n"
                       "Date: %s\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: %zu\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "%s",
                       version, code, msg, date, blen, body);
    send(fd, hdr, len, MSG_NOSIGNAL);
}

static void *client_thread(void *arg) {
    int sock = *(int *)arg;
    free(arg);

    pthread_mutex_lock(&conn_mtx);
    active_connections++;
    pthread_mutex_unlock(&conn_mtx);

    int keep = 1;

    while (keep) {
        struct timeval tv = {dyn_timeout(), 0};
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        char buf[8192] = {0};
        int n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;

        char method[16], path[1024], ver[16];
        if (sscanf(buf, "%15s %1023s %15s", method, path, ver) != 3) {
            send_error(sock, 400, "HTTP/1.0");
            break;
        }

        keep = wants_keep_alive(buf, ver);

        if (strcmp(method, "GET") != 0) {
            send_error(sock, 400, ver);
            break;
        }

        if (strcmp(path, "/") == 0) strcpy(path, "/index.html");
        if (strstr(path, "..")) {
            send_error(sock, 403, ver);
            break;
        }

        char file[2048];
        snprintf(file, sizeof(file), "%s%s", doc_root, path);

        if (access(file, F_OK) != 0) {
            send_error(sock, 404, ver);
            if (!keep) break;
            continue;
        }
        if (access(file, R_OK) != 0) {
            send_error(sock, 403, ver);
            if (!keep) break;
            continue;
        }

        struct stat st;
        if (stat(file, &st) != 0 || !S_ISREG(st.st_mode)) {
            send_error(sock, 403, ver);
            if (!keep) break;
            continue;
        }

        int fd = open(file, O_RDONLY);
        if (fd < 0) {
            send_error(sock, 500, ver);
            break;
        }

        char date[128], hdr[1024];
        http_date(date, sizeof(date));
        const char *ctype = content_type(file);

        int hlen = snprintf(hdr, sizeof(hdr),
                            "%s 200 OK\r\n"
                            "Date: %s\r\n"
                            "Content-Type: %s\r\n"
                            "Content-Length: %ld\r\n"
                            "Connection: %s\r\n"
                            "\r\n",
                            ver, date, ctype, (long)st.st_size,
                            keep ? "keep-alive" : "close");
        send(sock, hdr, hlen, MSG_NOSIGNAL);

        char fbuf[8192];
        ssize_t r;
        while ((r = read(fd, fbuf, sizeof(fbuf))) > 0)
            send(sock, fbuf, r, MSG_NOSIGNAL);
        close(fd);

        pthread_mutex_lock(&stat_mtx);
        total_requests++;
        pthread_mutex_unlock(&stat_mtx);

        if (!keep) break;
    }

    close(sock);

    pthread_mutex_lock(&conn_mtx);
    active_connections--;
    pthread_mutex_unlock(&conn_mtx);
    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    int port = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-document_root") && i + 1 < argc)
            doc_root = argv[++i];
        else if (!strcmp(argv[i], "-port") && i + 1 < argc)
            port = atoi(argv[++i]);
    }
    if (!doc_root || port == 0) {
        fprintf(stderr,
                "Usage: %s -document_root <path> -port <port 8000-9999>\n",
                argv[0]);
        return 1;
    }

    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("socket");
        return 1;
    }
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in sa = {0};
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port        = htons(port);
    if (bind(server, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(server, 50) < 0) {
        perror("listen");
        return 1;
    }

    printf("Web server running on port: %d", port);

    while (1) {
        int *cfd = malloc(sizeof(int));
        if (!cfd) continue;
        *cfd = accept(server, NULL, NULL);
        if (*cfd < 0) {
            free(cfd);
            continue;
        }
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, cfd) == 0)
            pthread_detach(tid);
        else {
            close(*cfd);
            free(cfd);
        }
    }
    return 0;
}
