#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Typedef.hpp"
#include "Utils.hpp"
#include <iostream>
#include <vector>

class Location {
private:
  std::string url;
  bool _post;
  bool _get;
  bool _delete;
  /* POST */
  /* DELETE */
  /* FAST_CGI */
  std::vector<std::string> available_extension;
  std::vector<std::pair<std::string, std::string> > cgi;
  /* TYPE_FILE IF EXIST */
  bool _exact_match;
  std::string root;
  std::string redirection;
  vec_string index_file;
  int code_redirection;
  bool _directory_listing;
  uint64_t limit_body_size;
  int _root_check;

public:
  Location();
  Location(const Location &rhs);
  Location &operator=(const Location &rhs);
  ~Location();

  /* getters */
  const uint64_t &getLimitBodySize(void) const;
  const std::string &getUrl(void) const;
  const std::string &getRoot(void) const;
  const std::string &getRedirection(void) const;
  const vec_string &getIndexFile(void) const;
  const int &getRedirCode(void) const;
  const bool &getAutoIndex(void) const;
  const bool &isExactMatch(void) const;
  const bool &getPostStatus(void) const;
  const bool &getGetSatus(void) const;
  const bool &getDeleteStatus(void) const;
  const std::vector<std::pair<std::string, std::string> > &getCgi(void) const;

  /* setters */
  void addLimitBodySize(const std::string &limit);
  void addUrl(const std::string &url, std::string root);
  void addRoot(const std::string &root);
  void addRedirection(const std::string &code, const std::string &redirect);
  void setAutoIndex(const std::string &check);
  void setExactMatch(void);
  void setIndexFile(const std::string &file);
  void setCgi(const std::string &extension, const std::string &executable);
  void setMethod(const std::string &method, const std::string &status);
  void fixUrl(const std::string &url);
  void fixRoot();
  void fixIndexFile();
};

#endif
