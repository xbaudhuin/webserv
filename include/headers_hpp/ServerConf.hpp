#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

typedef std::vector<std::string> vec_string;
typedef std::map<std::string, std::vector<int> > map_err_pages;

class ServerConf
{
    private:
        vec_string server_names;
        std::string port;
        std::string host;
        map_err_pages err_pages;
        std::string limit_body_size;

    public:
        ServerConf();
        ~ServerConf();
        void addServerName(const std::string &name);
        vec_string getServerNames();
        bool nameExist(const std::string &name);
        void addPort(const std::string &str);
        void addHost(const std::string &str);

};

// ServerConf::ServerConf()
// {
// }

// ServerConf::~ServerConf()
// {
// }

typedef std::vector<ServerConf&> vec_servConfs;

#endif
