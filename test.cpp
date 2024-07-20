#include "./include/headers_hpp/Webserv.hpp"

void test (void)
{
    throw security_error("it works");
}

std::string getDirectory(const std::string &uri)
{
    size_t pos = uri.size();
    pos = uri.find_last_of("/", pos);
    if(pos == std::string::npos)
        return("./");
    std::string s1 = uri.substr(0, pos + 1);
    s1.insert(0, 1, '.');
    return(s1);
}

std::string getFile(const std::string &uri)
{
    size_t pos = uri.size();
    pos = uri.find_last_of("/", pos);
    if(pos == std::string::npos)
        return(std::string(uri));
    std::string s1 = uri.substr(0, pos + 1);
    if(s1 == uri)
        throw std::logic_error("not a file");
    std::string s2 = uri.substr(pos, uri.size());
    s2.erase(0, 1);
    return(s2);
}

std::string getExtension(const std::string& uri)
{
    size_t pos = uri.size();
    pos = uri.find_last_of("/", pos);
    std::string s1 = uri.substr(0, pos + 1);
    if(s1 == uri)
        throw std::logic_error("not a file");
    std::string s2 = uri.substr(pos, uri.size());
    std::vector<std::string> v(2);
    v[0] = ".py";
    v[1] = ".php";
    std::string extension;
    for (size_t i = 0; i < v.size(); i++)
    {
        if(s2.find(v[i], 0) != std::string::npos)
            return(v[i].erase(0, 1));
    }
    throw std::logic_error("not a file");
    return(s2);
}

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    try
    {
        std::string s = getExtension("/coucou.py");
        std::cout << "Test: " << getFile("/html/wtf/coucou.py") << std::endl;
        std::string dir = getDirectory("/html/cgi/coucou.py");
        std::cout << "Test: " << dir << std::endl;
        if(chdir(dir.c_str()) == 0)
            std::cout << "Success" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "coucou " << e.what() << '\n';
    }
    
}