#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4252

int main(int argc, char **argv) {
  int sock = 0;
  int long valread;


	if (argc != 3) {
		std::cout << "1st arg = port, 2nd arg = nb client" << std::endl;
	}
  struct sockaddr_in serv_adr;
	std::string request = "SPAM";
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cout << "Error: fail to create socket" << std::endl;
    return 1;
  }
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_port = htons(atoi(argv[1]));

  if (inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr) <= 0) {
    std::cout << "Error: fail to inet_pton" << std::endl;
    close(sock);
    return (1);
  }
  for (size_t i = 0; i < atoi(argv[2]) ; i++)
  {
	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
	send(sock, request.c_str(), request.size(), 0);
  }
  sleep(150);

  return (0);
}
