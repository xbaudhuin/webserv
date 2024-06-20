#include "ServerConf.hpp"

std::string ServerConf::getIndexErrorPage(int errorCode)
{
    return("");
}

std::string ServerConf::limitBodySize(void)
{
    return(this->limit_body_size);
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
