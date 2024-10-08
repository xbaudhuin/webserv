#include "ServerConf.hpp"

ServerConf::ServerConf()
{
    port = 4243;
    addHost("127.0.0.1");
    limit_body_size = 0;
    rank = -1;
    root = "";
    err_pages[100] = "/error_pages/100.html";
    err_pages[101] = "/error_pages/101.html";
    err_pages[102] = "/error_pages/102.html";
    err_pages[103] = "/error_pages/103.html";
    err_pages[200] = "/error_pages/200.html";
    err_pages[201] = "/error_pages/201.html";
    err_pages[202] = "/error_pages/202.html";
    err_pages[203] = "/error_pages/203.html";
    err_pages[204] = "/error_pages/204.html";
    err_pages[205] = "/error_pages/205.html";
    err_pages[206] = "/error_pages/206.html";
    err_pages[207] = "/error_pages/207.html";
    err_pages[208] = "/error_pages/208.html";
    err_pages[226] = "/error_pages/226.html";
    err_pages[300] = "/error_pages/300.html";
    err_pages[301] = "/error_pages/301.html";
    err_pages[302] = "/error_pages/302.html";
    err_pages[303] = "/error_pages/303.html";
    err_pages[304] = "/error_pages/304.html";
    err_pages[305] = "/error_pages/305.html";
    err_pages[306] = "/error_pages/306.html";
    err_pages[307] = "/error_pages/307.html";
    err_pages[308] = "/error_pages/308.html";
    err_pages[400] = "/error_pages/400.html";
    err_pages[401] = "/error_pages/401.html";
    err_pages[402] = "/error_pages/402.html";
    err_pages[403] = "/error_pages/403.html";
    err_pages[404] = "/error_pages/404.html";
    err_pages[405] = "/error_pages/405.html";
    err_pages[406] = "/error_pages/406.html";
    err_pages[407] = "/error_pages/407.html";
    err_pages[408] = "/error_pages/408.html";
    err_pages[409] = "/error_pages/409.html";
    err_pages[410] = "/error_pages/410.html";
    err_pages[411] = "/error_pages/411.html";
    err_pages[412] = "/error_pages/412.html";
    err_pages[413] = "/error_pages/413.html";
    err_pages[414] = "/error_pages/414.html";
    err_pages[415] = "/error_pages/415.html";
    err_pages[416] = "/error_pages/416.html";
    err_pages[417] = "/error_pages/417.html";
    err_pages[418] = "/error_pages/418.html";
    err_pages[421] = "/error_pages/421.html";
    err_pages[422] = "/error_pages/422.html";
    err_pages[423] = "/error_pages/423.html";
    err_pages[424] = "/error_pages/424.html";
    err_pages[425] = "/error_pages/425.html";
    err_pages[426] = "/error_pages/426.html";
    err_pages[428] = "/error_pages/428.html";
    err_pages[429] = "/error_pages/429.html";
    err_pages[431] = "/error_pages/431.html";
    err_pages[451] = "/error_pages/451.html";
    err_pages[500] = "/error_pages/500.html";
    err_pages[501] = "/error_pages/501.html";
    err_pages[502] = "/error_pages/502.html";
    err_pages[503] = "/error_pages/503.html";
    err_pages[504] = "/error_pages/504.html";
    err_pages[505] = "/error_pages/505.html";
    err_pages[506] = "/error_pages/506.html";
    err_pages[507] = "/error_pages/507.html";
    err_pages[508] = "/error_pages/508.html";
    err_pages[510] = "/error_pages/510.html";
    err_pages[511] = "/error_pages/511.html";
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
    if(!this->nameExist(name))
        this->server_names.push_back(name);
}

vec_string& ServerConf::getServerNames()
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
        if(this->err_pages.find(err[i]) != this->err_pages.end())
            this->err_pages.erase(err[i]);
        this->err_pages[err[i]] = url;
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
}

void ServerConf::addLocation(Location &loc)
{
    if(loc.isADir())
    {
        std::string url = loc.getUrl();
#if PRINT == 2
        std::cout << RED << url << RESET << std::endl;
#endif
        for (size_t i = 0; i < this->_locations.size(); i++)
        {
            if(url == this->_locations[i].getUrl() && this->_locations[i].isGenerated())
            {
                this->_locations.erase(_locations.begin() + i, _locations.begin() + i + 1);
                i = 0;
            }
        }
    }
    if(!loc.isADir() && !loc.isExactMatch())
    {
        std::string url = loc.getIndexFile()[0] + "/";
        for (size_t i = 0; i < this->_locations.size(); i++)
        {
            if(url == this->_locations[i].getUrl())
                return;
        }
        Location lc(loc, 1);
        this->_locations.push_back(lc);
    }
    if(loc.hasAlias())
        loc.fixFileLocationAlias();
    this->_locations.push_back(loc);
}

void ServerConf::addRoot(const std::string &dir)
{
    std::string check = dir;
    std::string save = check;
    vec_string s = tokenizer(check, " ", "/");
    check = save;
    if(check[0] != '.' && check[1] && check[1] != '/')
        throw std::logic_error("Webserv: Error:\nRoot directive parameter is missing './' at the beginning");
    check.erase(0, 1);
    if(check[check.size() - 1] == '/')
        check.erase(check.size() - 1);
    this->root = check;
}

void ServerConf::setRootToErrorPages(){
    map_err_pages::iterator it = this->err_pages.begin();
    while (it != this->err_pages.end())
    {
        it->second.insert(0, this->root);
        it++;
    }
    
}
