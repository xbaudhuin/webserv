#include "Client.hpp"
#include "Error.hpp"
#include "Utils.hpp"
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <ostream>
#include <stdio.h>

bool Client::findIndex(std::string &url) {
  vec_string vector = _location->getIndexFile();
  std::string tmp;
  struct stat statbuf;
  size_t it = 0;
  for (; it < vector.size(); it++) {
    tmp = "." + vector[it];
    std::cout << PURP << "vectorIndex[" << it << "] = " << tmp << RESET
              << std::endl;
    if (stat(tmp.c_str(), &statbuf) == 0) {
      break;
    }
    if (errno == EACCES)
      _statusCode = 403;
    else if (_statusCode != 403 && errno == ENOENT) {
      continue;
    }
  }
  if (_statusCode >= 400)
    return (true);
  if (it == vector.size())
    return (false);
  url = tmp;
  return (true);
}

void Client::buildListingDirectory(std::string &url) {
  if (_location->getAutoIndex() == 0) {
    _statusCode = 404;
    return;
  }
  _sUri = "." + _location->getRootServer() + _sUri;
  _response.setStatusCode(200);
  _response.setDate();
  _response.setHeader("Content-Type", "text/html");
  addConnectionHeader();
  std::string body = "<!DOCTYPE html>\r\n";
  body += "<html>\r\n";
  body += "<head><title>Index of ";
  body += _sUri + "</title></head>\r\n";
  body += "<body>\r\n<h1>Index of ";
  body += _sUri + "</h1>";
  std::string loc = "." + _location->getUrl();
  if (_sUri != loc)
    body += "<hr><pre><a href=\"../\">../</a>\r\n";
  DIR *dir;
  dir = opendir(_sUri.c_str());
  if (dir == NULL) {
    std::cout << RED << "buildListingDirectory: dir = NULL; _sUri = " << _sUri
              << RESET << std::endl;
    if (errno == EACCES)
      _statusCode = 403;
    else if (errno == ENONET)
      _statusCode = 404;
    else
      _statusCode = 500;
    return;
  }
  struct dirent *ent;
  errno = 0;
  while (true) {
    ent = readdir(dir);
    while (ent && ent->d_name[0] == '.') {
      if (ent->d_name[1] == '.' && ent->d_name[2] == '\0' && url != _sUri)
        break;
      ent = readdir(dir);
    }
    if (ent == NULL)
      break;
    std::cout << YELLOW << "ent: " << ent->d_name << RESET << std::endl;
    struct stat file;
    std::string tmp = _sUri + ent->d_name;
    if (stat(tmp.c_str(), &file) == -1) {
      int err = errno;
      perror("ListtingDirectory");
      errno = err;
      std::cout << RED << "buildListingDirectory: stat = -1; ent->d_name ="
                << ent->d_name << RESET << std::endl;
      if (errno == EACCES) {
        _statusCode = 403;
      } else
        _statusCode = 500;
      return;
    }

    std::string time = body += "<a href=\"";
    body += ent->d_name;
    if (ent->d_type == DT_DIR)
      body += "/";
    body += "\">";
    body += ent->d_name;
    if (ent->d_type == DT_DIR)
      body += "/";
    body += "</a> Date: " + getDateOfFile(file.st_mtim.tv_sec);
    body += " ";
    if (ent->d_type == DT_REG) {
      std::ostringstream ss;
      ss << file.st_size;
      body += ss.str();
    } else
      body += "-";
    body += "\r\n";
  }
  body += "</pre><hr></body>\r\n";
  body += "</html>\r\n";
  _response.setBody(body, body.size());
  _response.BuildResponse();
}

void Client::findPages(const std::string &urlu) {
  (void)urlu;
  std::string url = "." + _location->getRootServer() + _sUri;
  if (_location->isADir() == true) {
    std::cout << RED << "Location is a Dir" << RESET << std::endl;
    if (_sUri[_sUri.size() - 1] == '/') {
      if (findIndex(url) == false) {
        return (buildListingDirectory(url));
      }
    }
  } else if (_location->isExactMatch() == false) {
    if (findIndex(url) == false) {
      std::cout << *_location << BLUE << "no exact match, index == false;"
                << RESET << std::endl;
      _statusCode = 404;
      return;
    }
  }
  _sPath = url;
  _file.open(url.c_str(), std::ios::in);
  std::cout << RED << "trying  to open file: " << url << RESET << std::endl;
  if (_file.is_open() == false) {
    std::cout << RED << "failed to open file: " << url << RESET << std::endl;
    if (access(url.c_str(), F_OK) == -1)
      _statusCode = 404;
    else
      _statusCode = 403;
    return;
  }
  struct stat st;
  stat(url.c_str(), &st);
  // std::memset(buf, 0, _sizeMaxResponse + 1);
  // std::vector<char> buf(_sizeMaxResponse);
  // char buf[_sizeMaxResponse] = {0};

  int toRead = std::min(static_cast<long int>(_sizeMaxResponse), st.st_size);
  std::vector<char> buf(toRead + 1);
  _file.read(&buf[0], toRead);

  std::cout << PURP << "read of size " << _file.gcount() << ": " << buf.size()
            << std::endl;
  if (_file.gcount() <= 0) {
    _statusCode = 500;
    return;
  }
  // std::stringstream buffer;
  // buffer << file.rdbuf();
  std::cout << YELLOW << "size of file: " << st.st_size << RESET << std::endl;
  _response.setBody(buf, st.st_size);

  if (st.st_size < static_cast<long int>(_sizeMaxResponse))
    _leftToRead = 0;
  else
    _leftToRead = st.st_size - _sizeMaxResponse;
  _nbRead++;
}

void Client::createResponseBody(void) {
  if (_statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "uri : " << _location->getUrl() << " ?" << RESET
              << std::endl;
    findPages(_location->getUrl());
  }
  if (_statusCode >= 400) {
    const std::string &ref = findErrorPage(_statusCode, *_server);
    _response.setBody(ref, ref.size());
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
  const std::string &ref = findErrorPage(_statusCode, *_server);
  _response.setBody(ref, ref.size());
}

void Client::handleRedirection(void) {
  _statusCode = _location->getRedirCode();
  defaultHTMLResponse();
  addConnectionHeader();
  _response.setHeader("Location", _location->getRedirection());
  _response.BuildResponse();
  return;
}

void Client::handleError(void) {
  defaultHTMLResponse();
  addConnectionHeader();
  _response.BuildResponse();
  return;
}

void Client::buildResponse(void) {
  if (_location != NULL) {
    std::cout << RED << "location = " << _location->getUrl()
              << ";statusCode = " << _statusCode << RESET << std::endl;
  } else {
    std::cout << RED << "location = NULL" << ";statusCode = " << _statusCode
              << RESET << std::endl;
  }
  if (_server == NULL) {
    _server = _defaultConf;
  }
  if (_statusCode < 400 && _location != NULL && _location->isRedirected()) {
    return (handleRedirection());
  }
  if (_statusCode >= 400)
    return (handleError());
  createResponseBody();
  std::cout << PURP
            << "After createResponseBody Client::buildResponse: body.size() = "
            << _response.getBody().size() << RESET << std::endl;
  if (_statusCode >= 400) {
    _response.reset();
    return (handleError());
  }
  _response.setStatusCode(_statusCode);
  _response.setDate();
  addConnectionHeader();
  size_t dot = _sUri.find_last_of('.');
  if (dot < 100) {
    std::cout << BLUE << "dot found _sUri.substr = " << _sUri.substr(dot) << RESET
              << std::endl;
  } else
    std::cout << BLUE << "dot not found :" << dot << RESET << std::endl;
  std::cout << "uri.size() = " << _sUri.size() << "; dot = " << dot << RESET
            << std::endl;
  if (_sUri.size() > 5 && _sUri.size() - dot < 5 && dot < _sUri.size()) {
    std::cout << PURP << "inside dot file " << RESET << std::endl;
    std::string extension = _sUri.substr(dot);
    if (extension == ".ico") {
      _response.setHeader("Content-Type", "image/vnd.microsoft.icon");
      // _response.setHeader("Content-encoding", "gzip");
    } else if (extension == ".png") {
      std::cout << PURP << "found .png" << RESET << std::endl;
      _response.setHeader("Content-Type", "image/png");
    } else if (extension == ".gif") {
      _response.setHeader("Content-Type", "image/gif");
    }
  } else {
    _response.setHeader("Content-Type", "text/html");
  }
  std::cout << PURP << "End of Client::buildResponse: body.size() = "
            << _response.getBody().size() << RESET << std::endl;
  _response.BuildResponse();
}

void Client::add400Response(void) {
  if (_epollIn == true)
    return;
  Response error400;
  error400.setStatusCode(400);
  error400.setDate();
  error400.setHeader("Content-Type", "text/html");
  const std::string &ref = findErrorPage(_statusCode, *_server);
  _response.setBody(ref, ref.size());
  error400.setHeader("Content-Length", _response.getBodySize());
  error400.setHeader("Connection", "close");
  error400.BuildResponse();
  _response.add400(error400);
  _epollIn = true;
  _keepConnectionAlive = false;
  return;
}

void Client::readFile(std::vector<char> &response) {
  char buf[_sizeMaxResponse];
  std::memset(buf, 0, _sizeMaxResponse);
  // std::vector<char> buf(_sizeMaxResponse);
  // char buf[_sizeMaxResponse] = {0};
  _file.read(buf, _sizeMaxResponse - 1);
  if (_file.gcount() < 0) {
    _statusCode = 500;
    return;
  }
  if (_file.gcount() == 0) {
    _leftToRead = 0;
  }
  // std::stringstream buffer;
  // buffer << file.rdbuf();

  if (_leftToRead < _sizeMaxResponse)
    _leftToRead = 0;
  else
    _leftToRead -= _sizeMaxResponse;
  _nbRead++;
  response.reserve(_file.gcount() + 1);
  response.insert(response.begin(), &buf[0], &buf[_file.gcount() - 1]);
  std::cout << BLUE << "readFile: " << buf << RESET << std::endl;
  return;
}

bool Client::sendResponse(std::vector<char> &response) {
  response.clear();
  if (_response.isReady() == false) {
    std::cout << RED << "response.isReady() = false" << RESET << std::endl;
    buildResponse();
    response = _response.getResponse();
    std::cout << BLUE << "response for _sUri:\n"
              << _sUri << "; of size : " << response.size() << RESET
              << std::endl;
    bool ret = _leftToRead != 0;
    std::cout << RED << "return of sendResponse: " << ret << RESET << std::endl;
    if (_leftToRead == 0)
      resetClient();
    return (_leftToRead != 0);
  }
  std::cout << PURP << "leftToRead = " << _leftToRead << RESET << std::endl;
  if (_leftToRead > 0) {
    readFile(response);
  }
  // std::cout << RED << "response.isNotDone() = " << ret << RESET << std::endl;
  if (_leftToRead == 0)
    resetClient();
  bool ret = _leftToRead != 0;
  std::cout << RED << "return of sendResponse: " << ret << RESET << std::endl;
  return (_leftToRead != 0);
}
