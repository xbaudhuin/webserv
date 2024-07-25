#include "Webserv.hpp"

void errorParsing(const std::string &message)
{
    static size_t i = 0;
    std::ofstream os;
    static const char *s = "./log/parsing_error.log";
    if(i == 0)
    {
        os.open(s, std::ostream::trunc | std::ostream::out);
        if(os.is_open())
        {
            os << message << std::endl;
            os.close();
        }
        else
        {
            std::cerr << "Error\nCouldn't open parsing_error.log and therefore we'll write in the stderr instead:\n" << message << std::endl;
        }
    }
    else
    {
        os.open(s, std::ostream::app | std::ostream::out);
        if(os.is_open())
        {
            os << message << std::endl;
            os.close();
        }
        else
            std::cerr << message << std::endl;
    }
    i++;
}

void errorServer(const std::string &message)
{
    static size_t i = 0;
    std::ofstream os;
    static const char *s = "./log/server_error.log";
    if(i == 0)
    {
        os.open(s, std::ostream::trunc | std::ostream::out);
        if(os.is_open())
        {
            os << message << std::endl;
            os.close();
        }
        else
        {
            std::cerr << "Error\nCouldn't open server_error.log and therefore we'll write in the stderr instead:\n" << message << std::endl;
        }
    }
    else
    {
        os.open(s, std::ostream::app | std::ostream::out);
        if(os.is_open())
        {
            os << message << std::endl;
            os.close();
        }
        else
            std::cerr << message << std::endl;
    }
    i++;
}

void errorChild(const std::string &message)
{
    static size_t i = 0;
    std::ofstream os;
    static const char *s = "./log/child_error.log";
    if(i == 0)
    {
        os.open(s, std::ostream::trunc | std::ostream::out);
        if(os.is_open())
        {
            os << message << std::endl;
            os.close();
        }
        else
        {
            std::cerr << "Error\nCouldn't open child_error.log and therefore we'll write in the stderr instead:\n" << message << std::endl;
        }
    }
    else
    {
        os.open(s, std::ostream::app | std::ostream::out);
        if(os.is_open())
        {
            os << message << std::endl;
            os.close();
        }
        else
            std::cerr << message << std::endl;
    }
    i++;
}
