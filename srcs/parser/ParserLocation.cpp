#include "ServerConf.hpp"
#include "Error.hpp"

int testRoot(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "root")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the root directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the root directive, ';' not found");
        }
        loc.addRoot(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testAlias(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "alias")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the alias directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the alias directive, ';' not found");
        }
        loc.addAlias(split[i]);
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
            throw std::logic_error("Webserv: Error:\nSyntax error inside the return directive, wrong format");
        }
        i++;
        if(split[i].find_first_of(";{}", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the return directive, wrong format");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the return directive, ';' not found");
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
            throw std::logic_error("Webserv: Error:\nSyntax error inside the autoindex directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the autoindex directive, ';' not found");
        }
        loc.setAutoIndex(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}


int testPathInfo(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "path_info")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the path_info directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the path_info directive, ';' not found");
        }
        loc.setPathInfo(split[i]);
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
            throw std::logic_error("Webserv: Error:\nSyntax error inside the client_max_body_size directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the client_max_body_size directive, ';' not found");
        }
        loc.addLimitBodySize(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testIndex(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    (void)size;
    if (split[i] == "index")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the index directive");
        }
        // i++;
        while(split[i] != ";")
        {
            if(split[i].find_first_of("{}", 0) != std::string::npos)
                throw std::logic_error("Webserv: Error:\nSyntax error inside the set_method directive, no ';' found");
            loc.setIndexFile(split[i]);
            i++;
        }
        i++;
        return(1);
    }
    return(0);
}

int testCgi(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if(split[i] == "cgi")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the cgi directive, wrong format");
        }
        i++;
        if(split[i].find_first_of(";{}", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the cgi directive, wrong format");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the cgi directive, ';' not found");
        }
        loc.setCgi(split[i - 1], split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testUploadLocation(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    if (split[i] == "upload_location")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the upload_location directive");
        }
        if(i + 1 < size && split[i + 1] != ";")
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the upload_location directive, ';' not found");
        }
        loc.setUploadLocation(split[i]);
        i+=2;
        return(1);
    }
    return(0);
}

int testSet_method(const vec_string &split, size_t &i, const size_t &size, Location &loc)
{
    (void)size;
    if(split[i] == "set_method")
    {
        i++;
        if(split[i].find_first_of("{};", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the set_method directive, wrong format");
        }
        i++;
        if(split[i].find_first_of(";{}", 0) != std::string::npos)
        {
            throw std::logic_error("Webserv: Error:\nSyntax error inside the set_method directive, wrong format");
        }
        int j = i;
        i--;
        while(split[j] != ";")
        {
            if(split[j].find_first_of("{}", 0) != std::string::npos)
                throw std::logic_error("Webserv: Error:\nSyntax error inside the set_method directive, no ';' found");
            j++;
        }
        j--;
        while (split[i] != split[j])
        {
            loc.setMethod(split[i], split[j]);
            i++;
        }
        i+=2;
        return(1);
    }
    return(0);
}

void ParserLocation(const vec_string &split, size_t &i,const size_t &size, ServerConf &cf)
{
    i++;
    if(split[i] == "{")
        return(errorParsing("Webserv: Error:\nSyntax error: Location directive missing the url"), goToNextIndex(split, i));
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
        throw std::logic_error("Webserv: Error:\nSyntax error: Location directive missing the '{' separator, skipping until we find the next '}'");
    i++;
    while(i < size)
    {
        if(testRoot(split, i, size, loc))
        {
            continue;
        }
        else if(testAlias(split, i, size, loc))
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
        else if(testCgi(split, i, size, loc))
        {
            continue;
        }
        else if(testSet_method(split, i, size, loc))
        {
            continue;
        }
        else if (testUploadLocation(split, i, size, loc))
        {
            continue;
        }
        else if(testPathInfo(split, i, size, loc))
        {
            continue;
        }
        if(split[i] == "}")
            break;
        std::string s = "Webserv: Error:\nUnknown directive found in the server block: ";
        s += split[i];
        throw std::logic_error(s);
        i++;
    }
    i++;
    if(loc.getIndexFile().size() == 0)
        loc.setIndexFile("/index.html");
    if(loc.getCgi().size() == 0 && loc.getUploadLocation().size() > 0)
        throw std::logic_error("Webserv: Error:\nupload_location set without any cgi specified");
    if(!loc.getCgi().size() && loc.hasPathInfo())
        throw std::logic_error("Webserv: Error:\npath_info set without any cgi specified");
    // std::cout << loc << std::endl;
    cf.addLocation(loc);
}
