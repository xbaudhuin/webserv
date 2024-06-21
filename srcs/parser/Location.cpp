#include "ServerConf.hpp"

Location::Location(){
    this->url = "";
    this->root = "";
    this->redirection = "";
    this->_directory_listing = 0;
    this->limit_body_size = 0;
}

Location::Location(const Location &rhs){

}

Location& Location::operator=(const Location &rhs){
    if(this != &rhs)
    {
        this->url = rhs.url;
        this->root = rhs.root;
        this->redirection = rhs.redirection;
        this->_directory_listing = rhs._directory_listing;
        this->limit_body_size = rhs.limit_body_size;
    }
    return(*this);
}

Location::~Location(){
    
}


/* getters */

uint64_t Location::getLimitBodySize(void) const{
    return(this->limit_body_size);
}

std::string Location::getUrl(void) const{
    return(this->url);
}

std::string Location::getRoot(void) const{
    return(this->root);    
}

std::string Location::getRedirection(void) const{
    return(this->redirection);
}

bool Location::getAutoIndex(void) const{
    return(this->_directory_listing);
}


/* setters */

void Location::addLimitBodySize(const std::string &limit)
{
    std::string value;
    uint64_t to_multiply = 1;
    size_t pos = 0;
    char c = '\0';
    pos = limit.find_first_not_of("0123456789kmgKMG;", 0);
    if(pos != std::string::npos)
        throw std::logic_error("Error:\nIncorrect client_limit_body_size passed as parameter1");
    vec_string check = split(limit, ";");
    if(check.size() > 1)
        throw std::logic_error("Error:\nIncorrect client_limit_body_size passed as parameter2");
    pos = limit.find_first_of("kmgKMG", 0);
    if(pos != std::string::npos)
    {
        std::cerr<< pos << " && " << check[0].size() << std::endl;
        if(pos == 0 || pos + 1 < check[0].size())
            throw std::logic_error("Error:\nIncorrect client_limit_body_size passed as parameter3");
        c = limit[pos];
    }
    else
        pos = limit.find_first_of(";", 0);
    value = limit.substr(0, pos);
    switch (c)
    {
        case 'k':
        case 'K':
            to_multiply *= 1024;
            break;
        case 'm':
        case 'M':
            to_multiply *= (1024 *1024);
            break;
        case 'g':
        case 'G':
            to_multiply *= (1024 *1024 *1024);
            break;
        default:
            break;
    }
    this->limit_body_size = static_cast<uint64_t>(std::strtoull(value.c_str(), NULL, 10)) * to_multiply;
    std::cout << "limit body size: " << this->limit_body_size << std::endl; 
}

void Location::addUrl(const std::string &url){
    size_t check = url.size();
    if(check <= 0)
        throw std::logic_error("Error:\nCouldnt set url, invalid path passed as parameter");
    std::string s;
    if(url[check] == ';')
        s = url.substr(0, check - 1);
    else
        s = url;
    this->url = s;
}

void Location::addRoot(const std::string &root){
    size_t check = root.size();
    if(check <= 0)
        throw std::logic_error("Error:\nCouldnt set root, invalid path passed as parameter");
    std::string s;
    if(root[check] == ';')
        s = root.substr(0, check - 1);
    else
        s = root;
    this->root = s;
}

void Location::addRedirection(const std::string &code, const std::string &redirect){
    size_t pos = code.find_first_not_of("0123456789", 0);
    if(pos != std::string::npos)
        throw std::logic_error("Error:\nCouldnt set redirection, invalid code passed as parameter");
    long int code_i = std::strtol(code.c_str(), NULL, 10);
    if(code_i < 300 || code_i > 320)
        throw std::logic_error("Error:\nCouldnt set redirection, invalid code passed as parameter");
    size_t check = redirect.size();
    if(check <= 0)
        throw std::logic_error("Error:\nCouldnt set redirection, invalid path passed as parameter");
    std::string s;
    if(redirect[check] == ';')
        s = redirect.substr(0, check - 1);
    else
        s = redirect;
    this->redirection = s;
}

void Location::setAutoIndex(const std::string &check)
{
    vec_string v = split(check, ";");
    if(v[0] == "on")
        this->_directory_listing = 1;
    else if(v[0] == "off")
        this->_directory_listing = 0;
    else
        throw std::logic_error("Error:\nCouln't set directory listing, unkown parameter passed as argument");
}

