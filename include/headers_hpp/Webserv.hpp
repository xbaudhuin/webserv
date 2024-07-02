#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "Error.hpp"
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

class Webserv
{
    private:
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
};

#endif
