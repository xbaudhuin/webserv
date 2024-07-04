#include "ServerConf.hpp"

std::string ServerConf::getIndexErrorPage(int errorCode)
{
    (void)errorCode;
    map_err_pages::iterator it = this->err_pages.find(errorCode);
    if(it == this->err_pages.end())
        return("");
    return(it->second);
}

uint32_t ServerConf::getHost(void) const
{
    return(this->host);
}

int ServerConf::getPort(void) const
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

std::string ServerConf::getMainServerName(void) const
{
    return(this->server_name);
}

map_err_pages ServerConf::getErrPages(void) const
{
    return(this->err_pages);
}

uint64_t ServerConf::getLimitBodySize(void) const
{
    return(this->limit_body_size);
}

vec_location& ServerConf::getLocations(void){
    return(this->_locations);
}

Location ServerConf::getPreciseLocation(const std::string &url) const
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

size_t ServerConf::getRank(void) const
{
    return(this->rank);
}

std::string ServerConf::getRoot(void) const{
    return(this->root);
}
