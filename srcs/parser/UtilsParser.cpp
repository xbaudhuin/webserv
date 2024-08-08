#include "Utils.hpp"

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

void goToNextIndex(const vec_string &split, size_t &i) {
  size_t pos = 0;
  while (1) {
    pos = split[i].find("}", 0);
    std::cerr << split[i] << std::endl;
    if (pos != std::string::npos)
      break;
    i++;
  }
}

void addErrorPagesNumber(std::vector<int> &vec, const vec_string &split,
                         size_t &i) {
  size_t size = split.size();
  char *ptr = NULL;
  size_t pos = 0;
  i++;
  while (i < size) {

    if (i + 1 < size && split[i + 1] == ";") {
      if (split[i].find('/', 0) == std::string::npos)
        throw std::logic_error(
            "Webserv: Error:\nError_page directive missing the url");
      break;
    } else if (split[i].find_first_of("{}", 0) != std::string::npos) {
      throw std::logic_error(
          "Webserv: Error:\nError_page directive found invalid "
          "because of misuse of '{}'");
    }
    if ((pos = split[i].find_first_not_of("0123456789", 0)) !=
             std::string::npos) {
      throw std::logic_error("Webserv: Error:\nError_page directive found "
                             "invalid because we found non "
                             "numerical characters inside the error codes");
    }
    long int code = std::strtol(split[i].c_str(), &ptr, 10);
    if (code < 100 || code > 599) {
      throw std::logic_error(
          "Webserv: Error:\nError_page directive found invalid because the "
          "error_codes "
          "do not conform to standard http request codes limits");
    }
    vec.push_back(static_cast<int>(code));
    i++;
  }
}

void trimWhitespace(std::string &str, const char *whiteSpaces) {
  str.erase(0, str.find_first_not_of(whiteSpaces));
  str.erase(str.find_last_not_of(whiteSpaces) + 1);
}

unsigned char toLower(char c) {
  return (std::tolower(static_cast<unsigned char>(c)));
}

void insertStringInVector(std::vector<char> &vec, const std::string &str) {
  vec.resize(vec.size() + str.size());
  for (size_t i = 0; i < str.size(); i++) {
    vec.push_back(str[i]);
  }
}

std::ostream &operator<<(std::ostream &stream, const std::vector<char> &vec) {
  for (std::vector<char>::const_iterator it = vec.begin(); it != vec.end();
       it++) {
    std::cout << *it;
  }
  return (stream);
}

bool isHexadecimal(char c) {
  if (c >= '0' && c <= '9')
    return (true);
  if (c >= 'a' && c <= 'f')
    return (true);
  if (c >= 'A' && c <= 'F')
    return (true);
  return (false);
}

time_t getTime(void) { return (std::time(0)); }

std::string getConfig(void)
{
  return("server {  listen 127.0.0.1:4250;  server_name webserv ;  client_max_body_size 12k ;  root ./html/cgi;error_page 404 405 406 /404.html;    location /python/ {                  cgi .py welcome.py;              upload_location /cookies/;      path_info on;      set_method POST DELETE GET on;  }  location /ruby/ {                  cgi .rb welcome.rb;              upload_location /cookies/;      path_info on;      set_method POST DELETE GET on;  }  location /perl/ {            cgi .pl welcome.pl;        upload_location /cookies/;      path_info on;       set_method POST DELETE GET on;      }  location /php/ {                  cgi .php jokes.php;              upload_location /cookies/;      path_info on;      set_method POST DELETE GET on;  }  location / {      set_method POST DELETE GET on;      autoindex on;  }}server {  server_name Taylor;  listen 4252;  root ./website/taylor;  location / {      index site.html;  }}server {  server_name oof;  listen 4253;  root ./;  location / {      autoindex on;  }}server {  server_name webserv;  listen 4251;  root ./website/dumas;  location / {      index site.html;  }}");
}
