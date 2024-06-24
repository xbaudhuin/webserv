#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <vector>

class Location
{
    private:
        std::string url;
        /* GET */
        /* POST */
        /* DELETE */
        /* FAST_CGI */
        std::vector<std::string> available_extension;
        std::vector<std::pair<std::string, std::string> > cgi;
        /* TYPE_FILE IF EXIST */
        bool _exact_match;
        std::string root;
        std::string redirection;
        std::string index_file;
        int code_redirection;
        bool _directory_listing;
        uint64_t limit_body_size;

    public:
        Location();
        Location(const Location &rhs);
        Location& operator=(const Location &rhs);
        ~Location();

        /* getters */
        uint64_t getLimitBodySize(void) const;
        std::string getUrl(void) const;
        std::string getRoot(void) const;
        std::string getRedirection(void) const;
        std::string getIndexFile(void) const;
        int getRedirCode(void) const;
        bool getAutoIndex(void) const;
        bool isExactMatch(void) const;
        std::vector<std::pair<std::string, std::string> > getCgi(void) const;

        /* setters */
        void addLimitBodySize(const std::string &limit);
        void addUrl(const std::string &url, std::string root);
        void addRoot(const std::string &root);
        void addRedirection(const std::string &code, const std::string &redirect);
        void setAutoIndex(const std::string &check);
        void setExactMatch(void);
        void setIndexFile(const std::string &file);
        void setCgi(const std::string &extension, const std::string &executable);
};

#endif
