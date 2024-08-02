#include "ServerConf.hpp"

void check_if_safe(const std::string &s, const bool& is_file)
{
    vec_string security_check = split(s, "/");
    long long int count = 0;
    for (size_t i = 0; i < security_check.size(); i++)
    {
        if(is_file && i== security_check.size() -1)
            break;
        if(security_check[i] == "..")
            count--;
        else
            count++;
        if (count < 0)
            break;
    }
    if(count < 0)
        throw security_error("BIG ISSUE");
}

Location& ServerConf::getPreciseLocation(const std::string &url)
{
    bool is_file = 0;
    size_t size = this->_locations.size();
    std::string s = url;
    std::cout << RED << "URI: " << s << " && root: " << this->root << " && param: " << url << RESET << std::endl;
    for (size_t i = 0; i < size; i++)
    {
        if(this->_locations[i].isExactMatch() && this->_locations[i].myUri() == s)
        {
#if PRINT == 2
            std::cout << YELLOW << "✨ JACKPOT ✨" << RESET << std::endl;
            std::cout << RED << "FOUND IN PRECISE LOCATION" << std::endl;
            std::cout << s << RESET << std::endl;
#endif
            return(this->_locations[i]);
        }
    }
    size_t pos = s.size();
    pos = s.find_last_of("/", pos);
    std::string s1 = s.substr(0, pos + 1);
    std::string s2;
    if(s1 == s)
    {
#if PRINT == 2
        std::cout << BLUE << "TWINNING !" << RESET << std::endl;
#endif   
    }
    else
    {
        is_file = 1;
        s2 = s.substr(pos, s.size());
#if PRINT == 2
        std::cout << GREEN << "EWW A FILE!" << std::endl;
        std::cout << "This is the ugly file: " << s2 << RESET << std::endl;
#endif
    }
    if(is_file)
    {
        size_t pos2 = pos;
        while (pos2 != std::string::npos)
        {   
            pos2 = s.find_last_of("/", pos2);
            std::string s1 = s.substr(0, pos2 + 1);
            std::cout << PURP "HERE IDIOT: " << s1 << " && pos: " << pos2 << std::endl;
            for (size_t i = 0; i < size; i++)
            {
                std::cout << this->_locations[i].myUri() << RESET << std::endl;
                if(!this->_locations[i].isExactMatch() && !this->_locations[i].isADir() && this->_locations[i].myUri() == s1)
                {
                    std::string uri = this->_locations[i].getIndexFile()[0];
                    std::string file = uri.substr(uri.find_last_of("/", uri.size()), uri.size());
                    if(file == s2)
                    {
#if PRINT == 2
                        std::cout << PURP << "NICE, FOUND IN FILE LOCATION!" << std::endl;
#endif
                        return(this->_locations[i]);
                    }
                    for (size_t z = 0; z < this->_locations[i].getCgi().size(); z++)
                    {
                        std::string uri = this->_locations[i].getCgi()[z].second;
                        std::string file = uri.substr(uri.find_last_of("/", uri.size()), uri.size());
                        if(file == s2)
                        {
#if PRINT == 2
                            std::cout << PURP << "NICE, FOUND IN FILE CGI LOCATION!" << std::endl;
#endif
                            return(this->_locations[i]);
                        }
                    }
                }
            }
            pos2--;
            if(pos2 <= 0)
                break;
        }
    }
    while (pos != std::string::npos)
    {   
        pos = s.find_last_of("/", pos);
        std::string s1 = s.substr(0, pos + 1);
        std::cout << YELLOW "HERE IDIOT 2: " << s1 << " && pos: " << pos << std::endl;
        for (size_t i = 0; i < size; i++)
        {
            std::cout << this->_locations[i].myUri() << RESET << std::endl;
            if(!this->_locations[i].isExactMatch() && this->_locations[i].isADir() && this->_locations[i].myUri() == s1)
            {
#if PRINT == 2
                std::cout << RED << "FOUND IN NORMAL LOCATION" << std::endl;
                std::cout << s1 << RESET << std::endl;
#endif
                return(this->_locations[i]);
            }
        }
        pos--;
        if(pos <= 0)
            break;
    }
    throw std::logic_error("you idiot sandwich");
    return(this->_locations[0]);
}
