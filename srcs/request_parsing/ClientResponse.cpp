#include "Client.hpp"
#include <cerrno>

void Client::findIndex(std::string &url) {
  vec_string vector = _location->getIndexFile();
  std::string tmp;
  struct stat *statbuf;
  size_t it = 0;
  for (; it < vector.size(); it++) {
    tmp = url + vector[it];
    if (stat(tmp.c_str(), statbuf) == 0) {
      break;
    }
    if (errno == EACCES)
      _statusCode = 403;
    else if (_statusCode != 403 && (errno == ENOENT || errno == ENOTDIR)) {
      _statusCode = 404;
      break;
    }
  }
  if (_statusCode >= 400)
    return;
  url += vector[it];
  return;
}

void Client::findPages(const std::string &urlu) {
  std::string url = "." + urlu;
  // if (url[url.size() - 1] == '/') { // if bool _location->isDirectory == true
  //   findIndex(url);
  //   return;
  // }
  std::ifstream file(url.c_str(), std::ios::in);
  std::cout << RED << "trying  to open file: " << url << RESET << std::endl;
  if (file.is_open() == false) {
    std::cout << RED << "failed to open file: " << url << RESET << std::endl;
    if (access(url.c_str(), F_OK) == -1)
      _statusCode = 404;
    else
      _statusCode = 403;
    return;
  }
  std::string s;
  while (getline(file, s)) {
    _responseBody.append(s);
    _responseBody.append("\r\n");
  }
  if (file.bad()) {
    std::cout << RED << "fail to read all file" << RESET << std::endl;
    _statusCode = 400;
  }
}

void Client::createResponseBody(void) {
  if (_statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "uri : " << _location->getUrl() << " ?" << RESET
              << std::endl;
    findPages(_location->getUrl());
  }
  if (_statusCode >= 400) {
    _responseBody = findErrorPage(_statusCode, _server->getErrPages());
  }
}

void Client::handleRedirection(void) { _statusCode }

void Client::sendResponse(std::string &response) {
  if (_statusCode < 400 && _location->isRedirected()) {
    handleRedirection();
    return;
  }
  createResponseBody();
  response += "HTTP/1.1 ";
  {
    std::ostringstream ss;
    ss << _statusCode;
    response += ss.str();
  }
  response += " reasonPhrase\r\n";
  response += "Webserv: 1.0.0\r\n";
  response += getDate();
  response += "Connection: ";
  if (_statusCode >= 400 || _version == 0) {
    response += "close\r\n";
  } else {
    response += "keep-alive\r\n";
  }

  if (_statusCode < 300 || _statusCode >= 400) {
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: ";
    {
      std::ostringstream ss;
      ss << _responseBody.size();
      response += ss.str();
    }
    response += " \r\n";
  }
  response += "\r\n";
  response += _responseBody;
  std::cout << BLUE << "response:\n" << response << RESET << std::endl;
  resetClient();
  return;
}
