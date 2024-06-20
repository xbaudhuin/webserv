#include "Config.hpp"

void configureBasicServer(ServerConf &cf)
{
    static int i = 0;
    cf.addHost("");
    cf.addPort("");
    cf.addServerName("");
    std::vector<int> v;
    v.push_back(404);
    cf.addErrorPage("", v);
    cf.setMainServerName();
    i++;
}

ServerConf parser(const vec_string &split, size_t &i, const size_t &size){
    ServerConf cf;
    size_t pos = 0;

    if (split[i] != "{" )
        throw std::logic_error("Error:\nMissing '{' of the server block");
    else if (( pos = split[i].find("}")) != std::string::npos)
        throw std::logic_error("Error:\nMisconfigured server block, issue comes from '}'");
    i++;
    if (split[i] == "}")
    {
        configureBasicServer(cf);
        return(cf);
    }
    
}
