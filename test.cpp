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

    try
    {
        pid_t f = fork();
        if(f == -1)
            return(1);
        else if(f == 0)
            child(argv, env, 1);
        wait(NULL);
        pid_t f1 = fork();
        if(f1 == -1)
            return(1);
        else if(f1 == 0)
            child(argv, env, 2);
        wait(NULL);
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

void Client::findPages(const std::string &urlu) {
  (void)urlu;
  std::string url = "." + _location->getRootServer() + _sUri;
  if (_location->isADir() == true) {
    std::cout << RED << "Location is a Dir" << RESET << std::endl;
    std::cout << PURP2 << "_sUri = " << _sUri << " && location = " << _location->getUrl() <<std::endl;
    if (_sUri[_sUri.size() - 1] == '/') {
      if (findIndex(url) == false) {
        return (buildListingDirectory(url));
      }
    }
  } else if (_location->isExactMatch() == false) {
    if (findIndex(url) == false) {
      std::cout << *_location << BLUE << "no exact match, index == false;"
                << RESET << std::endl;
      _statusCode = 404;
      return;
    }
  }
  _sPath = url;
  _file.open(url.c_str(), std::ios::in);
  std::cout << RED << "trying to open file: " << url << RESET << std::endl;
  if (_file.is_open() == false) {
    std::cout << RED << "failed to open file: " << url << RESET << std::endl;
    if (access(url.c_str(), F_OK) == -1)
      _statusCode = 404;
    else
      _statusCode = 403;
    return;
  }
  struct stat st;
  stat(_sPath.c_str(), &st);
  std::cout << YELLOW << "size of file: " << st.st_size << RESET << std::endl;
  _leftToRead = st.st_size;
  _response.setHeader("Content-Length", st.st_size);
  readFile();
}