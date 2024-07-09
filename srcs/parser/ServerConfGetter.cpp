#include "ServerConf.hpp"
#include "Error.hpp"

std::string &ServerConf::getIndexErrorPage(int errorCode)
{
    (void)errorCode;
    map_err_pages::iterator it = this->err_pages.find(errorCode);
    return(it->second);
}

const uint32_t &ServerConf::getHost(void) const
{
    return(this->host);
}

const int &ServerConf::getPort(void) const
{
    return(this->port);
}

void ServerConf::setMainServerName(void)
{
    std::string name = "Webserv";
    if(this->server_names.size() >= 1)
        name = this->server_names[0];
    this->server_name = name;
}

const std::string &ServerConf::getMainServerName(void) const
{
    return(this->server_name);
}

const map_err_pages &ServerConf::getErrPages(void) const
{
    return(this->err_pages);
}

const uint64_t &ServerConf::getLimitBodySize(void) const
{
    return(this->limit_body_size);
}

vec_location& ServerConf::getLocations(void){
    return(this->_locations);
}

Location& ServerConf::getPreciseLocation(const std::string &url)
{
    size_t size = this->_locations.size();
    std::string s = this->root + url;
    for (size_t i = 0; i < size; i++)
    {
        if(this->_locations[i].isExactMatch() && this->_locations[i].getUrl() == s)
        {
#ifdef PRINT
            std::cout << YELLOW << "✨ JACKPOT ✨" << RESET << std::endl;
            std::cout << RED << "FOUND IN PRECISE LOCATION" << std::endl;
            std::cout << s << RESET << std::endl;
#endif
            return(this->_locations[i]);
        }
    }
    size_t pos = s.size();
    pos = s.find_last_of("/", pos);
    std::string s1 = s.substr(0, pos + 1);
    std::string s2;
    if(s1 == s)
    {
#ifdef PRINT
        std::cout << BLUE << "TWINNING !" << RESET << std::endl;
#endif   
    }
    else
    {
        s2 = s.substr(pos, s.size());
#ifdef PRINT
        std::cout << GREEN << "EWW A FILE!" << std::endl;
        std::cout << "This is the ugly file: " << s2 << RESET << std::endl;
#endif
    }
    while (pos != std::string::npos)
    {   
        pos = s.find_last_of("/", pos);
        std::string s1 = s.substr(0, pos + 1);
        for (size_t i = 0; i < size; i++)
        {
            if(this->_locations[i].getUrl() == s)
            {
#ifdef PRINT
                std::cout << RED << "FOUND IN NORMAL LOCATION" << std::endl;
                std::cout << s1 << RESET << std::endl;
#endif
                return(this->_locations[i]);
            }
        }
        pos--;
        if(pos < this->root.size())
            break;
    }
    throw std::logic_error("you idiot sandwich");
    return(this->_locations[0]);
}

const size_t &ServerConf::getRank(void) const
{
    return(this->rank);
}

const std::string &ServerConf::getRoot(void) const{
    return(this->root);
}
