#include "Client.hpp"
#include "Error.hpp"
#include "Utils.hpp"
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <ostream>
#include <stdexcept>
#include <stdio.h>
#include <sys/stat.h>

bool Client::findIndex(std::string &url) {
  vec_string vector = _location->getIndexFile();
  std::string tmp;
  struct stat statbuf;
  size_t it = 0;
  for (; it < vector.size(); it++) {
    tmp = "." + vector[it];
    std::cout << PURP << "vectorIndex[" << it << "] = " << tmp << RESET;
    if (stat(tmp.c_str(), &statbuf) == 0) {
      std::cout << " found" << std::endl;
      break;
    }
    std::cout << " not found" << std::endl;
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

void Client::addContentTypeHeader(void) {
  size_t dot = _sUri.find_last_of('.');
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

void Client::findPages(const std::string &urlu) {
  (void)urlu;
  std::string url = "." + _location->getRootServer() + _sUri;
  std::cout << RED << "_sUri = " << _sUri << RESET << std::endl;
  std::cout << RED << "url = " << url << RESET << std::endl;
  if (_location->isADir() == true) {
    std::cout << RED << "Location is a Dir" << RESET << std::endl;
    if (url[url.size() - 1] == '/') {
      struct stat st;
      if (stat(url.c_str(), &st) == -1) {
        std::cout << RED << "Client::FindPages: stat(" << url << ") = -1"
                  << RESET << std::endl;
        _statusCode = 404;
        return;
      }
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
  struct stat st;
  std::cout << GREEN
            << "Client::findPages: AFTER searching location: _statusCode = "
            << _statusCode << "; url = " << url << std::endl;
  if (stat(url.c_str(), &st) == -1) {
    std::cout << RED << "Clent::findPages: stat(" << url << ") = -1" << RESET
              << std::endl;
    if (errno == ENOENT)
      _statusCode = 404;
    else if (errno == EACCES)
      _statusCode = 403;
    else
      _statusCode = 500;
    return;
  }
  if ((st.st_mode & S_IFMT) != S_IFREG) {
    std::cout << RED << "Not a regular file" << RESET << std::endl;
    _statusCode = 403;
    return;
  }
  _tmpFd = open(url.c_str(), O_RDONLY | O_CLOEXEC);
  // _file.open(url.c_str(), std::ios::in);
  std::cout << RED << "trying to open file: " << url << RESET << std::endl;
  if (_tmpFd == -1) {
    std::cout << RED << "failed to open file: " << url << RESET << std::endl;
    if (access(url.c_str(), F_OK) == -1)
      _statusCode = 404;
    else
      _statusCode = 403;
    return;
  }
  std::cout << YELLOW << "size of file: " << st.st_size << RESET << std::endl;
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
  std::cout << "Bytes to read=" << toRead << std::endl;
  std::vector<char> buf(toRead);
  ssize_t readBytes = read(_tmpFd, &buf[0], toRead);

  std::cout << PURP << "read of size " << readBytes << ": " << buf.size()
            << "; tried " << _leftToRead << std::endl;
  if (readBytes < 0) {
    std::cout << RED << "invalid read on _tmpFd" << RESET << std::endl;
    _statusCode = 500;
    return;
  }
  // if (readBytes == 0) {
  //   close(_filefd);
  //   _filefd = -1;
  //   return;
  // }
  // std::stringstream buffer;
  // buffer << file.rdbuf();
  // std::cout << PURP << "READ: buf : " << buf << RESET << std::endl;
  _response.setBody(buf);

  if (_leftToRead <= static_cast<size_t>(readBytes)) {
    std::cout << GREEN << "Finished to read file" << RESET << std::endl;
    _leftToRead = 0;
    // close(_filefd);
    // _filefd = -1;
  } else {
    _leftToRead = _leftToRead - _sizeMaxResponse;
    std::cout << BLUE << "still have to read: " << _leftToRead << RESET
              << std::endl;
  }
}

void Client::readFile(std::vector<char> &vec) {

  // std::memset(buf, 0, _sizeMaxResponse + 1);
  // std::vector<char> buf(_sizeMaxResponse);
  // char buf[_sizeMaxResponse] = {0};
  int toRead = std::min(_sizeMaxResponse, _leftToRead);
  std::vector<char> buf(toRead);
  ssize_t readBytes = read(_tmpFd, &buf[0], toRead);

  std::cout << PURP << "read of size " << readBytes << ": " << buf.size()
            << std::endl;
  if (readBytes < 0) {
    std::cout << PURP << "read of size " << readBytes << ": " << buf.size()
              << "; _leftToRead = " << _leftToRead << std::endl;
    _statusCode = 500;
    return;
  }
  // if (readBytes == 0) {
  //   close(_filefd);
  //   _filefd = -1;
  //   return;
  // }
  // std::stringstream buffer;
  // buffer << file.rdbuf();
  // std::cout << PURP << "READ: buf : " << buf << RESET << std::endl;
  vec.swap(buf);

  if (_leftToRead <= static_cast<size_t>(readBytes)) {
    std::cout << GREEN << "Finished to read file" << RESET << std::endl;
    _leftToRead = 0;
    // close(_filefd);
    // _filefd = -1;
  } else {
    _leftToRead = _leftToRead - _sizeMaxResponse;
    std::cout << BLUE << "still have to read: " << _leftToRead << RESET
              << std::endl;
  }
}

void Client::createResponseBody(void) {
  if (_statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "uri : " << _location->getUrl() << " ?" << RESET
              << std::endl;
    findPages(_location->getUrl());
  }
  // if (_statusCode >= 400) {
  //   const std::vector<char> ref = findErrorPage(_statusCode, *_server);
  //   _response.setBody(ref, ref.size());
  // }
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
  const std::vector<char> &ref = findErrorPage(_statusCode, *_server);
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
              << "; statusCode = " << _statusCode << RESET << std::endl;
  } else {
    std::cout << RED << "location = NULL" << "; statusCode = " << _statusCode
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
  addContentTypeHeader();
  std::cout << PURP << "End of Client::buildResponse: body.size() = "
            << _response.getBody().size() << RESET << std::endl;
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
  std::cerr << "Client::handleCgi: _statusCode = " << _statusCode << std::endl;
  if (_statusCode >= 400) {
    buildResponse();
    response = _response.getResponse();
    return;
  }
  char buf[1000];
  getcwd(buf, 1000);
  std::cout << "trying to open: " << _outfileCgi << " at path= " << buf
            << std::endl;
  struct stat st;
  if (stat(_outfileCgi.c_str(), &st) == -1) {
    _statusCode = 500;
    buildResponse();
    response = _response.getResponse();
    return;
  }
  std::cout << RED << "Client::HandleCGI: stat() != -1" << RESET << std::endl;
  std::stringstream ss;
  ss << st.st_size;
  std::cout << "stat->size = " << ss.str() << std::endl;
  _leftToRead = st.st_size;
  _tmpFd = open(_outfileCgi.c_str(), O_RDONLY);
  if (_tmpFd == -1) {
    std::cout << RED << "fail to open " << _outfileCgi << RESET << std::endl;
    _statusCode = 500;
    buildResponse();
    response = _response.getResponse();
    return;
  }
  readFile(response);
  _cgiPid = 0;
  return;
}

void Client::handleDelete(void) {
  std::string url = "." + _location->getRootServer() + _sUri;
  std::cout << YELLOW << "trying to unlink: " << url << RESET << std::endl;
  if (unlink(url.c_str()) == -1) {
    if (errno == EACCES) {
      _statusCode = 403;
    } else if (errno == ENOENT)
      _statusCode = 404;
    else
      _statusCode = 500;
    return (handleError());
  }
  _statusCode = 204;
  defaultHTMLResponse();
  _keepConnectionAlive = false;
  addConnectionHeader();
  _response.BuildResponse();
  return;
}

void Client::handleMultipart(void) {
  std::cout << YELLOW << "Client:handleMultipart: _currentMultipart = "
            << _currentMultipart
            << "; tmpfilename = " << _multipart[_currentMultipart].tmpFilename
            << "; file = " << _multipart[_currentMultipart].file << RESET
            << std::endl;
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
  std::cout << "next multipart" << std::endl;
  handleMultipart();
}

void Client::uploadTmpFileDifferentFileSystem(std::string &tmp,
                                              std::string &outfile) {
  std::vector<char> body;
  if (_bodyToRead == 0) {
    struct stat st;
    if (stat(tmp.c_str(), &st) == -1) {
      _statusCode = 500;
      return;
    }
    if (stat(outfile.c_str(), &st) != -1) {
      _statusCode = 409;
      return;
    }
    _tmpFd = open(tmp.c_str(), O_CLOEXEC, O_RDONLY);
    _uploadFd = open(outfile.c_str(), O_CLOEXEC, O_RDWR | O_CREAT | O_APPEND);
    if (_tmpFd == -1 || _uploadFd == -1) {
      _statusCode = 500;
      return;
    }
    _bodyToRead = st.st_size;
  }
  readFile(body);
  if (_statusCode == 500) {
    return;
  }
  ssize_t writeByte = write(_uploadFd, &body[0], body.size());
  if (writeByte == -1 || static_cast<size_t>(writeByte) < body.size())
    _statusCode = 500;
  if (_bodyToRead == 0) {
    close(_tmpFd);
    _tmpFd = -1;
    std::cerr << "unlink: " << tmp << std::endl;
    unlink(tmp.c_str());
    close(_uploadFd);
    _uploadFd = -1;
  }
  return;
}

void Client::handleChunk(void) {
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
  }
  if (_diffFileSystem == true) {
    uploadTmpFileDifferentFileSystem(_tmpFile, _sPathUpload);
  }
}

void Client::handleUpload(void) {

  struct stat st;
  if (stat(_sPathUpload.c_str(), &st) != -1) {
    _statusCode = 409;
    return;
  }
  _uploadFd = open(_sPathUpload.c_str(), O_CLOEXEC | O_CREAT | O_RDWR);
  if (_uploadFd == -1) {
    _statusCode = 501;
    return;
  }
  ssize_t byteWrite = write(_uploadFd, &_vBody[0], _vBody.size());
  if (byteWrite == -1 || static_cast<size_t>(byteWrite) != _vBody.size()) {
    _statusCode = 500;
    unlink(_sPathUpload.c_str());
    return;
  }
  _statusCode = 201;
  return;
}

void Client::handlePOST() {
  if (_multipart.size() >= 1) {
    handleMultipart();
  } else if (_chunkRequest == true) {
    handleChunk();
  } else {
    handleUpload();
  }
  handleError();
}

bool Client::getResponse(std::vector<char> &response) {
  if (_cgiPid > 0) {
    std::cout << PURP2 << "Client::sendResponse: HandleCGI" << RESET
              << std::endl;
    handleCgi(response);
    if (_leftToRead == 0)
      resetClient();
    return (_leftToRead != 0);
  } else if (_sMethod == "DELETE" && _statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "Client::sendResponse: HandleDELETE" << RESET
              << std::endl;
    handleDelete();
    response = _response.getResponse();
  } else if (_sMethod == "POST" && _statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "Client::sendResponse handlePOST" << RESET
              << std::endl;
    handlePOST();
    response = _response.getResponse();
  }
  // std::cout << PURP2 << "_vbody = " << _vBody << RESET << std::endl;
  else if (_response.isReady() == false) {
    std::cout << RED << "response.isReady() = false" << RESET << std::endl;
    buildResponse();
    response = _response.getResponse();
    std::cout << BLUE << "response for _sUri:\n"
              << _sUri << "; of size : " << response.size() << RESET
              << std::endl;
    if (_leftToRead == 0)
      resetClient();
    return (_leftToRead != 0);
  }
  bool ret = _leftToRead != 0;
  std::cout << RED << "return of sendResponse: " << ret << RESET << std::endl;
  if (_leftToRead > 0) {
    readFile(response);
    if (_statusCode == 500) {
      std::cerr << "Client::sendResponse: error in readFile()" << std::endl;
      return (false);
    }
  }
  // std::cout << RED << "response.isNotDone() = " << ret << RESET << std::endl;
  if (_leftToRead == 0)
    resetClient();
  std::cout << RED << "return of sendResponse: " << ret << RESET << std::endl;
  return (_leftToRead != 0);
}

bool Client::sendResponse(std::vector<char> &response) {
  errno = 0;
  resetVector(response);
  if (_location == NULL) {
    std::cout << RED << "FAIL NO LOCATION OSKOUR" << RESET << std::endl;
  }
  bool ret = getResponse(response);
  _time = getTime();
  return (ret);
}
