#include "Webserv.hpp"

int main(int argc, char **argv, char **env)
{
    (void)argc;
    int fd = open("./log/stderr.log", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd > 0)
    {
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
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
