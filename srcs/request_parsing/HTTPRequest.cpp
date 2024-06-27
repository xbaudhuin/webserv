#include "HTTPRequest.hpp"
#include "ServerConf.hpp"
#include <algorithm>
#include <stdexcept>

const char *validMethods[] = {"GET",     "POST",    "DELETE", "HEAD", "PUT",
                              "CONNECT", "OPTIONS", "TRACE",  "PATCH"};
const size_t methodSize = 9;

HTTPRequest::HTTPRequest(void) { return; }

HTTPRequest::HTTPRequest(std::istringstream &request)
    : _method(""), _uri(""), _version(""), _host(""), _body(""),
      statusCode(200) {
  _headers = {};
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

void HTTPRequest::removeReturnCarriage(std::string &line) {
  if (line[line.size() - 1] == '\r')
    line = line.substr(0, line.size() - 1);
  return;
}

void HTTPRequest::parseRequestLine(const std::string &requestLine) {
  vec_string split = split(requestLine, " ");
  if (split.size() != 3) {
    statusCode = 400;
    return;
  }
  _method = split[0];
  size_t i = 0;
  for (; i < methodSize && _method != validMethods[i]; i++) {
  }
  if (i == 9) {
    statusCode = 405;
    return;
  } else if (i > 2) {
    statusCode = 401;
    return;
  }
  _uri = split[1];
  if (_uri[0] != '/') {
    statusCode = 400;
    return;
  }
  if (split[2].compare(0, 5, "HTTP/") != 0 || split[2].size() < 8) {
    statusCode = 400;
    return;
  }
  _version = split[2].substr(5);
  if (std::isdigit(_version[0]) == false) {
    statusCode = 400;
    return;
  }
  for (i = 1; std::isdigit(_version[i]); i++) {
  }
  if (i > 1) {
    statusCode = 405;
    return;
  }
  if (_version[i] != '.') {
    statusCode = 400;
    return;
  }
  i++;
}

void HTTPRequest::parseRequest(std::istringstream &request) {
  std::string line;
  for (; std::getline(request, line);) {
    if (line.empty() == false && line != "\r")
      break;
  }
  if (request.eof())
    throw std::logic_error("webserv: HTTPRequest::ParseRequest: Empty request");
  removeReturnCarriage(line);
  parseRequestLine(line);
}
