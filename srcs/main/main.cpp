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
    try
    {
        Webserv serv(argv[1]);
        serv.addEnv(env);
		serv.start();
    }
    catch(const std::runtime_error &e)
    {
        return (1);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
