#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4247

int main(int ac, char **av) {
  int sock = 0;
  int long valread;

  struct sockaddr_in serv_adr;

  // Host: localhost:4247\nContent-Type: rest/html; charset=utf-8\nConta\n";
  std::string request;
  // request = "GET
  // /index.htmloooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  //
  // HTTP/1.145\n";
  request = "GET /respon/html/bonjour HTTP/1.1\r\n";
  // request += "Hosta:\n";
  request += "Host: webserv\r\n";
  // request += "bonjour: bbonjourbonjourronjourbonjoubonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjoubonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourrbonjourbonjourronjourrbonjourbonjourrjourbonjourrbonjourbonjourrrrbonjourbonjourrrr\n";
  // oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";

  // request += "\r\n";
  // request += "Accept: */*\r\n";
  // request += "Content-Type: application/x-www-form-urlencoded\r\n";
  // request += "Content-Length: 27\r\n";
  request += "\r\n";
  // request += "param1=value1&param2=value2";
  // request += "\r\n";

  char buffer[2048] = {0};
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cout << "Error: fail to create socket" << std::endl;
    return 1;
  }
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr) <= 0) {
    std::cout << "Error: fail to inet_pton" << std::endl;
    close(sock);
    return (1);
  }
  if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0) {
    std::cout << "Connection failed" << std::endl;
    close(sock);
    return (1);
  }
  send(sock, request.c_str(), request.size(), 0);
  std::cout << "Sent request: \n" << request << std::endl;
  valread = read(sock, buffer, 2048);
  std::cout << buffer << std::endl;
  sleep(3);
  // request = "Bonjour: */*\r\n";
  // request += "\r\n";

  valread = read(sock, buffer, 2048);
  std::cout << buffer << std::endl;
  // valread = read(sock, buffer, 2048);
  // std::cout << buffer << std::endl;
  // valread = read(sock, buffer, 2048);
  // std::cout << buffer << std::endl;
  // valread = read(sock, buffer, 2048);
  // std::cout << buffer << std::endl;
  // valread = read(sock, buffer, 2048);
  // std::cout << buffer << std::endl;

  return (0);
}
