#include "Location.hpp"

/* Constructors / Destructors */

Location::Location(){
    this->url = "";
    this->_is_generated = 0;
    this->root = "";
    this->_root_server = "";
    this->redirection = "";
    this->alias = "";
    this->_directory_listing = 0;
    this->limit_body_size = 0;
    this->code_redirection = 0;
    this->_exact_match = 0;
    this->available_extension.push_back(".py");
    this->available_extension.push_back(".php");
    this->available_extension.push_back(".rb");
    this->available_extension.push_back(".pl");
    this->_get = 1;
    this->_post = 1;
    this->_delete = 1;
    this->_root_check = 0;
    this->_alias_check = 0;
    this->upload_location = "";
    this->_is_a_dir = 0;
    this->_exec_path[".php"] = "/bin/php";
    this->_exec_path[".py"] = "/bin/python3";
    this->_exec_path[".rb"] = "/bin/ruby";
    this->_exec_path[".pl"] = "/bin/perl";
    this->_path_info = 0;
    this->_base_uri = "";
}

Location::Location(const Location &rhs){
    this->operator=(rhs);
}

Location::Location(const Location &rhs, int i){
    if(i == 0)
        this->operator=(rhs);
    else
    {
        this->url = rhs.index_file[0] + "/";
        vec_string s = rhs.index_file;
        s.erase(s.begin(), s.begin() + 1);
        this->index_file = s;
        if(this->index_file.size() == 0)
            this->index_file.push_back("/index.html");
        this->_is_generated = 1;
        this->root = rhs.root;
        this->redirection = rhs.redirection;
        this->code_redirection = rhs.code_redirection;
        this->_directory_listing = rhs._directory_listing;
        this->limit_body_size = rhs.limit_body_size;
        this->_exact_match = rhs._exact_match;
        this->available_extension = rhs.available_extension;
        this->cgi = rhs.cgi;
        this->_get = rhs._get;
        this->_post = rhs._post;
        this->_delete = rhs._delete;
        this->_root_check = rhs._root_check;
        this->_alias_check = rhs._alias_check;
        this->upload_location = rhs.upload_location;
        this->_is_a_dir = 1;
        this->_root_server = rhs._root_server;
        this->_exec_path = rhs._exec_path;
        this->_path_info = rhs._path_info;
        this->alias = rhs.alias;
        this->_base_uri = rhs._base_uri;
        this->_use_extension = rhs._use_extension;
    }
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
        this->_alias_check = rhs._alias_check;
        this->upload_location = rhs.upload_location;
        this->_is_a_dir = rhs._is_a_dir;
        this->_root_server = rhs._root_server;
        this->_exec_path = rhs._exec_path;
        this->_path_info = rhs._path_info;
        this->alias = rhs.alias;
        this->_is_generated = rhs._is_generated;
        this->_base_uri = rhs._base_uri;
        this->_use_extension = rhs._use_extension;
    }
    return(*this);
}

Location::~Location(){
    
}

/* Setters */

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

    if(s[0] != "/")
            throw std::logic_error("Error:\nInvalid URL passed in the location block");
    if(s.back() == "/")
    {
        isfile = 0;
    }
    else
    {
        isfile = 1;
    }
    if(isfile && !this->_exact_match)
    {
        this->url = "/";
        this->setIndexFile(url);
        this->_is_a_dir = 0;
    }
    else
    {
        this->url = url;
        this->_is_a_dir = 1;
        if(isfile)
        {
            this->setIndexFile(url.substr(url.find_last_of("/", url.size()), url.size()));
            this->_is_a_dir = 0;
        }
    }
}

void Location::addRoot(const std::string &dir){
    std::string check = dir;
    std::string save = check;
    if(this->hasAlias() || this->hasRoot())
        throw std::logic_error("Webserv: Error:\nAlias directive impossible to set since a root or alias was already set before");
    vec_string s = tokenizer(check, " ", "/");
    check = save;
    if(check[0] != '.' && check[1] && check[1] != '/')
        throw std::logic_error("Webserv: Error:\nRoot directive parameter is missing './' at the beginning");
    check.erase(0, 1);
    this->root = check;
    this->_root_check++;
}

void Location::addRedirection(const std::string &code, const std::string &redirect){
    size_t pos = code.find_first_not_of("0123456789", 0);
    if(pos != std::string::npos)
        throw std::logic_error("Error:\nCouldnt set redirection, invalid code passed as parameter");
    long int code_i = std::strtol(code.c_str(), NULL, 10);
    if(code_i < 300 || code_i > 308)
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

void Location::setPathInfo(const std::string &check)
{
    vec_string v = split(check, ";");
    if(v[0] == "on")
        this->_path_info = 1;
    else if(v[0] == "off")
        this->_path_info = 0;
    else
        throw std::logic_error("Error:\nCouln't set path_info, unkown parameter passed as argument");
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
    size_t i = 0;
    for (i = 0; i < this->_use_extension.size() ; i++)
    {
        if(this->_use_extension[i] == extension)
            break;
    }
    if (i == this->_use_extension.size())
        this->_use_extension.push_back(extension);
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
    if(this->hasAlias())
    {
        this->_root_server = this->alias;
    }
    else if(this->hasRoot())
    {
        this->_root_server = this->root;
    }
    else
        this->_root_server = url;
    this->_base_uri = this->url;
    if(!this->isADir() && !this->isExactMatch())
        this->_base_uri =this->index_file[0]; 
    if(this->_root_server[this->_root_server.size() - 1] == '/')
        s = this->_root_server.substr(0, this->_root_server.size() - 1);
    else
        s = this->_root_server;
    if(this->hasAlias())
    {
        this->url = this->_root_server;
        this->_root_server = s;
    }
    else
    {
        this->_root_server = s;
        this->url = s + this->url;
    }
    if(this->upload_location.size() > 0)
        this->upload_location.insert(0, s);
}

void Location::fixCgi(void){
    for (size_t i = 0; i < this->cgi.size(); i++)
    {
        if(this->cgi[i].second[0] == '/')
            this->cgi[i].second.erase(0, 1);
        this->cgi[i].second = this->url + this->cgi[i].second;
    }
    if(this->getPostStatus() && this->upload_location.size() == 0)
    {
        this->upload_location = this->_root_server + "/";
    }
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

std::string Location::getUploadLocation() const{
    return(this->upload_location);
}

void Location::setUploadLocation(const std::string &check)
{
    if (check[0] != '/')
        throw std::logic_error("Webserv: Error:\nupload_location parameter isn't a correct path, missing / at the begining");
    if (check[check.size() - 1] != '/')
        throw std::logic_error("Webserv: Error:\nupload_location parameter isn't a correct path, missing / at the end");
    this->upload_location = check;
}

bool Location::isRedirected(void) const{
    if(this->redirection.size() > 0 && this->code_redirection != 0)
        return(1);
    return(0);
}

const bool& Location::isADir(void) const{
    return(this->_is_a_dir);
}

const std::string& Location::getRootServer(void) const{
    return (this->_root_server);
}

void Location::addAlias(const std::string &dir){
    std::string check = dir;
    std::string save = check;
    if(this->hasAlias() || this->hasRoot())
        throw std::logic_error("Webserv: Error:\nAlias directive impossible to set since a root or alias was already set before");
    vec_string s = tokenizer(check, " ", "/");
    check = save;
    if(this->hasAlias() || this->hasRoot())
        throw std::logic_error("Webserv: Error:\nAlias directive impossible to set since a root or alias was already set before");
    if(check[0] != '.' && check[1] && check[1] != '/')
        throw std::logic_error("Webserv: Error:\nAlias directive parameter is missing './' at the beginning");
    check.erase(0, 1);
    this->alias = check;
    this->_alias_check++;
}

void Location::setBaseUri() {
    this->_base_uri = this->url;
}

void Location::fixFileLocationAlias(){
    if(this->index_file.size() == 1)
    {
        this->index_file.push_back("/index.html");
    }
}
