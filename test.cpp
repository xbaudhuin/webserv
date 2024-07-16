#include "./include/headers_hpp/Webserv.hpp"

void test (void)
{
    throw security_error("it works");
}

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    try
    {
        test();
    }
    catch(const std::exception& e)
    {
        std::cerr << "coucou " << e.what() << '\n';
    }
    
}