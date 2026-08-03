#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

char *create_http_response(const char *body, const char *content_type,
                           size_t *out_size) {
  size_t body_len = strlen(body);

  const char *header_template = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: %s\r\n"
                                "Content-Length: %zu\r\n"
                                "/r/n";

  int header_len = snprintf(NULL, 0, header_template, content_type, body_len);
  size_t total_len = header_len + body_len + 1;

  char *response_string = (char *)malloc(total_len);

  snprintf(response_string, total_len,
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: %s\r\n"
           "Content-Length: %zu\r\n"
           "\r\n"
           "%s",
           content_type, body_len, body);

  *out_size = total_len - 1;

  return response_string;
}

int main(int argc, char const *argv[]) {

  int sockfd, new_sock;
  struct sockaddr_in address;
  ssize_t valread;
  char buffer[1024] = {0};
  socklen_t addrlen = sizeof(address);
  int opt = 1;
  const char *body = "<html><body><h1>Hello World</h1></body></html>";
  const char *type = "text/html";
  size_t allocated_size = 0;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  //   // Forcefully attaching socket to the port 8080
  //   if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
  //                  sizeof(opt))) {
  //     perror("setsockopt");
  //     exit(EXIT_FAILURE);
  //   }

  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  if ((bind(sockfd, (struct sockaddr *)&address, addrlen)) < 0) {
    perror("Socket bind failed");
    exit(EXIT_FAILURE);
  }

  if ((listen(sockfd, 3)) < 0) {
    perror("Socket listen failed");
    exit(EXIT_FAILURE);
  }

  if ((new_sock = accept(sockfd, (struct sockaddr *)&address, &addrlen)) < 0) {
    perror("Socket acceptance failed");
    exit(EXIT_FAILURE);
  }

  valread = read(new_sock, buffer, 1024 - 1);
  printf("%s\n", buffer);

  char *http_response = create_http_response(body, type, &allocated_size);

  send(new_sock, http_response, allocated_size, 0);
  printf("HTTP message sent\n");

  close(new_sock);
  close(sockfd);

  return 0;
}
