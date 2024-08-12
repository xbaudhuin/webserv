#include "tester_request.hpp"

int main(int ac, char **av)
{
    int PORT = 4243;
    char *host = NULL;
    if(ac < 2)
        return(printf("INVALID ARGS, AT LEAST 2\n"));
    if (ac == 2)
        host = av[1];
    else if (ac == 3)
    {
        host = av[1];
        PORT = atoi(av[2]);
    }
    for (size_t i = 0; i < 2; i++)
    {
        pid_t pid;
        pid = fork();
        if(pid < 0)
            return(printf("fork failed\n"));
        if (pid == 0)
        {
            if (i == 0)
            {
                execv((char*)"/bin/rm", (char *[]) {(char*)"rm", (char*)"-rf", (char*)"logs", NULL});
                return(printf("execv failure rm"));
            }
            else
            {
                execv((char*)"/bin/mkdir", (char *[]) {(char*)"mkdir", (char*)"logs", NULL});
                return(printf("execv failure mkdir"));
            }
        }
        wait(NULL);
    }
    vs v;
    std::ifstream	config;
	config.open("./tester.txt");
	if (!config.is_open())
        return(printf("Couldn't open file ./tester.txt\n"));
	std::stringstream strm;
	strm << config.rdbuf();
	std::string str = strm.str();
	config.close();
    
    vs s = split(str, "\n");
    for (size_t i = 0; i < s.size(); i++)
    {
        s[i] = ReplaceAll(s[i], "$", "\r\n");
        s[i] = ReplaceAll(s[i], "%", "\n");
    }
    vector<t_request> r;
    for (size_t i = 0; i < s.size(); i++)
    {
        v.push_back(s[i]);
        t_request req;
        req.host = host;
        req.PORT = PORT;
        req.request = v[i];
        req.sock = 0;
        req.valread = 0;
        req.i = i + 1;
        std::string file_log = "./logs/request_";
        std::stringstream ss;
        ss << req.i;
        file_log += ss.str();
        file_log += ".log";
        req.os = new std::ofstream();
        req.os->open(file_log.c_str(), ostream::trunc | ostream::out);
        r.push_back(req);
    }

    for (size_t i = 0; i < r.size(); i++)
    {
        if (pthread_create(&(r[i].thread), NULL, launch_request, &(r[i])))
            return(42);
    }
    for (size_t i = 0; i < r.size(); i++)
    {
        if (pthread_join(r[i].thread, NULL))
            return(42);
    }

    return (0);
}