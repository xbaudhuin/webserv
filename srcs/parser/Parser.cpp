#include "Webserv.hpp"

void goToNextServer(const vec_string &split, size_t &i, size_t &index ,const size_t &size)
{
    size_t brackets = 1;
    for (; index < size; i++)
    {
        if(split[i] == "{")
            brackets++;
        else if (split[i] == "}")
            brackets--;
        if(brackets == 0)
            break;
    }
}

int checkListen(const vec_string &split, size_t &i, const size_t &size, ServerConf &cf)
{
    if(split[i] == "listen")
    {
        i++;
        if(split[i].find_first_of("{}", 0) != std::string::npos)
        {
            goToNextIndex(split, i);
            throw std::logic_error("Error inside the listen directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Error inside the listen directive , ';' not found");
        }
        cf.addPortOrHost(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int checkServerName(const vec_string &split, size_t &i, const size_t &size, ServerConf &cf)
{
    if(split[i] == "server_name")
    {
        i++;
        while (i < size)
        {
            if(split[i].find_first_of("{}", 0) != std::string::npos)
            {
                throw std::logic_error ("Error inside the server_name directive");
            }
            if(split[i] == ";")
                break;
            cf.addServerName(split[i]);
            i++;
        }
        i++;
        return(1);
    }
    return(0);
}

int checkErrorPages(const vec_string &split, size_t &i, ServerConf &cf)
{
    if (split[i] == "error_page")
    {
        std::vector<int> vec;
        addErrorPagesNumber(vec, split, i);
        cf.addErrorPage(split[i], vec);
        i+=2;
        return(1);
    }
    return(0);
}

int checkMaxBodySize(const vec_string &split, size_t &i, const size_t &size, ServerConf &cf)
{
    if (split[i] == "client_max_body_size")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the client_max_body_size directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Error inside the client_max_body_size directive, ';' not found");
        }
        cf.addLimitBodySize(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int checkRoot(const vec_string &split, size_t &i, const size_t &size, ServerConf &cf)
{
    if (split[i] == "root")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the root directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Error inside the root directive, ';' not found");
        }
        cf.addRoot(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}



ServerConf parser(const vec_string &split, size_t &i, const size_t &size){

    ServerConf cf;
    size_t pos = 0;
    static size_t rank = 0;
    size_t index = i;

    if (split[i] != "{" )
        throw std::logic_error("Error:\nMissing '{' of the server block");
    else if (( pos = split[i].find("}")) != std::string::npos)
        throw std::logic_error("Error:\nMisconfigured server block, issue comes from '}'");
    i++;
    if(split[i].find_first_of("{};") != std::string::npos)
        throw std::logic_error("Error:\nMisconfigured server block, found \"{\" again");
    try
    {    
        while(i < size)
        {
            index = i;
            if(checkListen(split, i, size, cf))
            {
                continue;
            }
            else if(checkServerName(split, i, size, cf))
            {
                continue;
            }
            else if (checkErrorPages(split, i, cf))
            {
                continue;
            }
            else if (checkMaxBodySize(split, i ,size, cf))
            {
                continue;
            }
            else if (checkRoot(split, i, size, cf))
            {
                continue;
            }
            else if (split[i] == "location")
            {
                ParserLocation(split, i, size, cf);
                continue;
            }
            if(split[i] == "}")
                break;
            std::string s = "Error:\nUnknown directive found in the server block: ";
            s += split[i];
            throw std::logic_error(s);
        }
    }
    catch(const std::exception& e)
    {
        goToNextServer(split, i, index, size);
        throw;
    }
    cf.setMainServerName();
    cf.setRank(rank);
    cf.setRootToErrorPages();
    for (size_t i = 0; i < cf.getLocations().size(); i++)
    {
        cf.getLocations()[i].fixUrl(cf.getRoot());
        cf.getLocations()[i].fixCgi();
        cf.getLocations()[i].fixIndexFile();
    }
    
    rank++;
    return(cf);
}

void isSameVec_string(const vec_string &v, const vec_string &v1)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        for (size_t j = 0; j < v1.size(); j++)
        {
            if(v[i] == v1[j])
                throw std::logic_error("Webserv: Error:\nServer Config with same name and port found");
        }
    }
    
}

void Webserv::checkConfigs(void)
{
    for (size_t i = 0; i < this->_confs.size(); i++)
    {
        vec_string v = this->_confs[i].first;
        int port = this->_confs[i].second.getPort();
        for (size_t j = i + 1; j < this->_confs.size(); j++)
        {
            if(this->_confs[j].second.getPort() == port)
            {
                isSameVec_string(v, this->_confs[j].first);
            }
        }
        
    }
    
}
