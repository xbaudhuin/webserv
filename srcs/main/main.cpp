#include "Webserv.hpp"

int main(int argc, char **argv, char **env)
{
    (void)argc;
    Webserv serv;
    try
    {
        serv.addEnv(env);
        std::string config;
        if(argv[1])
            config = argv[1];
        else
            config = "./config/good_config/test.conf";
        serv.parseConfig(config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
