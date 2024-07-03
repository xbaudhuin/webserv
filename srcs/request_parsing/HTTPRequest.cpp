#include "HTTPRequest.hpp"
#include "Error.hpp"
#include "ServerConf.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <limits>
#include <ostream>
#include <stdexcept>

const char *HTTPRequest::_validMethods[] = {"GET",     "POST",  "DELETE",
                                            "HEAD",    "PUT",   "CONNECT",
                                            "OPTIONS", "TRACE", "PATCH"};

const size_t HTTPRequest::_methodSize = 9;

const char *HTTPRequest::_whiteSpaces = " \t";

const size_t HTTPRequest::_uriMaxSize = 8000;

const size_t HTTPRequest::_headerMaxSize = 8000;

HTTPRequest::HTTPRequest(void) { return; }

HTTPRequest::HTTPRequest(int fd)
    : _socket(fd), _statusCode(200), _method(""), _uri(""), _version(0), _host(""),
      _body("") {
  return;
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

void trimWhitespace(std::string &str, const char *whiteSpaces) {
  str.erase(0, str.find_first_not_of(whiteSpaces));
  str.erase(str.find_last_not_of(whiteSpaces) + 1);
}

void HTTPRequest::removeReturnCarriage(std::string &line) {
  if (line.size() - 1 == '\r')
    line.erase(line.size() - 1);
  return;
}

int HTTPRequest::parseUri(const std::string &uri) {
  if (uri[0] != '/')
    return (400);
  if (uri.size() > _uriMaxSize)
    return (414);
  size_t pos = uri.find_first_of('?');
  _uri = uri.substr(0, pos);
  if (_uri.size() > 2048)

    if (pos != uri.npos) {
      std::string query = uri.substr(pos + 1);
      if (query.empty() == false)
        _queryUri = split(query, "+&");
    }
  return (200);
}

size_t HTTPRequest::parseRequestLine(const std::string &requestLine) {
  vec_string split_request = split(requestLine, " ");
  if (split_request.size() != 3) {
    std::cout << RED << "StatusCode(split.size()): " << "400" << RESET
              << std::endl;
    return (400);
  }
  _method = split_request[0];
  size_t i = 0;
  for (; i < _methodSize && _method != _validMethods[i]; i++) {
  }
  if (i == _methodSize) {
    return (405);
  } else if (i > 2) {
    return (401);
  }
  _statusCode = parseUri(split_request[1]);
  if (_statusCode >= 300) {
    return (_statusCode);
  }
  if (split_request[2].compare(0, 5, "HTTP/") != 0 ||
      split_request[2].size() < 8) {
    std::cout << RED << "StatusCode(split_compare): " << "400" << RESET
              << std::endl;
    return (400);
  }
  std::string version = split_request[2].substr(5);
  if (std::isdigit(version[0]) == false) {
    std::cout << RED << "StatusCode(isdigit): " << "400" << RESET << std::endl;
    return (400);
  }
  for (i = 1; std::isdigit(version[i]); i++) {
  }
  if (i > 1) {
    return (405);
  }
  if (version[i] != '.') {
    std::cout << RED << "StatusCode(i): " << "400" << RESET << std::endl;
    return (400);
  }
  i++;
  char *after = NULL;
  long int nb = std::strtol(version.c_str() + i, &after, 10);
  if (nb < 0 || nb > 999 || *after != '\0') {
    std::cout << RED << "StatusCode(nb): " << "400" << RESET << std::endl;
    return (400);
  }
  std::cout << "VERSION = " << nb << "\n";
  if (nb >= 1)
    _version = 1;
  else
    _version = 0;
  return (200);
}

void HTTPRequest::getServerconf(const mapConfs mapServerConf,
                                ServerConf &defaultServer) {
  try {
    _server = &(mapServerConf.at(_host));
  } catch (std::exception &e) {
    _server = &defaultServer;
  }
}

unsigned char ToLower(char c) {
  return (std::tolower(static_cast<unsigned char>(c)));
}

bool HTTPRequest::insertInMap(std::string &line) {
  std::transform(line.begin(), line.end(), line.begin(), ToLower);
  size_t pos = line.find_first_of(':');
  std::string key = line.substr(0, pos);
  std::string value = line.substr(pos + 1);
  if (value.size() > _headerMaxSize){
    return (false); 
  }
  trimWhitespace(value, _whiteSpaces);
  std::cout << "KEY = " << key << std::endl;
  std::cout << "VALUE = " << value << std::endl;

  if (pos != line.npos) {
    if (_headers.insert(std::make_pair(key, value)).second == false &&
        key == "host")
      return (false);
  }
  return (true);
}

int HTTPRequest::readRequest(void){
  std::string *buf[BUFSIZ];
  ssize_t read = recv(_socket, buf, BUFSIZ, 0);
  if (read == -1)
  {
    std::cout << RED << "initial recv return -1 on " << _socket << RESET << std::endl;
    return (-1);
  }
  if (read == 0){
    std::cout << RED << "initial recv return 0 on " << _socket << RESET << std::endl;
    return (0);
  }

}

void HTTPRequest::parseRequest(std::istringstream &request, const mapConfs &map,
                               ServerConf &defaultServer) {
  std::string requestLine, line;
  for (; std::getline(request, requestLine);) {
    std::cout << "current line = " << requestLine << "\n";
    if (requestLine.empty() == false || requestLine != "\r")
      break;
  }
  if (request.eof())
    throw std::logic_error("webserv: HTTPRequest::ParseRequest: Empty request");
  removeReturnCarriage(requestLine);

  std::cout << GREEN << "finished requestLine\n" << RESET;
  for (; std::getline(request, line);) {
    removeReturnCarriage(line);
    std::cout << YELLOW << "current line = " << line << "\n" << RESET;
    if (line.empty() == true)
      break;
    if (insertInMap(line) == false) {
      _statusCode = 400;
      return;
    }
  }
  if (_headers.count("host") == 0) {
    _statusCode = 400;
    return;
  }
  _host = _headers.at("host");
  getServerconf(map, defaultServer);
  _statusCode = parseRequestLine(requestLine);
  if (_statusCode >= 400)
    return;
  try {
    // int size = std::strtol(_headers.at("content-lenght").c_str(), NULL, 10);
    // read(size) and add body
  } catch (std::exception &e) {
  }
  if (line.empty() == false)
    throw(std::logic_error(""));
}

void HTTPRequest::print() {
  std::cout << "status code = " << _statusCode << "\n";
  std::cout << "method      = " << _method << "\n";
  std::cout << "uri         = " << _uri << "\n";
  std::cout << "_version    = " << _version << "\n";
  std::cout << "_host       = " << _host << "\n";
  // std::cout << "_headers       = " << _host << "\n";
  std::cout << "_body       = " << _body << "\n";
}
