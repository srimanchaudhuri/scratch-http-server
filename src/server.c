#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const *argv[]) {

  int sockfd, new_sock;
  struct sockaddr_in address;
  ssize_t valread;
  char buffer[1024] = {0};
  socklen_t addrlen = sizeof(address);
  int opt = 1;

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

  send(new_sock, "Hello from server", strlen("Hello from server"), 0);
  printf("Hello message sent\n");

  close(new_sock);
  close(sockfd);

  return 0;
}
