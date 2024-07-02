#include "Config.hpp"
#include "Error.hpp"

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

vec_confs Config::getMapConfs(void)
{
    return(this->confs);
}

bool checkNumberBrackets(const vec_string &split)
{
    size_t size = split.size();
    size_t count = 0;
    for (size_t i = 0; i < size; i++)
    {
        if(split[i] == "{")
        {
            count++;
        }
        else if(split[i] == "}")
        {
            if(count == 0 )
                return 1;
            count--;
        }
    }
    return 0;
    
}

void Config::parse(vec_string split)
{
    int check = 0;
    size_t size = split.size();
    // for(size_t jt = 0; jt < size; jt++)
    // {
    //     std::cout << split[jt] << std::endl;
    // }
    if(checkNumberBrackets(split))
        return(errorParsing("Issue with the file, uneven number of {}"));
    for(size_t i = 0; i < size; i++)
    {
        if(split[i] == "server")
        {
            i++;
            try
            {
                ServerConf newConf = parser(split, i, size);
                vec_string name = newConf.getServerNames();
                this->confs.push_back(std::make_pair(name, newConf));
                check++;
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
                // throw;
            }
        }
    }
    // std::cout << "Test: " << this->confs.size() << std::endl;
    if(check)
    {
        std::cout << "Coucou" << std::endl;
        printConfig(this->confs);  
    }

}

Config::~Config()
{
}