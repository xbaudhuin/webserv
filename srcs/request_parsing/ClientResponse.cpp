#include "Client.hpp"
#include "Utils.hpp"
#include <cerrno>

void Client::findIndex(std::string &url) {
  vec_string vector = _location->getIndexFile();
  std::string tmp;
  struct stat statbuf;
  size_t it = 0;
  for (; it < vector.size(); it++) {
    tmp = url + vector[it];
    if (stat(tmp.c_str(), &statbuf) == 0) {
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
  if (_location->isADir() == true) {
    if (_uri.back() == '/')
      findIndex(url);
    else
      url += _uri.substr(_uri.find_last_of('/'));
  }
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
    std::cout << PURP << "adding line to body: " << s << std::endl;
    _response.addLineToBoddy(s);
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
    _response.setBody(findErrorPage(_statusCode, *_server));
  }
}

void Client::addConnectionHeader(void) {
  if (_version == 0) {
    _response.setHeader("Connection", "close");
    _keepConnectionAlive = false;
    return;
  }
  if (_statusCode == 400 || _statusCode == 408 || _statusCode == 413 ||
      _statusCode == 414 || _statusCode == 500 || _statusCode == 501) {
    _response.setHeader("Connection", "close");
    _keepConnectionAlive = false;
  } else {
    _keepConnectionAlive = true;
    _response.setHeader("Connection", "keep-alive");
  }
  return;
}

void Client::defaultHTMLResponse(void) {
  _response.setStatusCode(_statusCode);
  _response.setDate();
  _response.setHeader("Content-Type", "text/html");
  _response.setBody(findErrorPage(_statusCode, *_server));
  _response.setHeader("Content-Length", _response.getBodySize());
}

bool Client::handleRedirection(std::string &send) {
  _statusCode = _location->getRedirCode();
  defaultHTMLResponse();
  addConnectionHeader();
  _response.setHeader("Location", _location->getRedirection());
  _response.getResponse(send);
  return (_keepConnectionAlive);
}

bool Client::handleError(std::string &send) {
  defaultHTMLResponse();
  addConnectionHeader();
  _response.getResponse(send);
  return (_keepConnectionAlive);
}

bool Client::sendResponse(std::string &response) {
  if (_statusCode < 400 && _location->isRedirected()) {
    return (handleRedirection(response));
  }
  if (_statusCode >= 400)
    return (handleError(response));
  createResponseBody();
  if (_statusCode >= 400) {
    _response.reset();
    return (handleError(response));
  }
  _response.setStatusCode(_statusCode);
  _response.setDate();
  addConnectionHeader();
  _response.setHeader("Content-Type", "text/html");
  _response.setHeader("Content-Length", _response.getBodySize());

  std::cout << BLUE << "response:\n" << response << RESET << std::endl;
  resetClient();
  return (true);
}
