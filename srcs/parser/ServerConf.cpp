#include "ServerConf.hpp"

ServerConf::ServerConf()
{
    port = -1;
    host = -1;
    socket = -1;
    limit_body_size = "";
}

ServerConf::ServerConf(const ServerConf &rhs)
{
    this->operator=(rhs);
}

ServerConf& ServerConf::operator=(const ServerConf &rhs)
{
    if(this != &rhs)
    {
        this->server_names = rhs.server_names;
        this->port = rhs.port;
        this->host = rhs.host;
        this->socket = rhs.socket;
        this->err_pages = rhs.err_pages;
        this->limit_body_size = rhs.limit_body_size;
        this->locate = rhs.locate;
    }
    return(*this);
}

ServerConf::~ServerConf()
{
}

void ServerConf::addServerName(const std::string &name)
{
    this->server_names.push_back(name);
}

vec_string ServerConf::getServerNames()
{
    return(this->server_names);
}

bool ServerConf::nameExist(const std::string &name)
{
    size_t size = this->server_names.size();
    for(size_t i = 0; i < size; i++)
    {
        if(this->server_names[i] == name)
            return(1);
    }
    return(0);
}

void ServerConf::addPortOrHost(const std::string &str)
{
    size_t pos = str.find_first_not_of("0123456789.:;", 0);
    if(pos != std::string::npos)
        throw std::logic_error("syntax error for listen directive");
    vec_string vec = split(str, ":");
    if(vec.size() == 1)
    {
        if((pos = vec[0].find_first_of(".")) == std::string::npos)
            this->addPort(vec[0]);
        else
            this->addHost(vec[0]);        
    }
    else if(vec.size() == 2)
    {
        this->addHost(vec[0]);
        this->addPort(vec[1]);
    }
    else
        throw std::logic_error("syntax error for listen directive");
}

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
            octet = static_cast<uint8_t>(std::strtol(vec[i].c_str(), NULL, 10));
            if(octet > 255)
            {
                this->host = -1;
                throw std::logic_error("syntax error for host target");
            }
            ip |= (octet << ((3 - i) * 8));
        }
        this->host = htonl(ip);
    }
    else
        throw std::logic_error("syntax error for host target");
}

void ServerConf::addPort(const std::string &str)
{
    std::string port_s = "";
    size_t pos = str.find_first_of(";", 0);
    if(pos != std::string::npos)
        port_s = str.substr(0, str.size() - 1);
    else
        port_s = str;

    /* small check because internet says that we can maintain at most 65535 connections */
    long int port_value = strtol(port_s.c_str(), NULL, 10);
    if(port_value > 65535 || port_value < 0)
        throw std::logic_error("syntax error for port target");
    this->port = static_cast<int>(port_value);
}
