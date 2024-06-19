#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

typedef std::vector<std::string> vec_string;
typedef std::map<int, std::string> map_err_pages;
typedef std::map<std::string, std::map<int, std::string> > location;

class ServerConf
{
    private:
        vec_string server_names;
        int port;
        int host;
        uint32_t socket;
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
        uint32_t getSocket(void);
        std::string getIndexErrorPage(int errorCode);
        std::string limitBodySize(void);
        int getPort(void) const;
        int getHost(void) const;


};

// ServerConf::ServerConf()
// {
// }

// ServerConf::~ServerConf()
// {
// }

typedef std::map<std::string, ServerConf> map_confs;

#endif
