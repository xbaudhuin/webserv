#ifndef ERROR_HPP
# define ERROR_HPP

#include <iostream>

template <typename F>
void writeInsideLog(const std::exception &e, F f)
{
    std::string message = e.what();
    f(message);
}

void errorParsing(const std::string &message);
void errorServer(const std::string &message);

#endif
