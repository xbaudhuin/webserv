#include "Location.hpp"

/* Basic Getters */

const std::string & Location::getAlias()const{
    return(this->alias);
}

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

std::string Location::myUri() const{
    return(this->_base_uri);
}

/* Special Getters */

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
            for (size_t i = 0; i < this->_use_extension.size(); i++)
            {
                std::cerr << "EXTENSIONS AVAILABLE :" << this->_use_extension[i] << std::endl;
                if(ex == this->_use_extension[i])
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
