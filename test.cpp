#include "Webserv.hpp"

void ServerConf::addHost(const std::string &str)
{
    size_t size = str.size();
    size_t count = 0;
    uint32_t ip = 0;
    uint8_t octet;

    for(size_t i = 0 ; i < size; i++)
    {
        if(str[i] == '.')
        {
            count++;
            if(i + 1 == size || (i + 1 < size && str[i + 1] == '.'))
                throw std::logic_error("syntax error for host target");
        }
    }

    if(count && count != 3)
        throw std::logic_error("syntax error for host target");

    vec_string vec = split(str, ".");

    /* Future condition to handle * and localhost if necessary 
    if(vec.size() == 1)
    {
    }
    else */

    /* bit shifting from chatgpt, i dunno if it works */
    if(vec.size() == 4)
    {
        for(int i = 0; i < 4; i++)
        {
            if(vec[i].size() > 3 || std::atoi(vec[i].c_str()) > 255)
                throw std::logic_error("syntax error for host target");
        }
        int bitshift = 0;
        for(int i = 0; i < 4; i++)
        {
            octet = static_cast<uint32_t>(std::strtol(vec[i].c_str(), NULL, 10));
            ip << bitshift;
            ip |= octet;
            bitshift = 8;
        }
        this->host = htonl(ip);
    }
    else
        throw std::logic_error("syntax error for host target");
}