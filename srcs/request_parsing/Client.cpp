#include "Client.hpp"

Client::Client(int fd, mapConfs &mapConfs, ServerConf *defaultConf)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf), _server(NULL),
      _location(NULL), _statusCode(0), _version(1), _requestSize(0),
      _bodyToRead(-1), _chunkRequest(false), _requestIsDone(true), _checkMulti(false),
      _multipartRequest(false), _tmpFd(-1), _sizeChunk(0), _currentMultipart(0),
      _response(), _keepConnectionAlive(true), _diffFileSystem(false),
      _epollIn(false), _uploadFd(-1), _leftToRead(0), _cgiPid(0) {

  _time = getTime();
  if (defaultConf == NULL)
    throw(std::logic_error("Default server is NULL"));
  return;
}

Client::Client(int fd, mapConfs &mapConfs, ServerConf *defaultConf, in_addr_t IpClient)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf), _server(NULL),
      _location(NULL), _statusCode(0), _version(1), _requestSize(0),
      _bodyToRead(-1), _chunkRequest(false), _requestIsDone(true), _checkMulti(false),
      _multipartRequest(false), _tmpFd(-1), _sizeChunk(0), _currentMultipart(0),
      _response(), _keepConnectionAlive(true), _diffFileSystem(false),
      _epollIn(false), _uploadFd(-1), _leftToRead(0), _cgiPid(0) {

	(void)IpClient;
  _time = getTime();
  if (defaultConf == NULL)
    throw(std::logic_error("Default server is NULL"));
  return;
}

Client::~Client(void) {
  for (size_t i = 0; i < _multipart.size(); i++) {
    if (_multipart[i].tmpFilename.empty() == false) {
      unlink(_multipart[i].tmpFilename.c_str());
    }
    if (_multipart[i].file.empty() == false) {
      unlink(_multipart[i].file.c_str());
    }
  }
  if (_tmpFd != -1)
    close(_tmpFd);
  if (_uploadFd != -1)
    close(_uploadFd);
  if (_infileCgi.empty() == false) {
    unlink(_infileCgi.c_str());
  }
  if (_outfileCgi.empty() == false) {
    unlink(_outfileCgi.c_str());
  }
  if (_cgiPid > 0) {
    if (kill(_cgiPid, SIGKILL) == -1) {
      std::cerr << "Client::~Client: Kill failed on " << _cgiPid << std::endl;
    }
    waitpid(_cgiPid, NULL, WNOHANG);
  }
  return;
}

Client::Client(Client const &copy) : _mapConf(copy._mapConf) {
  _uploadFd = -1;
  _tmpFd = -1;
  if (this != &copy)
    *this = copy;
  return;
}

Client &Client::operator=(Client const &rhs) {
  if (this != &rhs) {
    _socket = rhs._socket;
    _defaultConf = rhs._defaultConf;
    _server = rhs._server;
    _location = rhs._location;
    _time = rhs._time;
    _statusCode = rhs._statusCode;
    _sMethod = rhs._sMethod;
    _sUri = rhs._sUri;
    _sPath = rhs._sPath;
    _sQueryUri = rhs._sQueryUri;
    _version = rhs._version;
    _headers = rhs._headers;
    _requestSize = rhs._requestSize;
    _vBody = rhs._vBody;
    _vBuffer = rhs._vBuffer;
    _bodyToRead = rhs._bodyToRead;
    _chunkRequest = rhs._chunkRequest;
    _requestIsDone = rhs._requestIsDone;
    _boundary = rhs._boundary;
    _checkMulti = rhs._checkMulti;
    _multipartRequest = rhs._multipartRequest;
    if (_tmpFile.empty() == false) {
      unlink(_tmpFile.c_str());
    }
    _tmpFile = rhs._tmpFile;
    if (_tmpFd != -1) {
      close(_tmpFd);
    }
    if (rhs._tmpFd != -1) {
      _tmpFd = open(_tmpFile.c_str(), O_CLOEXEC | O_RDWR | O_APPEND);
      if (_tmpFd == -1)
        throw std::runtime_error("Client::operator=: fail to open _tmpFd on " +
                                 _tmpFile);
    } else
      _tmpFd = -1;
    _sizeChunk = rhs._sizeChunk;
    _multipart = rhs._multipart;
    _currentMultipart = rhs._currentMultipart;
    _response = rhs._response;
    _keepConnectionAlive = rhs._keepConnectionAlive;
    _diffFileSystem = rhs._diffFileSystem;
    _epollIn = rhs._epollIn;
    if (_uploadFd != -1)
      close(_uploadFd);
    if (rhs._uploadFd != -1)
      _uploadFd = dup(rhs._uploadFd);
    else
      _uploadFd = -1;
    _leftToRead = rhs._leftToRead;
    _infileCgi = rhs._infileCgi;
    _outfileCgi = rhs._outfileCgi;
    _cgiPid = rhs._cgiPid;
    _sPathInfo = rhs._sPathInfo;
  }
  return (*this);
}

time_t Client::getTime(void) { return (std::time(0)); }

bool Client::isTimedOut(void) const {
  time_t current;
  time(&current);
  double timeOut = std::difftime(current, _time);
  if (timeOut >= _timeOutClient)
    return (true);
  return (false);
}

void Client::resetClient(void) {
  std::cout << YELLOW << "reset client + response" << RESET << std::endl;
  _server = NULL;
  _location = NULL;
  _statusCode = 0;
  _sMethod = "";
  _sUri = "";
  _sPath = "";
  _sPathUpload = "";
  _sQueryUri = "";
  _version = 1;
  _headers.clear();
  _requestSize = 0;
  resetVector(_vBuffer);
  resetVector(_vBody);
  _bodyToRead = -1;
  _chunkRequest = false;
  _requestIsDone = true;
  _boundary = "";
  _checkMulti = false;
  _multipartRequest = false;
  if (_tmpFile.empty() == false) {
    std::cerr << "unlink: " << _tmpFile << std::endl;
    unlink(_tmpFile.c_str());
  }
  _tmpFile = "";
  if (_uploadFd != -1)
    close(_uploadFd);
  _uploadFd = -1;
  if (_tmpFd != -1) {
    close(_tmpFd);
  }
  _tmpFd = -1;
  _sizeChunk = 0;
  resetVector(_multipart);
  _currentMultipart = 0;
  _response.reset();
  _diffFileSystem = false;
  _epollIn = false;
  _leftToRead = 0;
  _cgiPid = 0;
  if (_outfileCgi.size() > 0) {
    std::cerr << "unlink: " << _outfileCgi << std::endl;
    unlink(_outfileCgi.c_str());
  }
  _outfileCgi = "";
  if (_infileCgi.size() > 0) {
    std::cerr << "unlink: " << _infileCgi << std::endl;
    unlink(_infileCgi.c_str());
  }
  _infileCgi = "";
  _sPathInfo = "";
  if (_uploadFd != -1)
    close(_uploadFd);
  _uploadFd = -1;
}

ServerConf *Client::getServerConf(const std::string &host) {
  mapConfs::const_iterator it;
  it = _mapConf.find(host);
  if (it != _mapConf.end()) {
    std::cout << YELLOW << "found via host: " << host
              << "; server name: " << ((*it).second)->getMainServerName()
              << RESET << std::endl;
    return ((*it).second);
  }
  std::cout << YELLOW << "return default server" << RESET << std::endl;
  return (_defaultConf);
}

// const std::vector<char> &Client::getBuffer(void) const { return (_vBuffer); }

// int Client::getBodyToRead(void) const { return (_bodyToRead); }

bool Client::keepConnectionOpen(void) const { return (_keepConnectionAlive); }

std::string Client::getDateOfFile(time_t rawtime) const {

  tm *gmtTime = gmtime(&rawtime);
  char buffer[80] = {0};
  std::strftime(buffer, 80, "%d-%m-%y %H:%M:%S GMT;", gmtTime);
  return (buffer);
}

void Client::addCgiToMap(std::map<int, pid_t> &mapCgi) {
  try {
    if (_cgiPid > 0) {
      mapCgi.insert(std::make_pair(_socket, _cgiPid));
    }
  } catch (std::exception &e) {
    if (_cgiPid > 0) {
      if (kill(_cgiPid, SIGKILL) == -1)
        std::cerr << "Client::addCgiToMap: Failed to kill pid: " << _cgiPid
                  << std::endl;
      _statusCode = 500;
    }
    waitpid(_cgiPid, NULL, WNOHANG);
  }
}

void Client::setStatusCode(size_t exitStatus) {
  std::cerr << "Client::setStatusCode : begin:  exitStatus = " << exitStatus
            << "; _statusCode = " << _statusCode << std::endl;
  switch (exitStatus) {
  case 0: {
    _statusCode = 200;
    break;
  }
  case 126: {
    _statusCode = 422;
    break;
  }
  case 130: {
    _statusCode = 504;
    break;
  }
  default: {
    _statusCode = 500;
  }
  }
  std::cerr << "Client::setStatusCode : end:  exitStatus = " << exitStatus
            << "; _statusCode = " << _statusCode << std::endl;
}

// bool Request::addBuffer(const std::vector<char> buffer) {
//   if (_bodyToRead > 0 || _requestIsDone == false) {
//     _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
//     return (parseBody());
//   }
//   fillBufferWithoutReturnCarriage(buffer);
//   int newLine = hasNewLine();
//   if (newLine == 0) {
//     return (false);
//   }
//   if (_vBuffer.size() < 20 || newLine > 0) {
//     if (earlyParsing(newLine) == false) {
//       _statusCode = 400;
//       _server = _defaultConf;
//       return (true);
//     }
//   }
//   int64_t pos = hasEmptyLine(newLine);
//   if (pos == -1) {
//     return (false);
//   }
//   std::string request(&_vBuffer[0], &_vBuffer[pos]);
//   _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + pos + 2);
//   parseRequest(request);
//   std::cout << RED << "End after parseRequest; StatusCode = " << _statusCode
//             << RESET << std::endl;
//
//   if (_vBuffer.empty() == false) {
//     if (_bodyToRead > 0) {
//       _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
//       return (parseBody());
//     } else
//       _statusCode = 400;
//   }
//   if (_statusCode != 0) {
//     return (true);
//   }
//   std::cerr << "Client::addBuffer: end: _statusCode = " << _statusCode
//             << std::endl;
//   return (false);
// }

// void Client::print() {
//
//   if (_server == NULL)
//     std::cout << RED << "no server\n" << RESET;
//   else
//     std::cout << GREEN "server    = " << _server->getServerNames()[0] <<
//     RESET
//               << "\n";
//   if (_location == NULL)
//     std::cout << RED << "no location\n" << RESET;
//   else
//     std::cout << GREEN "location    = " << _location->getUrl() << RESET <<
//     "\n";
//   std::cout << "time        = " << _time << "\n";
//   std::cout << "time out ?  = " << isTimedOut() << "\n";
//
//   std::cout << "_socket     = " << _socket << "\n";
//   std::cout << "status code = " << _statusCode << "\n";
//   std::cout << "method      = " << _sMethod << "\n";
//   std::cout << "uri         = " << _sUri << "\n";
//   std::cout << "queryUri    = " << _sQueryUri << "\n";
//   std::cout << "_requestSize= " << _requestSize << "\n";
//   std::cout << "_version    = " << _version << "\n";
//   std::cout << "_sHost       = " << _sHost << "\n";
//   std::cout << "_headers: key = value" << "\n";
//   for (std::map<std::string, std::string>::iterator i = _headers.begin();
//        i != _headers.end(); i++) {
//     std::cout << (*i).first << " = " << (*i).second << "\n";
//   }
//   std::cout << "_bodyToRead    = " << _bodyToRead << "\n";
//   std::cout << "remain buffer = " << _vBuffer << std::endl;
// }
