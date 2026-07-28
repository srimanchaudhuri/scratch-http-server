#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const *argv[]) {

  int clientfd, new_sock, status;
  struct sockaddr_in address;
  ssize_t valread;
  char *hello = "Hello, From Client";
  char buffer[1024] = {0};
  socklen_t addrlen = sizeof(address);
  int opt = 1;

  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
    printf("Invalid address\n");
    return -1;
  }

  if ((status = connect(clientfd, (struct sockaddr *)&address, addrlen)) < 0) {
    printf("Invalid connection\n");
    return -1;
  }

  send(clientfd, hello, strlen(hello), 0);
  printf("Hello sent\n");
  valread = read(clientfd, buffer, 1024 - 1);
  printf("%s\n", buffer);

  close(clientfd);
  return 0;
}
