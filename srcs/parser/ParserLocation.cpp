#include "Webserv.hpp"

int testRoot(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "root")
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
        loc.addRoot(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testRedir(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "return")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the return directive, wrong format");
        }
        i++;
        if(split[i].find_first_of(";{}", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the return directive, wrong format");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Error inside the return directive, ';' not found");
        }
        loc.addRedirection(split[i - 1], split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testAutoIndex(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "autoindex")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the autoindex directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Error inside the autoindex directive, ';' not found");
        }
        loc.setAutoIndex(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testMaxBodySize(const vec_string &split, size_t &i, const size_t &size, Location &loc)
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
        loc.addLimitBodySize(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testIndex(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if (split[i] == "index")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the index directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Error inside the index directive, ';' not found");
        }
        loc.setIndexFile(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

void ParserLocation(const vec_string &split, size_t &i,const size_t &size, ServerConf &cf)
{
    i++;
    if(split[i] == "{")
        return(errorParsing("Error:\nLocation directive missing the url"), goToNextIndex(split, i));
    Location loc;
    /* add in case of modifier */
    /* i++; */
    if(split[i] == "=")
    {
        loc.setExactMatch();
        i++;
    }
    
    loc.addUrl(split[i], cf.getRoot());
    i++;
    if(split[i] != "{")
        throw std::logic_error("Error:\nLocation directive missing the '{' separator, skipping until we find the next '}'");
    i++;
    while(i < size)
    {
        if(testRoot(split, i, size, loc))
        {
            continue;
        }
        else if(testAutoIndex(split, i, size, loc))
        {
            continue;
        }
        else if(testRedir(split, i, size, loc))
        {
            continue;
        }
        else if (testMaxBodySize(split, i, size, loc))
        {
            continue;
        }
        else if (testIndex(split, i, size, loc))
        {
            continue;
        }
        
        if(split[i] == "}")
            break;
        i++;
    }
    i++;
    cf.addLocation(loc);
}
