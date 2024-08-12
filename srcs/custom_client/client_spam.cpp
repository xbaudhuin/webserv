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


	if (argc != 4) {
		std::cout << "1st arg = port, 2nd arg = nb client, 3rd arg = request" << std::endl;
		return 1;
	}
  struct sockaddr_in serv_adr;
	// std::string request = argv[3];
	std::string request = "GET / HTTP/1.1\r\n";
	request += "host: Taylor\r\n";
	request += "\r\n";
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
  for (int i = 0; i < atoi(argv[2]) ; i++)
  {
	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
	send(sock, request.c_str(), request.size(), 0);
	usleep(50);
  }
  sleep(150);

  return (0);
}
