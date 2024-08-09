#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4250

int main(int argc, char **argv) {
  int sock = 0;
  int long valread;


	// if (argc != 4) {
	// 	std::cout << "1st arg = port, 2nd arg = nb client, 3rd arg = request" << std::endl;
	// 	return 1;
	// }
  struct sockaddr_in serv_adr;
	// std::string request = argv[3];
	std::string request = "GET /dumas/";
	request += "��@�*v����%��́�Y��L�'��(M�NSl�,��0�'엻;}myU��1��K�A �^����Fr���Z-��C�g�Ze�1xwi��՘'�0���o��Z���Ğ[aBR7���~�e��I�����ڠ@���� %�X�q�{vR��Te��G�n\\\\\\////@�Y�5�rSR";
	request += " HTTP/1.1\r\n";
	request += "host: salut\r\n";
	request += "\r\n";
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
  for (size_t i = 0; i < 500 ; i++)
  {
	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
	send(sock, request.c_str(), request.size(), 0);
	usleep(100);
  }
  sleep(150);

  return (0);
}
