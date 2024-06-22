#include "Webserv.hpp"

void ParserLocation(const vec_string &split, size_t &i,const size_t &size, ServerConf &cf)
{
    i++;
    if(split[i] == "{")
        return(errorParsing("Error:\nLocation directive missing the url"), goToNextIndex(split, i));
    Location loc;
    /* add in case of modifier */
    /* i++; */
    loc.addUrl(split[i]);
    i++;
    if(split[i] != "{")
        return(goToNextIndex(split, i), errorParsing("Error:\nLocation directive missing the '{' separator, skipping until we find the next '}'"));
    while(i < size)
    {
        if(split[i] == "root")
        {
            i++;
            if(split[i].find_first_of("{}", 0) != std::string::npos)
            {
                errorParsing("Error inside the root directive");
                goToNextIndex(split, i);
                continue;
            }
            if(i < size && split[i].find(';', 0) == std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                {
                    errorParsing("Error inside the root directive, ';' not found");
                    continue;
                }
            }
            try
            {
                loc.addRoot(split[i]);
                i++;
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
            }
            
        }
        else if(split[i] == "autoindex")
        {
            i++;
            if(split[i].find_first_of("{}", 0) != std::string::npos)
            {
                errorParsing("Error inside the autoindex directive");
                goToNextIndex(split, i);
                continue;
            }
            if(i < size && split[i].find(';', 0) == std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                {
                    errorParsing("Error inside the autoindex directive, ';' not found");
                    continue;
                }
            }
            try
            {
                loc.setAutoIndex(split[i]);
                i++;
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
            }
        }
        else if(split[i] == "return")
        {
            i++;
            std::cerr << split[i] << "&&" << split[i  + 1] << std::endl;
            if(split[i].find(';', 0) != std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                {
                    errorParsing("Error inside the return directive, ';' not found");
                    continue;
                }
            }
            i++;
            if(split[i].find_first_of("{}", 0) != std::string::npos)
            {
                errorParsing("Error inside the return directive");
                goToNextIndex(split, i);
                continue;
            }
            if(i < size && split[i].find(';', 0) == std::string::npos)
            {
                if(i + 1 < size && split[i + 1] != ";")
                {
                    errorParsing("Error inside the return directive, ';' not found");
                    continue;
                }
            }
            try
            {
                loc.addRedirection(split[i - 1], split[i]);
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
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
            try
            {
                loc.addLimitBodySize(split[i]);
                i++;
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
            }
            
            continue;
        }
        if(split[i] == "}")
            break;
        i++;
    }
    i++;
    cf.addLocation(loc);
}
