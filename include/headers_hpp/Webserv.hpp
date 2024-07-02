#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "Error.hpp"
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "SubServ.hpp"
#include "Typedef.hpp"
#include "Utils.hpp"
# include "sockets.hpp"


class Webserv
{
    private:
		int			_epollFd;
		mapSubServs	_subServs;
        vec_string env;
        char **env_char;
        Config conf;
        Webserv();
        void parseConfig(const std::string &conf);

    public:
        Webserv(const char* s);
        Webserv(const Webserv& rhs);
        Webserv& operator=(const Webserv &rhs);
        ~Webserv();
        void addEnv(char **env);
        char ** getEnv(void);
		int addSocketToEpoll(int socketFd);
};

#endif
