#ifndef ERROR_HPP
# define ERROR_HPP

#include <iostream>
#include <vector>

# define RED "\033[1;31m"
# define RESET "\033[0m"
# define PURP "\033[1;95m"
# define PURP2 "\033[1;35m"
# define ORANGE "\033[1;91m"
# define LIGHTB "\033[1;36m"
# define BLUE "\033[1;94m"
# define GREEN "\033[1;92m"
# define YELLOW "\033[1;93m"

typedef std::vector<std::string> vec_string;

template <typename F>
void writeInsideLog(const std::exception &e, F f)
{
    std::string message = e.what();
    f(message);
}

void errorParsing(const std::string &message);
void errorServer(const std::string &message);
void goToNextIndex(const vec_string &split, size_t &i);

#endif
