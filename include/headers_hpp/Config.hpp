#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <vector>
#include <iostream>
#include "ServerConf.hpp"

class Config
{
    private:
        map_confs confs;

    public:
        Config();
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);
        ~Config();
        Config parse(vec_string split); 
        map_confs getMapConfs(void);

};

vec_string split(const std::string &str, const std::string &charset);

#endif
