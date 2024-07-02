#include "ServerConf.hpp"
#include "Error.hpp"
ServerConf::ServerConf()
{
    port = 80;
    host = 0;
    socket = -1;
    limit_body_size = 0;
    rank = -1;
    root = "./html";
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
        this->_locations = rhs._locations;
        this->rank = rhs.rank;
        this->root = rhs.root;
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

void ServerConf::setRank(size_t rank)
{
    this->rank = rank;
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
            ip = ip << bitshift;
            ip |= octet;
            bitshift = 8;
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

void ServerConf::addErrorPage(const std::string &url, const std::vector<int> err)
{
    size_t size = err.size();
    for(size_t i = 0; i < size; i++)
    {
        this->err_pages.insert(std::make_pair(err[i], url));
    }
}

void ServerConf::addLimitBodySize(const std::string &limit)
{
    std::string value;
    uint64_t to_multiply = 1;
    size_t pos = 0;
    char c = '\0';
    pos = limit.find_first_not_of("0123456789kmgKMG;", 0);
    if(pos != std::string::npos)
        throw std::logic_error("Error:\nIncorrect client_limit_body_size passed as parameter1");
    vec_string check = split(limit, ";");
    if(check.size() > 1)
        throw std::logic_error("Error:\nIncorrect client_limit_body_size passed as parameter2");
    pos = limit.find_first_of("kmgKMG", 0);
    if(pos != std::string::npos)
    {
        // std::cerr<< pos << " && " << check[0].size() << std::endl;
        if(pos == 0 || pos + 1 < check[0].size())
            throw std::logic_error("Error:\nIncorrect client_limit_body_size passed as parameter3");
        c = limit[pos];
    }
    else
        pos = limit.find_first_of(";", 0);
    value = limit.substr(0, pos);
    switch (c)
    {
        case 'k':
        case 'K':
            to_multiply *= 1024;
            break;
        case 'm':
        case 'M':
            to_multiply *= (1024 *1024);
            break;
        case 'g':
        case 'G':
            to_multiply *= (1024 *1024 *1024);
            break;
        default:
            break;
    }
    this->limit_body_size = static_cast<uint64_t>(std::strtoull(value.c_str(), NULL, 10)) * to_multiply;
    // std::cout << "limit body size: " << this->limit_body_size << std::endl; 
}

void ServerConf::addLocation(const Location &loc)
{
    this->_locations.push_back(loc);
}

void ServerConf::addRoot(const std::string &dir)
{
    std::string check = dir;
    // if(check[0] != '.')
    //     check.insert(0, ".");
    std::string save = check;
    vec_string s = tokenizer(check, " ", "/");
    check = save;
    // if(s[0] != ".")
    //     throw std::logic_error("Error:\nRoot directive parameter is missing '/' at the beginning");
    if(check[check.size() - 1] == '/')
        check.erase(check.end() - 1);
    this->root = check;
}
