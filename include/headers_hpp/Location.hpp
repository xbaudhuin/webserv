#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>

class Location
{
    private:
        std::string url;
        /* GET */
        /* POST */
        /* DELETE */
        /* FAST_CGI */
        /* TYPE_FILE IF EXIST */
        std::string root;
        std::string redirection;
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
        bool getAutoIndex(void) const;

        /* setters */
        void addLimitBodySize(const std::string &limit);
        void addUrl(const std::string &url);
        void addRoot(const std::string &root);
        void addRedirection(const std::string &code, const std::string &redirect);
        void setAutoIndex(const std::string &check);
};

#endif
