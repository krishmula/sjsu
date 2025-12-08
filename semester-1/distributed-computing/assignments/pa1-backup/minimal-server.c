#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  char *doc_root = NULL;
  int port = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-document_root") == 0) {
      doc_root = argv[++i];
    } else if (strcmp(argv[i], "-port") == 0) {
      port = atoi(argv[++i]);
    }
  }

  if (!doc_root || !port) {
    printf("Usage: %s -document_root <path> -port\n", argv[0]);
    return 1;
  }

  printf("Server starting on port %d, serving from %s\n", port, doc_root);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {AF_INET, htons(port), 0};
  bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
  listen(server_fd, 5);

  while (1) {
    int client = accept(server_fd, NULL, NULL);

    char buf[1000];
    recv(client, buf, 1000, 0);

    char method[16], path[256], version[16];
    sscanf(buf, "%s %s %s", method, path, version);
    printf("Request: %s\n", method, path, version);

    char filepath[512];
    sprintf(filepath, "%s%s", doc_root, path);

    printf("Looking for file: %s\n", filepath);

    int fd = open(filepath, O_RDONLY);

    if (fd < 0) {
      char *resp = "HTTP/1.0 404 Not Found\r\n\r\n404 Not Found";
      send(client, resp, strlen(resp), 0);
    } else {
      char *header = "HTTP/1.0 200 OK\r\n\r\n";
      send(client, header, strlen(header), 0);

      char filebuf[4096];
      int bytes;
      while ((bytes = read(fd, filebuf, 4096)) > 0) {
        send(client, filebuf, bytes, 0);
      }
      close(fd);
    }
    close(client);
  }
}
