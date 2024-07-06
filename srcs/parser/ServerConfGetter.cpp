#include "ServerConf.hpp"

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

const Location& ServerConf::getPreciseLocation(const std::string &url) const
{
    size_t size = this->_locations.size();
    for (size_t i = 0; i < size; i++)
    {
        if(this->_locations[i].getUrl() == url)
            return(this->_locations[i]);
    }
    throw std::logic_error("how wtf?");
    return(this->_locations[0]);
}

const size_t &ServerConf::getRank(void) const
{
    return(this->rank);
}

const std::string &ServerConf::getRoot(void) const{
    return(this->root);
}
