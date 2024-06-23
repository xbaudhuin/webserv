#include "Webserv.hpp"

void goToNextIndex(const vec_string &split, size_t &i){
    size_t pos = 0;
    while (1)
    {
        pos = split[i].find("}", 0);
        std::cerr << split[i] << std::endl;
        if(pos != std::string::npos)
            break;
        i++;
    }
}

void addErrorPagesNumber(std::vector<int> &vec, const vec_string &split, size_t &i)
{
    size_t size = split.size();
    char *ptr = NULL;
    size_t pos = 0;
    i++;
    while(i < size)
    {
        if(split[i] == ";")
        {
            i--;
            break;
        }
        else if (split[i].find_first_of("{}", 0) != std::string::npos)
        {
            throw std::logic_error("Error inside the error_page directive");
        }
        else if (split[i].find(";", 0) != std::string::npos || (i + 1 < size  && split[i + 1] == ";"))
        {
            break;
        }

        if(split[i][0] == '=')
            ;
        else if((pos = split[i].find_first_not_of("0123456789", 0)) != std::string::npos)
        {
            throw std::logic_error("Error inside the error codes, non numerical characters found");
        }
        long int code = std::strtol(split[i].c_str(), &ptr, 10);
        if(code < 100 || code > 599)
        {
            std::string s = ptr;
            std::string s2 = "Invalid error code, skipping it :" + s;
            errorParsing(s2);
        }
        vec.push_back(static_cast<int>(code));
        i++;
    }
}
