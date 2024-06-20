#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include <fstream>
#include <sstream>

class Webserv
{
    private:
        vec_string env;
        char **env_char;
        Config conf;

    public:
        Webserv();
        Webserv(const Webserv& rhs);
        Webserv& operator=(const Webserv &rhs);
        ~Webserv();
        void addEnv(char **env);
        void parseConfig(const std::string &conf);
        char ** getEnv(void);
};

template <typename F>
void writeInsideLog(const std::exception &e, F f)
{
    std::string message = e.what();
    f(message);
}

void errorParsing(const std::string &message);
void errorServer(const std::string &message);

#endif
