#include "Webserv.hpp"

void configureBasicServer(ServerConf &cf)
{
    static int i = 0;
    cf.addHost("127.0.0.1");
    cf.addPort("80");
    cf.addServerName("Webserv");
    std::vector<int> v;
    v.push_back(404);
    // cf.addErrorPage("", v);
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
    while(i < size)
    {
        if(split[i] == "listen")
        {
            i++;
            if(i < size && split[i].find(';', 0) == std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                {
                    errorParsing("Error inside the listen directive, ';' not found");
                    continue;
                }
            }
            try
            {
                cf.addPortOrHost(split[i]);
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
            }
            
            i++;
            continue;
        }

        else if(split[i] == "server_name")
        {
            i++;
            while (i < size)
            {
                if(split[i].find_first_of("{}", 0) != std::string::npos)
                {
                    errorParsing("Error inside the server_name directive");
                    break;
                }
                if((pos = split[i].find(';')) != std::string::npos)
                {
                    cf.addServerName(split[i].substr(0, pos));
                    break;
                }
                cf.addServerName(split[i]);
                i++;
            }
            i++;
            continue;
        }
        if(split[i] == "}")
            break;
        i++;
    }
    return(cf);
}
