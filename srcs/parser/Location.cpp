#include "ServerConf.hpp"

Location::Location(){
    this->url = "";
    this->root = "";
    this->redirection = "";
    this->_directory_listing = 0;
    this->limit_body_size = 0;
    this->code_redirection = 0;
    this->_exact_match = 0;
    this->available_extension.push_back("py");
    this->available_extension.push_back("sh");
    this->_get = 1;
    this->_post = 0;
    this->_delete = 1;
    this->_root_check = 0;
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
        this->available_extension = rhs.available_extension;
        this->cgi = rhs.cgi;
        this->_get = rhs._get;
        this->_post = rhs._post;
        this->_delete = rhs._delete;
        this->_root_check = rhs._root_check;
    }
    return(*this);
}

Location::~Location(){
    
}


/* getters */

const uint64_t& Location::getLimitBodySize(void) const{
    return(this->limit_body_size);
}

const std::string& Location::getUrl(void) const{
    return(this->url);
}

const std::string& Location::getRoot(void) const{
    return(this->root);    
}

const std::string& Location::getRedirection(void) const{
    return(this->redirection);
}

const vec_string& Location::getIndexFile(void) const{
    return(this->index_file);
}

const int& Location::getRedirCode(void) const{
    return(this->code_redirection);
}

const bool& Location::getAutoIndex(void) const{
    return(this->_directory_listing);
}

const bool& Location::isExactMatch(void) const{
    return(this->_exact_match);
}

const bool& Location::getGetSatus(void) const{
    return(this->_get);
}

const bool& Location::getPostStatus(void) const{
    return(this->_post);
}

const bool& Location::getDeleteStatus(void) const{
    return(this->_delete);
}

const std::vector<std::pair<std::string, std::string> >& Location::getCgi(void) const{
    return(this->cgi);
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
    (void)root;
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
        this->url = "/";
        this->setIndexFile(url);

    }
    else
    {
        this->url = url;
        if(isfile)
        {
            this->setIndexFile(url.substr(url.find_last_of("/", url.size()), url.size()));
        }
    }
}

void Location::addRoot(const std::string &root){
    if(this->_root_check > 0)
        throw std::logic_error("Error:\nInvalid root directive inside Location block, root was already set once");
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
    this->_root_check++;
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
    this->index_file.push_back(file);
}

void Location::setCgi(const std::string &extension, const std::string &executable)
{
    if(std::find(this->available_extension.begin(), this->available_extension.end(), extension) == this->available_extension.end())
        throw std::logic_error("Error:\nUnknown extension found inside the cgi directive");
    if(executable.size() < extension.size() || executable.substr(executable.size() - extension.size()) != extension)
        throw std::logic_error("Error:\nIncorrect extension found inside the cgi directive");
    this->cgi.push_back(std::make_pair(extension, executable));
}

void Location::setMethod(const std::string &method, const std::string &status){
    bool stat;
    if(status == "on" || status == "off")
        stat = status == "on" ? 1 : 0;
    else throw std::logic_error("Error:\nUnknown parameter passed inside the set_method directive");
    if(method == "GET")
        this->_get = stat;
    else if(method == "POST")
        this->_post = stat;
    else if(method == "DELETE")
        this->_delete = stat;
    else throw std::logic_error("Error:\nUnknown method passed inside the set_method directive");
}

void Location::fixUrl(const std::string &url){
    std::string s;
    // std::cout << this->root << std::endl;
    if(this->root[this->root.size() - 1] == '/')
        s = this->root.substr(0, this->root.size() - 1);
    else
        s = this->root;
    // s = s.substr(s.find_last_of("/", s.size()), s.size() );
    this->url = url + s + this->url;
}

void Location::fixRoot(void){
    // if(this->root.size() > 0)
    // {
    //     std::string s = this->url;
    //     if(s[s.size() - 1] == '/')
    //     {
    //         s = s.substr(0, s.size() - 1);
    //     }
    //     this->root.insert(0, s);
    // }
}

void Location::fixIndexFile(void){
    for (size_t i = 0; i < this->index_file.size(); i++)
    {
        std::string s = this->url;
        if(this->index_file[i][0] == '/')
            this->index_file[i].erase(0, 1);
        if(s[s.size() - 1] != '/')
        {
            s = s.substr(0, s.find_last_of("/", s.size()) + 1);
        }
        this->index_file[i].insert(0, s);
    }
}