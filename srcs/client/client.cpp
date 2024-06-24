#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#define PORT 4247

int main(int ac, char **av)
{
  int sock = 0; 
  int long valread;

  struct sockaddr_in serv_adr;

  // Host: localhost:4247\nContent-Type: rest/html; charset=utf-8\nConta\n";
  std::string request;
  request = "\r\n\r\n\nPOST /endpoint HTTP/1.95\n";
  request += "Hosta:\n";
  request += "Host: localhost:4247\n";
  request += "Content-Type: rest/html; charset=utf-8\n";

  char buffer[1024] = {0};
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    std::cout << "Error: fail to create socket" << std::endl;
    return 1;
  }
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr) <= 0){
    std::cout << "Error: fail to inet_pton" << std::endl;
    close(sock);
    return (1);
  }
  if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0){
    std::cout << "Connection failed" << std::endl;
    close(sock);
    return (1);
  }
  send(sock, request.c_str(), request.size(), 0);
  std::cout << "Sent request" << std::endl;
  valread = read(sock, buffer, 1024);
  std::cout << buffer<< std::endl;
  return (0);

}
