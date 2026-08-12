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

char *read_file(const char *filepath) {
  FILE *http_file = fopen(filepath, "rb");
  if (http_file == NULL) {
    printf("Could not open %s\n", filepath);
    return NULL;
  }

  fseek(http_file, 0, SEEK_END);
  long file_size = ftell(http_file);
  rewind(http_file);

  char *http_string = (char *)malloc(file_size + 1);
  if (http_string == NULL) {
    printf("Memory allocation failed\n");
    fclose(http_file);
    return NULL;
  }

  size_t bytes_read = fread(http_string, 1, file_size, http_file);
  http_string[bytes_read] = '\0';
  fclose(http_file);

  return http_string;
}

char *str_replace(const char *source, const char *placeholder,
                  const char *replacement) {
  size_t ph_len = strlen(placeholder);
  size_t rp_len = strlen(replacement);

  // Count occurrences
  int count = 0;
  const char *p = source;
  while ((p = strstr(p, placeholder)) != NULL) {
    count++;
    p += ph_len;
  }

  size_t result_len = strlen(source) + count * (rp_len - ph_len) + 1;
  char *result = (char *)malloc(result_len);
  char *dst = result;

  p = source;
  while (*p) {
    if (strncmp(p, placeholder, ph_len) == 0) {
      memcpy(dst, replacement, rp_len);
      dst += rp_len;
      p += ph_len;
    } else {
      *dst++ = *p++;
    }
  }
  *dst = '\0';

  return result;
}

int main(int argc, char const *argv[]) {

  int sockfd, new_sock;
  struct sockaddr_in address;
  ssize_t valread;
  char buffer[1024] = {0};
  socklen_t addrlen = sizeof(address);
  int opt = 1;
  size_t allocated_size = 0;

  const char *type = "text/html";

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

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

  while (1) {
    if ((new_sock = accept(sockfd, (struct sockaddr *)&address, &addrlen)) <
        0) {
      perror("Socket acceptance failed");
      exit(EXIT_FAILURE);
    }

    valread = read(new_sock, buffer, 1024 - 1);
    char route[50];
    char route_string[50];
    sscanf(buffer, "%*s %49s", route);
    sscanf(buffer, "%*s /%49s", route_string);
    printf("%s\n", route);

    if (strcmp("/", route) == 0) {
      printf("Entered default route\n");
      char *body = read_file("../index.html");
      char *http_response = create_http_response(body, type, &allocated_size);
      send(new_sock, http_response, allocated_size, 0);
      free(http_response);
      close(new_sock);
      free(body);
    } else {
      printf("Entered %s route\n", route_string);

      char *template = read_file("../greeting.html");
      char *body_string = str_replace(template, "{{NAME}}", route_string);
      free(template);

      char *http_response =
          create_http_response(body_string, type, &allocated_size);
      send(new_sock, http_response, allocated_size, 0);
      free(body_string);
      free(http_response);
      close(new_sock);
    }
  }

  close(sockfd);

  return 0;
}
