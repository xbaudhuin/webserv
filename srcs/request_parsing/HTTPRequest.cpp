#include "HTTPRequest.hpp"
#include "Error.hpp"
#include "ServerConf.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <limits>
#include <ostream>
#include <stdexcept>

const char *HTTPRequest::validMethods[] = {"GET",     "POST",    "DELETE", "HEAD", "PUT",
                              "CONNECT", "OPTIONS", "TRACE",  "PATCH"};

const size_t HTTPRequest::methodSize = 9;

const char *HTTPRequest::whiteSpaces = " \t";

HTTPRequest::HTTPRequest(void) { return; }

HTTPRequest::HTTPRequest(std::istringstream &request)
    : statusCode(200), _method(""), _uri(""), _version(0), _host(""), _body("")
       {
  parseRequest(request);
}

HTTPRequest::~HTTPRequest(void) { return; }

HTTPRequest::HTTPRequest(HTTPRequest const &copy) {
  if (this != &copy)
    *this = copy;
  return;
}

HTTPRequest &HTTPRequest::operator=(HTTPRequest const &rhs) {
  if (this != &rhs) {
    _method = rhs._method;
    _uri = rhs._uri;
    _version = rhs._version;
    _host = rhs._host;
    _headers = rhs._headers;
    _body = rhs._body;
  }
  return (*this);
}

void trimWhitespace(std::string &str, const char *whiteSpaces){
  str.erase(0, str.find_first_not_of(whiteSpaces));
  str.erase(str.find_last_not_of(whiteSpaces) + 1);
}


void HTTPRequest::removeReturnCarriage(std::string &line) {
  if (line[0] == '\r')
    line.erase(0, 1);
  return;
}

size_t HTTPRequest::parseRequestLine(const std::string &requestLine) {
  vec_string split_request = split(requestLine, " ");
  if (split_request.size() != 3) {
    std::cout << RED << "StatusCode(split.size()): " << "400" <<  RESET  << std::endl;
    return (400);
  }
  _method = split_request[0];
  size_t i = 0;
  for (; i < methodSize && _method != validMethods[i]; i++) {
  }
  if (i == methodSize) {
    return (405);
  } else if (i > 2) {
    return (401);
  }
  _uri = split_request[1];
  if (_uri[0] != '/') {
    std::cout << RED << "StatusCode(uri): " << "400" <<  RESET << std::endl;
    return (400);
  }
  if (split_request[2].compare(0, 5, "HTTP/") != 0 ||
      split_request[2].size() < 8) {
    std::cout << RED << "StatusCode(split_compare): " << "400" <<  RESET << std::endl;
    return (400);
  }
  std::string version = split_request[2].substr(5);
  if (std::isdigit(version[0]) == false) {
    std::cout << RED << "StatusCode(isdigit): " << "400" <<  RESET << std::endl;
    return (400);
  }
  for (i = 1; std::isdigit(version[i]); i++) {
  }
  if (i > 1) {
    return (405);
  }
  if (version[i] != '.') {
    std::cout << RED << "StatusCode(i): " << "400" <<  RESET << std::endl;
    return (400);
  }
  i++;
  char *after = NULL;
  long int nb = std::strtol(version.c_str() + i, &after, 10);
  if (nb < 0 || nb > 999 || *after != '\0') {
    std::cout << RED << "StatusCode(nb): " << "400" <<  RESET << std::endl;
    return (400);
  }
  std::cout << "VERSION = " << nb << "\n";
  if (nb >= 1)
    _version = 1;
  else
    _version = 0;
  return (200);
}

unsigned char ToLower(char c) {
  return (std::tolower(static_cast<unsigned char>(c)));
}

bool HTTPRequest::insertInMap(std::string &line) {
  std::transform(line.begin(), line.end(), line.begin(),
                ToLower);
  size_t pos = line.find_first_of(':');
  std::string key = line.substr(0, pos);
  std::string value = line.substr(pos + 1);
  trimWhitespace(value, whiteSpaces);
  std::cout << "KEY = " << key << std::endl;
  std::cout << "VALUE = " << value << std::endl; 

  if (pos != line.npos) {
    if (_headers.insert(std::make_pair(key, value)).second ==
            false &&
        key == "host")
      return (false);
  }
  return (true);
}

void HTTPRequest::parseRequest(std::istringstream &request) {
  std::string line;
  for (; std::getline(request, line);) {
    std::cout << "current line = " << line << "\n";
    if (line.empty() == false || line != "\r")
      break;
  }
  if (request.eof())
    throw std::logic_error("webserv: HTTPRequest::ParseRequest: Empty request");
  removeReturnCarriage(line);
  statusCode = parseRequestLine(line);
  if (statusCode >= 400)
    return;
  std::cout << GREEN << "finished requestLine\n" << RESET;
  for (; std::getline(request, line);) {
    removeReturnCarriage(line);
    std::cout << YELLOW << "current line = " << line << "\n" << RESET;
    if (line.empty() == true)
      break;
    if (insertInMap(line) == false) {
      statusCode = 400;
      return;
    }
  }
  _host = _headers.at("host");
  try {
    int size = std::strtol(_headers.at("content-lenght").c_str(), NULL, 10);
    //read(size) and add body
  }
  catch (std::exception &e){
}
  if (line.empty() == false)
    throw (std::logic_error(""));
}

void HTTPRequest::print(){
  std::cout << "status code = " << statusCode << "\n";
  std::cout << "method      = " << _method << "\n";
  std::cout << "uri         = " << _uri << "\n";
  std::cout << "_version    = " << _version << "\n";
  std::cout << "_host       = " << _host << "\n";
  // std::cout << "_headers       = " << _host << "\n";
  std::cout << "_body       = " << _body << "\n";
}
