#include "Location.hpp"

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
    this->_post = 0;
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

const bool& Location::hasPathInfo(void) const{
    return(this->_path_info);
}

const bool& Location::isGenerated() const{
    return(this->_is_generated);
}

const bool& Location::isExactMatch(void) const{
    return(this->_exact_match);
}

const bool& Location::getGetStatus(void) const{
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

const vec_string& Location::availableExtension() const{
    return(this->available_extension);
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

void Location::fixUrl(const std::string &url){
    std::string s;
    // std::string uri = url;
    if(this->hasAlias())
    {
        this->_root_server = this->alias;
        // std::cout << this->alias << std::endl;
    }
    else if(this->hasRoot())
    {
        this->_root_server = this->root;
        // std::cout << this->root << std::endl;
    }
    else
        this->_root_server = url;
    this->_base_uri = this->url;
    if(!this->isADir() && !this->isExactMatch())
        this->_base_uri =this->index_file[0]; 
    std::cout << "HERE MATE: " << this->_root_server << " && " << this->url << std::endl;
    if(this->_root_server[this->_root_server.size() - 1] == '/')
        s = this->_root_server.substr(0, this->_root_server.size() - 1);
    else
        s = this->_root_server;
    // s = s.substr(s.find_last_of("/", s.size()), s.size() );
    if(this->hasAlias())
    {
        // this->_root_server = this->_root_server;
        // if(this->_root_server[this->_root_server.size() - 1] == '/')
        //     this->url = this->_root_server + "/";
        // else
        this->url = this->_root_server;
        this->_root_server = s;
        std::cout << "HERE ALIAS: "<< this->url <<std::endl;
    }
    else
    {
        this->_root_server = s;
        this->url = s + this->url;
        std::cout << "HERE NORMAL:" << this->url <<std::endl;
        std::cout << "HERE NORMAL 2:" << s <<std::endl;
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
        throw std::logic_error("Webserv: Error:\nupload_location parameter isn't a correct path, missing /");
    if (check[check.size() - 1] != '/')
        throw std::logic_error("Webserv: Error:\nupload_location parameter isn't a correct path, missing /");
    this->upload_location = check;
}

std::ostream& operator<<(std::ostream& out, const Location& loc){
    if(loc.getUrl().size() >  0)
        out << "Url: " << loc.getUrl() << "\n\t";
    out << "Server Root: " << loc.getRootServer() << "\n\t";
    out << "Exact Match: " << (loc.isExactMatch() ? "YES" : "NO") << "\n\t";
    out << "Base Uri: " << loc.myUri() << "\n\t";
    if(loc.hasRoot())
        out << "Root: " << loc.getRoot() << "\n\t";
    if(loc.hasAlias())
        out << "Alias: " << loc.getAlias() << "\n\t";
    if(loc.getIndexFile().size() > 0)
        for (size_t j = 0; j < loc.getIndexFile().size(); j++)
        {
            out << "Index File[" << j << "]: " << loc.getIndexFile()[j] << "\n\t";
        }
    if(loc.getCgi().size() > 0)
    {
        for (size_t j = 0; j < loc.getCgi().size(); j++)
        {
            out << "Cgi File: " << loc.getCgi()[j].second << "\n\t";
        }
    }
    if(loc.getUploadLocation().size() > 0)
        out << "Cgi Upload Location: " << loc.getUploadLocation() << "\n\t";
    if(loc.getRedirection().size() > 0)
        out << "Redirection URL and CODE: " << loc.getRedirection() << " && " << loc.getRedirCode() << "\n\t";
    out << "Path info: " << (loc.hasPathInfo() ? "ON" : "OFF") << "\n\t";
    out << "Limit body size: " << loc.getLimitBodySize() << "\n\t";
    out << "Method GET status: " << (loc.getGetStatus() ? "on" : "off") << "\n\t";
    out << "Method POST status: " << (loc.getPostStatus() ? "on" : "off") << "\n\t";
    out << "Method DELETE status: " << (loc.getDeleteStatus() ? "on" : "off") << "\n\t";
    out << "Directory Listing Status: " << loc.getAutoIndex() << std::endl;
    return(out);
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

std::string Location::getCgiPath(const std::string &uri) const{
    try
    {
        std::string uri_file = getFile(uri);
        std::string ext = getExtension(uri);
        std::cerr << BLUE << "UriFile: " << uri_file << " && extension: " << ext << RESET << std::endl;
        for (size_t i = 0; i < this->cgi.size(); i++)
        {
            if(this->cgi[i].first == ext)
            {
                std::string file = getFile(this->cgi[i].second);
                if(uri_file == file)
                    return(getDirectory(this->cgi[i].second));
            }
        }
        std::string s = getDirectory(uri);
        std::cerr << this->_root_server << " && " << s << std::endl;
        s = s.erase(0, 1);
        return("." + this->_root_server + s);
    }
    catch(const bad_key_error& e)
    {
        return(getDirectory(uri));
    }
    catch(const std::exception& e)
    {
        return(getDirectory(this->cgi[0].second));
    }
    
    return("");
}

std::string Location::getCgiFile(const std::string& uri) const{
    try
    {
        std::string uri_file = getFile(uri);
        std::string ext = getExtension(uri);
        std::cerr << BLUE << "UriFile: " << uri_file << " && extension: " << ext << RESET << std::endl;
        for (size_t i = 0; i < this->cgi.size(); i++)
        {
            if(this->cgi[i].first == ext)
            {
                std::string file = getFile(this->cgi[i].second);
                if(uri_file == file)
                    return(file);
            }
        }
        return(uri_file);
    }
    catch(const std::exception& e)
    {
        return(getFile(this->cgi[0].second));
    }
    
    return("");
}

std::string Location::getExtension(const std::string& uri) const
{
    size_t pos = uri.size();
    pos = uri.find_last_of("/", pos);
    std::string s1 = uri.substr(0, pos + 1);
    if(s1 == uri)
        throw std::logic_error("not a file");
    std::string s2 = uri.substr(pos, uri.size());
    std::string extension;
    for (size_t i = 0; i < this->available_extension.size(); i++)
    {
        if(s2.find(this->available_extension[i], 0) != std::string::npos)
            return(this->available_extension[i]);
    }
    throw bad_key_error("not a file");
    return(s2);
}

bool Location::isCgi(const std::string& uri) const{
    try
    {
        std::string ex = this->getExtension(uri);
        std::string uri_file = getFile(uri);
        std::cerr << BLUE << "Extensions: " << ex << RESET << std::endl;
        if(this->cgi.size() > 0)
        {
            for (size_t i = 0; i < this->available_extension.size(); i++)
            {
                if(ex == this->available_extension[i])
                    return(1);
            }        
        }
    }
    catch(const bad_key_error& e)
    {
        return(0);
    }
    catch(const std::exception& e)
    {
        if(this->cgi.size() > 0)
            return(1);
    }
    return(0);
}

const std::string& Location::getExecutePath(const std::string& uri){
    try
    {
        std::string ex = this->getExtension(uri);
        return(this->_exec_path[ex]);
    }
    catch(const std::exception& e)
    {
        std::string ex = this->cgi[0].first;
        return(this->_exec_path[ex]);
    }
    return(uri);
}

const int &Location::hasAlias()const{
    return(this->_alias_check);
}

const int &Location::hasRoot()const{
    return(this->_root_check);
}

const std::string & Location::getAlias()const{
    return(this->alias);
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

std::string Location::myUri() const{
    // std::string s = this->_root_server;
    // if(s[s.size() - 1] == '/')
        // s = s.substr(0, s.size() - 1);
    return(this->_base_uri);
}

void Location::fixFileLocationAlias(){
    if(this->index_file.size() == 1)
    {
        this->index_file.push_back("/index.html");
    }
}
