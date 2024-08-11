#include "Error.hpp"
#include "Webserv.hpp"
#include <ostream>

const char *childLog = "./log/child_log.log";
const char *parsingError = "./log/parsing_error.log";
const char *serverLog = "./log/server_log.log";
const char *error = "./log/stderr.log";
const char *childError = "./log/child_stderr.log";

void errorParsing(const std::string &message)
{
    static size_t i = 0;
    std::ofstream os;
    if(i == 0)
    {
        os.open(parsingError, std::ostream::trunc | std::ostream::out);
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
        os.open(parsingError, std::ostream::app | std::ostream::out);
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
    if(i == 0)
    {
        os.open(serverLog, std::ostream::trunc | std::ostream::out);
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
        os.open(serverLog, std::ostream::app | std::ostream::out);
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
    if(i == 0)
    {
        os.open(childLog, std::ostream::trunc | std::ostream::out);
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
        os.open(childLog, std::ostream::app | std::ostream::out);
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
