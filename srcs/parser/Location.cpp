#include "ServerConf.hpp"

Location::Location(){
    this->url = "";
    this->root = "";
    this->redirection = "";
    this->_directory_listing = 0;
    this->limit_body_size = 0;
    this->code_redirection = 0;
    this->_exact_match = 0;
    this->index_file = "";
}

Location::Location(const Location &rhs){
    this->operator=(rhs);
}

Location& Location::operator=(const Location &rhs){
    if(this != &rhs)
    {
        this->url = rhs.url;
        this->root = rhs.root;
        this->redirection = rhs.redirection;
        this->code_redirection = rhs.code_redirection;
        this->_directory_listing = rhs._directory_listing;
        this->limit_body_size = rhs.limit_body_size;
        this->_exact_match = rhs._exact_match;
        this->index_file = rhs.index_file;
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

std::string Location::getIndexFile(void) const{
    return(this->index_file);
}

int Location::getRedirCode(void) const{
    return(this->code_redirection);
}

bool Location::getAutoIndex(void) const{
    return(this->_directory_listing);
}

bool Location::isExactMatch(void) const{
    return(this->_exact_match);
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
        throw std::logic_error("Error:\nIncorrect client_limit_body_size loc passed as parameter1");
    vec_string check = split(limit, ";");
    if(check.size() > 1)
        throw std::logic_error("Error:\nIncorrect client_limit_body_size loc passed as parameter2");
    pos = limit.find_first_of("kmgKMG", 0);
    if(pos != std::string::npos)
    {
        // std::cerr<< pos << " && " << check[0].size() << std::endl;
        if(pos == 0 || pos + 1 < check[0].size())
            throw std::logic_error("Error:\nIncorrect client_limit_body_size loc passed as parameter3");
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
}

void Location::addUrl(const std::string &url, std::string root){
    bool isfile = 0;
    size_t check = url.size();
    if(check <= 0)
        throw std::logic_error("Error:\nCouldnt set url, invalid path passed as parameter");
    std::string line = url;
    vec_string s = tokenizer(line, " ", "/");
    for (size_t i = 0; i < s.size(); i++)
    {
        if(i == 0)
        {
            if(s[i] != "/")
                throw std::logic_error("Error:\nInvalid URL passed in the location block");
            continue;
        }
        else if (i % 2 == 0)
        {
            isfile = 0;
            if(s[i] != "/")
                throw std::logic_error("Error:\nInvalid URL passed in the location block");
            continue;
        }
        isfile = 1;
    }
    if(isfile && !this->_exact_match)
    {
        this->url = (root.erase(0)) + "/";
        this->setIndexFile(root + url);
    }
    else
    {
        this->url = url;
        this->setIndexFile("");
    }
}

void Location::addRoot(const std::string &root){
    size_t check = root.size();
    if(check <= 0)
        throw std::logic_error("Error:\nCouldnt set root, invalid path passed as parameter");
    std::string s;
    check--;
    if(root[check] == ';')
        s = root.substr(0, check);
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
    check--;
    if(redirect[check] == ';')
        s = redirect.substr(0, check);
    else
        s = redirect;
    this->redirection = s;
    this->code_redirection = static_cast<int>(code_i);
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

void Location::setExactMatch(void){
    this->_exact_match = 1;
}

void Location::setIndexFile(const std::string &file)
{
    this->index_file = file;
}
