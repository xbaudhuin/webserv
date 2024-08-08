#include "Client.hpp"

bool Client::findIndex(std::string &url) {
  vec_string vector = _location->getIndexFile();
  std::string tmp;
  struct stat statbuf;
  size_t it = 0;
  for (; it < vector.size(); it++) {
    tmp = "." + vector[it];
    if (stat(tmp.c_str(), &statbuf) == 0) {
      break;
    }
    if (errno == EACCES) {
      _statusCode = 403;
    } else if (_statusCode != 403 && errno == ENOENT) {
      continue;
    }
  }
  if (_statusCode >= 400) {
    logErrorClient("Client::findIndex: fail to access index: " + url);
  }
  return (true);
  if (it == vector.size())
    return (false);
  url = tmp;
  return (true);
}

void Client::addContentTypeHeader(void) {
  size_t dot = _sUri.find_last_of('.');
  if (_sUri.size() > 5 && _sUri.size() - dot < 5 && dot < _sUri.size()) {
    std::string extension = _sUri.substr(dot);
    if (extension == ".ico") {
      _response.setHeader("Content-Type", "image/vnd.microsoft.icon");
    } else if (extension == ".png") {
      _response.setHeader("Content-Type", "image/png");
    } else if (extension == ".gif") {
      _response.setHeader("Content-Type", "image/gif");
    }
  } else {
    _response.setHeader("Content-Type", "text/html");
  }
}

void Client::buildListingDirectory(std::string &url) {
  if (_location->getAutoIndex() == 0) {
    _statusCode = 404;
    logErrorClient(
        "Client::buildListingDirectory: no index or ListingDirectory");
    return;
  }
  if (_location->hasAlias())
    _sUri = "." + _location->getRootServer() +
            _sUri.substr(_location->myUri().size() - 1);
  else
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
    if (errno == EACCES)
      _statusCode = 403;
    else if (errno == ENONET)
      _statusCode = 404;
    else
      _statusCode = 500;
    logErrorClient("Client::buildListingDirectory: fail to opendir");
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
    struct stat file;
    std::string tmp = _sUri + ent->d_name;
    if (stat(tmp.c_str(), &file) == -1) {
      if (errno == EACCES) {
        _statusCode = 403;
      } else
        _statusCode = 500;
      logErrorClient("Client::buildListingDirectory: fail to get stat info");
      closedir(dir);
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
    body += "<br>\r\n";
  }
  closedir(dir);
  body += "</pre><hr></body>\r\n";
  body += "</html>\r\n";
  _response.setBody(body, body.size());
  _response.BuildResponse();
}

void Client::getUrlFromLocation(std::string &url) const {
  if (_location->hasAlias()) {
    std::string urli = _location->getRootServer();
    if (urli[urli.size() - 1] != '/')
      urli += '/';
    url = "." + urli +
          (_sUri.size() >= _location->myUri().size()
               ? _sUri.substr(_location->myUri().size())
               : "");
    // std::cout << "s_Uri size: " << _sUri.size()
    //           << " && myUri size:" << _location->myUri().size() << std::endl;
    // std::cout << "HERE _sUri substr = "
    //           << (_sUri.size() >= _location->myUri().size()
    //                   ? _sUri.substr(_location->myUri().size())
    //                   : "")
    //           << std::endl;
  } else if (_location->hasAlias()) {
    std::string urli = _location->getRootServer();
    if (urli[urli.size() - 1] != '/')
      urli += '/';
    url = "." + urli +
          (_sUri.size() > _location->myUri().size()
               ? _sUri.substr(_location->myUri().size())
               : "");
    // std::cout << "MY URI AND MY URI SIZE: " << _location->myUri() << " && "
    //           << _location->myUri().size() << std::endl;
    // std::cout << "s_Uri size: " << _sUri.size()
    //           << " && myUri size:" << _location->myUri().size() << std::endl;
    // std::cout << "HERE _sUri substr = "
    //           << (_sUri.size() >= _location->myUri().size()
    //                   ? _sUri.substr(_location->myUri().size())
    //                   : "")
    //           << std::endl;
  } else
    url = "." + _location->getRootServer() + _sUri;
  std::cout << std::endl << *_location << std::endl;
  std::cout << RED << "_sUri = " << _sUri << RESET << std::endl;
  std::cout << RED << "url = " << url << RESET << std::endl;
}

void Client::findPages(void) {
  std::string url;
  getUrlFromLocation(url);
  if (_location->isADir() == true) {
    if (url[url.size() - 1] == '/') {
      struct stat st;
      if (stat(url.c_str(), &st) == -1) {
        _statusCode = 404;
        logErrorClient("Client::findPages: no such file or directory: " + url);
        return;
      }
      if (findIndex(url) == false) {
        return (buildListingDirectory(url));
      }
    }
  } else if (_location->isExactMatch() == false) {
    if (findIndex(url) == false) {
      _statusCode = 404;
      logErrorClient("Client::findPages: no such file or directory: " + url);
      return;
    }
  }
  _sPath = url;
  struct stat st;
  std::cout << GREEN
            << "Client::findPages: AFTER searching location: _statusCode = "
            << _statusCode << "; url = " << url << std::endl;
  errno = 0;
  if (stat(url.c_str(), &st) == -1) {
    if (errno == ENOENT || errno == ENOTDIR)
      _statusCode = 404;
    else if (errno == EACCES)
      _statusCode = 403;
    else
      _statusCode = 500;
    logErrorClient("Client::findPages: fail to stat: " + url);
    return;
  }
  if ((st.st_mode & S_IFMT) != S_IFREG) {
    _statusCode = 403;
    logErrorClient("Client::findPages: not a file: " + url);
    return;
  }
  _tmpFd = open(url.c_str(), O_RDONLY | O_CLOEXEC);
  if (_tmpFd == -1) {
    if (access(url.c_str(), F_OK) == -1)
      _statusCode = 404;
    else
      _statusCode = 403;
    logErrorClient("Client::findPages: failed to open file: " + url);
    return;
  }
  _leftToRead = st.st_size;
  _response.setHeader("Content-Length", st.st_size);
  addConnectionHeader();
  readFile();
}

void Client::readFile(void) {

  // std::memset(buf, 0, _sizeMaxResponse + 1);
  // std::vector<char> buf(_sizeMaxResponse);
  // char buf[_sizeMaxResponse] = {0};
  int toRead = std::min(_sizeMaxResponse, _leftToRead);
  std::vector<char> buf(toRead);
  ssize_t readBytes = read(_tmpFd, &buf[0], toRead);

  if (readBytes < 0) {
    _statusCode = 500;
    logErrorClient("Client::readFile: fail to read");
    return;
  }
  _response.setBody(buf);

  if (_leftToRead <= static_cast<size_t>(readBytes)) {
    _leftToRead = 0;

  } else {
    _leftToRead = _leftToRead - _sizeMaxResponse;
  }
}

void Client::readFile(std::vector<char> &vec) {
  int toRead = std::min(_sizeMaxResponse, _leftToRead);
  std::vector<char> buf(toRead);
  ssize_t readBytes = read(_tmpFd, &buf[0], toRead);

  if (readBytes < 0) {
    _statusCode = 500;
    logErrorClient("Client::readFile: fail to read");
    return;
  }
  vec.swap(buf);
  if (_leftToRead <= static_cast<size_t>(readBytes)) {
    _leftToRead = 0;

  } else {
    _leftToRead = _leftToRead - _sizeMaxResponse;
  }
}

void Client::createResponseBody(void) {
  if (_statusCode > 0 && _statusCode < 400) {
    findPages();
  }
}

void Client::addConnectionHeader(void) {
  if (_version == 0) {
    _response.setHeader("Connection", "close");
    _keepConnectionAlive = false;
    return;
  }
  if (_statusCode == 400 || _statusCode == 408 || _statusCode == 413 ||
      _statusCode == 414 || _statusCode == 429 || _statusCode == 503) {
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
  const std::vector<char> ref = findErrorPage(_statusCode, *_server);
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
  if (_server == NULL) {
    _server = _defaultConf;
  }
  if (_location == NULL && _statusCode < 400)
    _statusCode = 404;
  if (_statusCode < 400 && _location->isRedirected()) {
    return (handleRedirection());
  }
  if (_statusCode >= 400)
    return (handleError());
  createResponseBody();
  if (_statusCode >= 400) {
    _response.reset();
    return (handleError());
  }
  _response.setStatusCode(_statusCode);
  _response.setDate();
  addConnectionHeader();
  addContentTypeHeader();
  _response.BuildResponse();
}

void Client::addErrorResponse(size_t errorCode) {
  if (_epollIn == true)
    return;
  if (errorCode < 100 || errorCode >= 600)
    throw std::logic_error("Invalid Error Code");
  Response error400;
  error400.setStatusCode(errorCode);
  error400.setDate();
  error400.setHeader("Content-Type", "text/html");
  const std::vector<char> &ref = findErrorPage(errorCode, *_defaultConf);
  _response.setBody(ref, ref.size());
  error400.setHeader("Content-Length", _response.getBodySize());
  error400.setHeader("Connection", "close");
  error400.BuildResponse();
  _response.add400(error400);
  _epollIn = true;
  _keepConnectionAlive = false;
  return;
}

void Client::handleCgi(std::vector<char> &response) {
  if (_statusCode >= 400) {
    buildResponse();
    response = _response.getResponse();
    return;
  }
  struct stat st;
  if (stat(_outfileCgi.c_str(), &st) == -1) {
    _statusCode = 500;
    logErrorClient("Client::handleCgi: fail to stat: " + _outfileCgi);
    handleError();
    response = _response.getResponse();
    return;
  }
  _leftToRead = st.st_size;
  _tmpFd = open(_outfileCgi.c_str(), O_RDONLY);
  if (_tmpFd == -1) {
    _statusCode = 500;
    logErrorClient("Client::handleCgi: fail to open: " + _outfileCgi);
    handleError();
    response = _response.getResponse();
    return;
  }
  readFile(response);
  _cgiPid = 0;
  return;
}

void Client::handleDelete(void) {
  std::string url = "." + _location->getRootServer() + _sUri;
  errno = 0;
  if (unlink(url.c_str()) == -1) {
    if (errno == EACCES) {
      _statusCode = 403;
    } else if (errno == ENOENT)
      _statusCode = 404;
    else
      _statusCode = 500;
    logErrorClient("Client::handleDelete: fail to unlink: " + url);
  } else
    _statusCode = 204;
  handleError();
  return;
}

void Client::handleMultipart(void) {
  struct stat st;
  if (stat(_multipart[_currentMultipart].file.c_str(), &st) != -1) {
    _statusCode = 409;
    logErrorClient("Client::handleMultipart: fail to stat: " +
                   _multipart[_currentMultipart].file);
    return;
  }
  errno = 0;
  if (_diffFileSystem == false &&
      rename(_multipart[_currentMultipart].tmpFilename.c_str(),
             _multipart[_currentMultipart].file.c_str()) != 0) {
    switch (errno) {
    case EXDEV:
      errno = 0;
      _diffFileSystem = true;
      break;
    case EACCES:
      _statusCode = 403;
      break;
    case EEXIST:
      _statusCode = 409;
      break;
    default:
      _statusCode = 500;
    }
    logErrorClient("Client::handleMultipart: fail to rename: " +
                   _multipart[_currentMultipart].tmpFilename + " to " +
                   _multipart[_currentMultipart].file);
  }
  if (_diffFileSystem == true) {
    uploadTmpFileDifferentFileSystem(_multipart[_currentMultipart].tmpFilename,
                                     _multipart[_currentMultipart].file);
  }
  if (_diffFileSystem == false || _bodyToRead == 0)
    _currentMultipart++;
  if (_statusCode < 400 && _currentMultipart == _multipart.size()) {
    _statusCode = 201;
  }
  if (_statusCode >= 201)
    return;
  handleMultipart();
}

void Client::uploadTmpFileDifferentFileSystem(std::string &tmp,
                                              std::string &outfile) {
  std::vector<char> body;
  if (_bodyToRead == 0) {
    struct stat st;
    if (stat(tmp.c_str(), &st) == -1) {
      _statusCode = 500;
      logErrorClient(
          "Client::uploadTmpFileDifferentFileSystem: fail to stat :" + tmp);
      return;
    }
    if (stat(outfile.c_str(), &st) != -1) {
      _statusCode = 409;
      logErrorClient(
          "Client::uploadTmpFileDifferentFileSystem: file already exist: " +
          outfile);
      return;
    }
    _tmpFd = open(tmp.c_str(), O_CLOEXEC, O_RDONLY);
    _uploadFd =
        open(outfile.c_str(), O_CLOEXEC | O_RDWR | O_CREAT | O_APPEND, 00644);
    if (_tmpFd == -1 || _uploadFd == -1) {
      _statusCode = 500;
      logErrorClient(
          "Client::uploadTmpFileDifferentFileSystem: fail to open both file");
      return;
    }
    _bodyToRead = st.st_size;
    _leftToRead = _bodyToRead;
  }
  readFile(body);
  if (_statusCode == 500) {
    return;
  }
  ssize_t writeByte = write(_uploadFd, &body[0], body.size());
  if (writeByte == -1 || static_cast<size_t>(writeByte) < body.size()) {
    _statusCode = 500;
    logErrorClient(
        "Client::uploadTmpFileDifferentFileSystem: fail to write all bytes");
  }
  _bodyToRead -= writeByte;
  if (_bodyToRead == 0) {
    close(_tmpFd);
    _tmpFd = -1;
    unlink(tmp.c_str());
    close(_uploadFd);
    _uploadFd = -1;
  }
  return;
}

void Client::handleUpload(void) {
  struct stat st;
  errno = 0;
  stat(_tmpFile.c_str(), &st);
  if (_diffFileSystem == false &&
      rename(_tmpFile.c_str(), _sPathUpload.c_str()) != 0) {
    switch (errno) {
    case EXDEV:
      errno = 0;
      _diffFileSystem = true;
      break;
    case EACCES:
      _statusCode = 403;
      break;
    case EEXIST:
      _statusCode = 409;
      break;
    default:
      _statusCode = 500;
    }
    logErrorClient("Client::handleUpload: fail to rename " + _tmpFile + " to " +
                   _sPathUpload);
  }
  if (_diffFileSystem == true) {
    uploadTmpFileDifferentFileSystem(_tmpFile, _sPathUpload);
  }
}

void Client::handlePOST() {
  if (_multipart.size() >= 1) {
    handleMultipart();
  } else
    handleUpload();
  if (_bodyToRead == 0)
    handleError();
}

bool Client::getResponse(std::vector<char> &response) {
  if (_cgiPid > 0) {
    std::cout << PURP2 << "Client::sendResponse: HandleCGI" << RESET
              << std::endl;
    handleCgi(response);
    return (_leftToRead != 0);
  } else if (_sMethod == "DELETE" && _statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "Client::sendResponse: HandleDELETE" << RESET
              << std::endl;
    handleDelete();
    response = _response.getResponse();
    return (false);
  } else if (_sMethod == "POST" && _statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "Client::sendResponse handlePOST" << RESET
              << std::endl;
    handlePOST();
    if (_bodyToRead == 0) {
      response = _response.getResponse();
    }
    return (_bodyToRead != 0);
  } else if (_response.isReady() == false) {
    buildResponse();
    response = _response.getResponse();
    return (_leftToRead != 0);
  }
  bool ret = _leftToRead != 0;
  if (_leftToRead > 0) {
    readFile(response);
    if (_statusCode == 500) {
      addErrorResponse(500);
      return (false);
    }
  }
  std::cout << RED << "return of sendResponse: " << ret << RESET << std::endl;
  return (_leftToRead != 0);
}

bool Client::sendResponse(std::vector<char> &response) {
  errno = 0;
  resetVector(response);
  bool ret = getResponse(response);
  if (ret == false) {
    if (_statusCode < 400)
      logErrorClient(_sUri);
    resetClient();
  }
  _time = getTime();
  return (ret);
}
