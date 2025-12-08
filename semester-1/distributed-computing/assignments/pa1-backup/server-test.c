/*  server.c  –  CS249 PA-1 compliant web server (quiet version)  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/*                      GLOBAL CONFIG / STATE                         */
/* ------------------------------------------------------------------ */
static char *doc_root           = NULL;
static int   active_connections = 0;
static pthread_mutex_t conn_mtx = PTHREAD_MUTEX_INITIALIZER;

/* ------------------------------------------------------------------ */
/*                      HELPER  UTILITIES                             */
/* ------------------------------------------------------------------ */
const char* get_content_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
        return "text/html";
    }
    if (strcmp(ext, ".shtml") == 0) {
        return "text/html";
    }
    if (strcmp(ext, ".txt") == 0) {
        return "text/plain";
    }
    if (strcmp(ext, ".css") == 0) {
        return "text/css";
    }
    if (strcmp(ext, ".js") == 0) {
        return "application/javascript";
    }
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return "image/jpeg";
    }
    if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    }
    if (strcmp(ext, ".png") == 0) {
        return "image/png";
    }
    if (strcmp(ext, ".svg") == 0) {
        return "image/svg+xml";
    }
    if (strcmp(ext, ".ico") == 0) {
        return "image/x-icon";
    }
    if (strcmp(ext, ".bmp") == 0) {
        return "image/bmp";
    }
    if (strcmp(ext, ".webp") == 0) {
        return "image/webp";
    }
    if (strcmp(ext, ".pdf") == 0) {
        return "application/pdf";
    }
    if (strcmp(ext, ".json") == 0) {
        return "application/json";
    }
    if (strcmp(ext, ".xml") == 0) {
        return "application/xml";
    }
    if (strcmp(ext, ".woff") == 0) {
        return "font/woff";
    }
    if (strcmp(ext, ".woff2") == 0) {
        return "font/woff2";
    }
    if (strcmp(ext, ".ttf") == 0) {
        return "font/ttf";
    }
    if (strcmp(ext, ".otf") == 0) {
        return "font/otf";
    }
    if (strcmp(ext, ".zip") == 0) {
        return "application/zip";
    }
    
    return "application/octet-stream";
}

static void http_date(char *buf, size_t len)
{
    time_t now = time(NULL);
    struct tm gmt;
    gmtime_r(&now, &gmt);
    strftime(buf, len, "%a, %d %b %Y %H:%M:%S GMT", &gmt);
}

static void send_error(int client, int code, const char *version)
{
    const char *msg, *body;
    switch (code) {
        case 400:
            msg  = "Bad Request";
            body = "<h1>400 Bad Request</h1>";
            break;
        case 403:
            msg  = "Forbidden";
            body = "<h1>403 Forbidden</h1>";
            break;
        case 404:
            msg  = "Not Found";
            body = "<h1>404 Not Found</h1>";
            break;
        default:
            msg  = "Internal Server Error";
            body = "<h1>500 Internal Server Error</h1>";
            code = 500;
    }

    char date[128], hdr[512];
    http_date(date, sizeof date);
    int len = snprintf(hdr, sizeof hdr,
        "%s %d %s\r\n"
        "Date: %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n%s",
        version, code, msg, date, strlen(body), body);

    send(client, hdr, len, MSG_NOSIGNAL);
}

/* ------------------------------------------------------------------ */
/*                      CONNECTION   HANDLER                          */
/* ------------------------------------------------------------------ */
static int timeout_for_load(void)
{
    pthread_mutex_lock(&conn_mtx);
    int c = active_connections;
    pthread_mutex_unlock(&conn_mtx);

    if (c <= 5)   return 10;
    if (c <= 20)  return 5;
    if (c <= 50)  return 2;
    return 1;
}

static void *client_thread(void *arg)
{
    int sock = *(int *)arg;
    free(arg);

    pthread_mutex_lock(&conn_mtx);
    ++active_connections;
    pthread_mutex_unlock(&conn_mtx);

    char buf[8192];
    int keep_alive = 1;

    while (keep_alive) {
        struct timeval tv = { .tv_sec = timeout_for_load(), .tv_usec = 0 };
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);

        ssize_t n = recv(sock, buf, sizeof buf - 1, 0);
        if (n <= 0) break;

        buf[n] = '\0';

        /* parse request line */
        char method[16], path[1024], version[16];
        if (sscanf(buf, "%15s %1023s %15s", method, path, version) != 3) {
            send_error(sock, 400, "HTTP/1.0");
            break;
        }

        /* only GET */
        if (strcmp(method, "GET") != 0) { send_error(sock, 400, version); break; }

        /* default file */
        if (strcmp(path, "/") == 0) strcpy(path, "/index.html");

        /* traversal protection */
        if (strstr(path, "..")) { send_error(sock, 403, version); break; }

        /* full path */
        char full[2048];
        snprintf(full, sizeof full, "%s%s", doc_root, path);

        /* permissions / existence check */
        if (access(full, F_OK) != 0) { send_error(sock, 404, version); continue; }
        if (access(full, R_OK) != 0) { send_error(sock, 403, version); continue; }

        struct stat st;
        if (stat(full, &st) != 0 || !S_ISREG(st.st_mode)) {
            send_error(sock, 403, version);
            continue;
        }

        int fd = open(full, O_RDONLY);
        if (fd < 0) { send_error(sock, 500, version); break; }

        const char *ctype = get_content_type(full);
        char date[128], hdr[1024];
        http_date(date, sizeof date);

        int hdr_len = snprintf(hdr, sizeof hdr,
            "%s 200 OK\r\n"
            "Date: %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: keep-alive\r\n\r\n",
            version, date, ctype, (long)st.st_size);

        send(sock, hdr, hdr_len, MSG_NOSIGNAL);

        /* send file */
        ssize_t r;
        while ((r = read(fd, buf, sizeof buf)) > 0)
            if (send(sock, buf, r, MSG_NOSIGNAL) < 0) break;
        close(fd);

        /* decide keep-alive */
        keep_alive = (strcmp(version, "HTTP/1.1") == 0);
        if (!keep_alive) break;
    }

    close(sock);

    pthread_mutex_lock(&conn_mtx);
    --active_connections;
    pthread_mutex_unlock(&conn_mtx);
    return NULL;
}

/* ------------------------------------------------------------------ */
/*                              MAIN                                  */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    int port = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-document_root") && i + 1 < argc)
            doc_root = argv[++i];
        else if (!strcmp(argv[i], "-port") && i + 1 < argc)
            port = atoi(argv[++i]);
    }
    if (!doc_root || port == 0) {
        fprintf(stderr, "Usage: %s -document_root <path> -port <port>\n", argv[0]);
        return 1;
    }

    /* ignore SIGPIPE so broken sockets don’t kill the server */
    signal(SIGPIPE, SIG_IGN);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    struct sockaddr_in addr = {0};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);
    if (bind(srv, (struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("bind"); close(srv); return 1;
    }
    if (listen(srv, 50) < 0) { perror("listen"); close(srv); return 1; }

    printf("Web server running on port %d, doc-root = %s\n", port, doc_root);

    for (;;) {
        int *client_fd = malloc(sizeof *client_fd);
        if (!client_fd) continue;

        *client_fd = accept(srv, NULL, NULL);
        if (*client_fd < 0) { free(client_fd); continue; }

        pthread_t t;
        if (pthread_create(&t, NULL, client_thread, client_fd) == 0)
            pthread_detach(t);
        else {
            close(*client_fd);
            free(client_fd);
        }
    }
}
