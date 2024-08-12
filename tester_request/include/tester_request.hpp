#ifndef TESTER_REQUEST_HPP
# define TESTER_REQUEST_HPP

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
#include <pthread.h>
#include <sys/wait.h>

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

typedef struct s_request
{
    char *host;
    int PORT;
    string request;
    int sock;
    int long valread;
    struct sockaddr_in serv_adr;
    int i;
    pthread_t		thread;
    std::ofstream *os;
} t_request;


typedef vector<string> vs;
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
vs split(const std::string &str, const std::string &charset);
void * launch_request(void *v);

#endif
