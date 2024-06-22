#include "Webserv.hpp"

void configureBasicServer(ServerConf &cf)
{
    static int i = 0;
    cf.addHost("127.0.0.1");
    cf.addPort("80");
    cf.addServerName("Webserv");
    std::vector<int> v;
    v.push_back(404);
    cf.addErrorPage("./html/basic_error_page.html", v);
    cf.setMainServerName();
    i++;
}

ServerConf parser(const vec_string &split, size_t &i, const size_t &size){

    ServerConf cf;
    size_t pos = 0;
    static size_t rank = 0;
    size_t index_brack = i;

    if (split[i] != "{" )
        throw std::logic_error("Error:\nMissing '{' of the server block");
    else if (( pos = split[i].find("}")) != std::string::npos)
        throw std::logic_error("Error:\nMisconfigured server block, issue comes from '}'");
    i++;
    if(split[i].find("{") != std::string::npos)
        throw std::logic_error("Error:\nMisconfigured server block, found \"{\" again");
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
            if(split[i].find_first_of("{}", 0) != std::string::npos)
            {
                goToNextIndex(split, i);
                throw std::logic_error("Error inside the listen directive");
            }
            if(i < size && split[i].find(';', 0) == std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                    throw std::logic_error("Error inside the listen directive , ';' not found");
            }
            cf.addPortOrHost(split[i]);
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
                    goToNextIndex(split, i);
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

        else if (split[i] == "error_page")
        {
            std::vector<int> vec;
            addErrorPagesNumber(vec, split, i);
            if((pos = split[i].find(";")) != std::string::npos)
                cf.addErrorPage(split[i].substr(0, pos), vec);
            else
            {
                cf.addErrorPage(split[i], vec);
                i++;
            }
            i++;
            continue;
            
        }
        
        else if (split[i] == "client_max_body_size")
        {
            i++;
            if(split[i].find_first_of("{}", 0) != std::string::npos)
            {
                errorParsing("Error inside the client_max_body_size directive");
                goToNextIndex(split, i);
                continue;
            }
            if(i < size && split[i].find(';', 0) == std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                {
                    errorParsing("Error inside the client_max_body_size directive, ';' not found");
                    continue;
                }
            }
            cf.addLimitBodySize(split[i]);
            i++;
            continue;
        }

        else if (split[i] == "location")
        {
            ParserLocation(split, i, size, cf);
            continue;
        }
        
        std::cout << split[i] << std::endl;
        if(split[i] == "}")
            break;
        i++;
    }
    cf.setMainServerName();
    cf.setRank(rank);
    rank++;
    return(cf);
}
