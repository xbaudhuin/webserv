#include "Config.hpp"

Config::Config()
{
}

Config::Config(const Config& rhs)
{
    this->operator=(rhs);
}

Config& Config::operator=(const Config &rhs)
{
    if(this != &rhs)
    {

    }
    return(*this);
}

Config Config::parse(vec_string split)
{
    size_t i = split.size();
    for(size_t j = 0; j < i; j++)
    {
        std::cout << split[j] << std::endl;
    }
    Config config;
    return(config);
}

Config::~Config()
{
}