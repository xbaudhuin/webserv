#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <algorithm>

# define RED "\033[1;31m"
# define RESET "\033[0m"
# define PURP "\033[1;95m"
# define PURP2 "\033[1;35m"
# define ORANGE "\033[1;91m"
# define LIGHTB "\033[1;36m"
# define BLUE "\033[1;94m"
# define GREEN "\033[1;92m"
# define YELLOW "\033[1;93m"

using namespace std;

typedef vector<string> vs;

#define PORT 4243

vs split(const std::string &str, const std::string &charset)
{
    vs split;
    size_t pos = 0;
    size_t posend = 0;

    while(1)
    {
        if((pos = str.find_first_not_of(charset, pos)) == std::string::npos)
            break;
        posend = str.find_first_of(charset, pos);
        std::string to_add = str.substr(pos, posend - pos);
        split.push_back(to_add);
        if(posend == std::string::npos)
            break;
        pos = posend;
    }

    return(split);
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

int main(int ac, char **av) {
  
  vs v;
  std::ifstream	config;
	config.open("./tester.log");
	if (!config.is_open())
    return(printf("Couldn't open file ./tester.log\n"));
	std::stringstream strm;
	strm << config.rdbuf();
	std::string str = strm.str();
	config.close();
  vs s = split(str, "\n");
  for (size_t i = 0; i < s.size(); i++)
  {
    s[i] = ReplaceAll(s[i], "$", "\r\n");
    s[i] = ReplaceAll(s[i], "%", "\n");
  }
  
  for (size_t i = 0; i < s.size(); i++)
  {
    v.push_back(s[i]);
  }

  
  for (size_t i = 0; i < v.size(); i++)
  {
      int sock = 0;
      int long valread;

      struct sockaddr_in serv_adr;
      char buffer[20048] = {0};
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
      send(sock, v[i].c_str(), v[i].size(), 0);
      std::cout << RED << "**********SENT REQUEST [" << i << "] :********** \n" << RESET << v[i] << "\n" << std::endl;
      valread = read(sock, buffer, 20048);
      std::cout << buffer << std::endl;
      std::cout << BLUE << "READ = " << valread << RESET "\n" << std::endl;
      valread = read(sock, buffer, 20048);
      std::cout << YELLOW << buffer << RESET "\n" <<  std::endl;
      std::cout << "READ = " << valread << std::endl;
      close(sock);
  }
  
  return (0);
}
