#include "./include/headers_hpp/Webserv.hpp"
#include <sys/wait.h>

void test (void)
{
    throw security_error("it works");
}

// std::string getDirectory(const std::string &uri)
// {
//     size_t pos = uri.size();
//     pos = uri.find_last_of("/", pos);
//     if(pos == std::string::npos)
//         return("./");
//     std::string s1 = uri.substr(0, pos + 1);
//     s1.insert(0, 1, '.');
//     return(s1);
// }

// std::string getFile(const std::string &uri)
// {
//     size_t pos = uri.size();
//     pos = uri.find_last_of("/", pos);
//     if(pos == std::string::npos)
//         return(std::string(uri));
//     std::string s1 = uri.substr(0, pos + 1);
//     if(s1 == uri)
//         throw std::logic_error("not a file");
//     std::string s2 = uri.substr(pos, uri.size());
//     s2.erase(0, 1);
//     return(s2);
// }

// std::string getExtension(const std::string& uri)
// {
//     size_t pos = uri.size();
//     pos = uri.find_last_of("/", pos);
//     std::string s1 = uri.substr(0, pos + 1);
//     if(s1 == uri)
//         throw std::logic_error("not a file");
//     std::string s2 = uri.substr(pos, uri.size());
//     std::vector<std::string> v(2);
//     v[0] = ".py";
//     v[1] = ".php";
//     std::string extension;
//     for (size_t i = 0; i < v.size(); i++)
//     {
//         if(s2.find(v[i], 0) != std::string::npos)
//             return(v[i].erase(0, 1));
//     }
//     throw std::logic_error("not a file");
//     return(s2);
// }

void child(char **argv, char **env, int i)
{
    if(!argv[i])
        throw cgiException("exiting child missing arg");
    std::string cmd = argv[1];
    std::string s = "/bin/" + cmd;
    if(i == 1)
    {
        argv[i] = NULL;
        std::string coucou  = argv[i];
    }
    char *tab[2] = {const_cast<char *>(s.c_str()), NULL};
    execve(s.c_str(), tab, env);
    throw cgiException("exiting child fail execve");
}

int main(int argc, char **argv, char**env)
{
    (void)argc;
  (void) argv;
    try
    {
        std::vector<char> s ;
        std::string str = "coucou je suis un chat";
        for (size_t i = 0; i < str.size(); i++)
        {
          s.push_back(str[i]);
        }
        for(size_t i = 0; i < s.size(); i++)
        {
          if(s[i] == 't')
            s.erase(s.begin() + i, s.begin() + i + 1);
        }
        
        std::cout << s << std::endl;
    }
    catch(const cgiException& e)
    {
        std::cerr << "bam " << e.what() << '\n';
    }
    catch(const std::exception& e)
    {
        std::cerr << "coucou " << e.what() << '\n';
    }
    
}
