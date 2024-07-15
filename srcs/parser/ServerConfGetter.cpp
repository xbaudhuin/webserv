#include "ServerConf.hpp"

std::string &ServerConf::getIndexErrorPage(int errorCode) {
  (void)errorCode;
  map_err_pages::iterator it = this->err_pages.find(errorCode);
  return (it->second);
}

const uint32_t &ServerConf::getHost(void) const { return (this->host); }

const int &ServerConf::getPort(void) const { return (this->port); }

void ServerConf::setMainServerName(void) {
  std::string name = "Webserv";
  if (this->server_names.size() >= 1)
    name = this->server_names[0];
  this->server_name = name;
}

const std::string &ServerConf::getMainServerName(void) const {
  return (this->server_name);
}

const map_err_pages &ServerConf::getErrPages(void) const {
  return (this->err_pages);
}

const uint64_t &ServerConf::getLimitBodySize(void) const {
  return (this->limit_body_size);
}
vec_location& ServerConf::getLocations(void){
    return(this->_locations);
}

Location& ServerConf::getPreciseLocation(const std::string &url)
{
    bool is_file = 0;
    size_t size = this->_locations.size();
    std::string s = this->root + url;
    std::cout << RED << "URI: " << s << RESET << std::endl;
    for (size_t i = 0; i < size; i++)
    {
        if(this->_locations[i].isExactMatch() && this->_locations[i].getUrl() == s)
        {
#if PRINT == 2
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
#if PRINT == 2
        std::cout << BLUE << "TWINNING !" << RESET << std::endl;
#endif   
    }
    else
    {
        is_file = 1;
        s2 = s.substr(pos, s.size());
#if PRINT == 2
        std::cout << GREEN << "EWW A FILE!" << std::endl;
        std::cout << "This is the ugly file: " << s2 << RESET << std::endl;
#endif
    }
    if(is_file)
    {
        size_t pos2 = pos;
        while (pos2 != std::string::npos)
        {   
            pos2 = s.find_last_of("/", pos2);
            std::string s1 = s.substr(0, pos2 + 1);
            // std::cout << "HERE IDIOT: " << s1 << std::endl;
            for (size_t i = 0; i < size; i++)
            {
                if(!this->_locations[i].isExactMatch() && !this->_locations[i].isADir() && this->_locations[i].getUrl() == s1)
                {
                    std::string uri = this->_locations[i].getIndexFile()[0];
                    std::string file = uri.substr(uri.find_last_of("/", uri.size()), uri.size());
                    std::cout << YELLOW << file << " && " << s2 << " && " << uri<< RESET << std::endl;
                    if(file == s2)
                    {
#if PRINT == 2
                        std::cout << PURP << "NICE, FOUND IN FILE LOCATION!" << std::endl;
#endif
                        return(this->_locations[i]);
                    }
                }
            }
            pos2--;
            if(pos2 < this->root.size())
                break;
        }
    }
    while (pos != std::string::npos)
    {   
        pos = s.find_last_of("/", pos);
        std::string s1 = s.substr(0, pos + 1);
        // std::cout << "HERE IDIOT: " << s1 << std::endl;
        for (size_t i = 0; i < size; i++)
        {
            if(this->_locations[i].isADir() && this->_locations[i].getUrl() == s1)
            {
#if PRINT == 2
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
