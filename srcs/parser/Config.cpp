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
        this->confs = rhs.confs;
    }
    return(*this);
}

map_confs Config::getMapConfs(void)
{
    return(this->confs);
}

void Config::parse(vec_string split)
{
    size_t size = split.size();
    for(size_t jt = 0; jt < size; jt++)
    {
        std::cout << split[jt] << std::endl;
    }
    for(size_t i = 0; i < size; i++)
    {
        if(split[i] == "server")
        {
            i++;
            ServerConf newConf = parser(split, i, size);
            std::string name = newConf.getMainServerName();
            this->confs.insert(std::make_pair(name, newConf));
        }
    }
}

Config::~Config()
{
}