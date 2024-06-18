#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <vector>
#include <iostream>
#include "ServerConf.hpp"

class Config
{
    private:
        
    public:
        Config();
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);
        ~Config();
        Config parse(vec_string split); 
};

vec_string split(const std::string &str, const std::string &charset);

#endif
