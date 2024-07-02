#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "Error.hpp"
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "SubServ.hpp"

class Webserv
{
    private:
        vec_string 				env;
		int						_epollSocket;
        char 					**env_char;
        Config					conf;
		std::map<int, SubServ>	_subServs;

    public:
        Webserv();
        Webserv(const Webserv& rhs);
        Webserv& operator=(const Webserv &rhs);
        ~Webserv();
        void addEnv(char **env);
        void parseConfig(const std::string &conf);
        char ** getEnv(void);
};

#endif
