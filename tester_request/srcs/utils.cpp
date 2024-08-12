#include "tester_request.hpp"

vs split(const std::string &str, const std::string &charset)
{
    vs split;
    size_t pos = 0;
    size_t posend = 0;

    while(1)
    {
        if((pos = str.find_first_not_of(charset, pos)) == std::string::npos)
            break;
        posend = str.find_first_of(charset, pos);
        std::string to_add = str.substr(pos, posend - pos);
        split.push_back(to_add);
        if(posend == std::string::npos)
            break;
        pos = posend;
    }

    return(split);
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
