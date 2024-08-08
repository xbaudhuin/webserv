#include "Client.hpp"
#include <netinet/in.h>

Client::Client(int fd, mapConfs &mapConfs, ServerConf *defaultConf,
               in_addr_t clientIp)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf),
      _clientIp(clientIp), _server(NULL), _location(NULL), _statusCode(0),
      _version(1), _requestSize(0), _bodyToRead(-1), _chunkRequest(false),
      _requestIsDone(true), _checkMulti(false), _multipartRequest(false),
      _tmpFd(-1), _sizeChunk(0), _currentMultipart(0), _response(),
      _keepConnectionAlive(true), _diffFileSystem(false), _epollIn(false),
      _uploadFd(-1), _leftToRead(0), _cgiPid(0) {

  _time = getTime();
  if (defaultConf == NULL)
    throw(std::logic_error("Default server is NULL"));
  return;
}

// Client::Client(int fd, mapConfs &mapConfs, ServerConf *defaultConf,
//                in_addr_t IpClient)
//     : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf),
//     _server(NULL),
//       _location(NULL), _statusCode(0), _version(1), _requestSize(0),
//       _bodyToRead(-1), _chunkRequest(false), _requestIsDone(true),
//       _checkMulti(false), _multipartRequest(false), _tmpFd(-1),
//       _sizeChunk(0), _currentMultipart(0), _response(),
//       _keepConnectionAlive(true), _diffFileSystem(false), _epollIn(false),
//       _uploadFd(-1), _leftToRead(0), _cgiPid(0) {
//
//   (void)IpClient;
//   _time = getTime();
//   if (defaultConf == NULL)
//     throw(std::logic_error("Default server is NULL"));
//   return;
// }

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

void Client::copyMultipart(const std::vector<multipartRequest> &rhs) {
  for (size_t i = 0; i < _multipart.size(); i++) {
    if (_multipart[i].tmpFilename.empty() == false)
      if (i >= rhs.size() || _multipart[i].tmpFilename != rhs[i].tmpFilename) {
        unlink(_multipart[i].tmpFilename.c_str());
      }
    if (_multipart[i].file.empty() == false) {
      if (i >= rhs.size() || _multipart[i].file != rhs[i].file) {
        unlink(_multipart[i].file.c_str());
      }
    }
  }
  _multipart = rhs;
}

Client &Client::operator=(Client const &rhs) {
  if (this != &rhs) {
    _socket = rhs._socket;
    _defaultConf = rhs._defaultConf;
    _clientIp = rhs._clientIp;
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
    copyMultipart(rhs._multipart);
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

time_t Client::getTime(void) const { return (std::time(0)); }

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
    unlink(_outfileCgi.c_str());
  }
  _outfileCgi = "";
  if (_infileCgi.size() > 0) {
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
    return ((*it).second);
  }
  return (_defaultConf);
}

// const std::vector<char> &Client::getBuffer(void) const { return
// (_vBuffer); }

// int Client::getBodyToRead(void) const { return (_bodyToRead); }

bool Client::keepConnectionOpen(void) const { return (_keepConnectionAlive); }

std::string Client::getDateOfFile(time_t rawtime) const {

  tm *gmtTime = gmtime(&rawtime);
  char buffer[80] = {0};
  std::strftime(buffer, 80, "%d-%m-%y %H:%M:%S GMT; ", gmtTime);
  return (buffer);
}

void Client::addCgiToMap(std::map<int, pid_t> &mapCgi) {
  try {
    if (_cgiPid > 0) {
      mapCgi.insert(std::make_pair(_socket, _cgiPid));
    }
  } catch (std::exception &e) {
    if (_cgiPid > 0) {
      _statusCode = 500;
      if (kill(_cgiPid, SIGKILL) == -1) {
        logErrorClient("Client::addCgiToMap: fail to kill child when "
                       "map.insert failed");
        return;
      } else {
        logErrorClient("Client::addCgiToMap: fail to map.insert pid child");
      }
    }
    if (waitpid(_cgiPid, NULL, 0) == -1)
      logErrorClient("Client::addCgiToMap: fail to waitpid after kill");
  }
}

std::string Client::prepareLogMessage() const {
  std::string message = "ip: ";
  struct in_addr addr;
  addr.s_addr = _clientIp;
  char strIp[INET_ADDRSTRLEN] = {0};
  if (inet_ntop(AF_INET, &addr, strIp, sizeof(strIp)) == NULL) {
    std::cerr << "fail to get ip of client on fd: " << _socket << std::endl;
    message += "Unknown";
  } else {
    message += strIp;
  }
  std::stringstream ss;
  ss << "; fd: ";
  ss << _socket;
  ss << "; Port: ";
  ss << _defaultConf->getPort();
  ss << "; ";
  message += ss.str();
  message += getDateOfFile(getTime());
  return (message);
}

void Client::logErrorClient(const std::string &str) const {
  std::string message = prepareLogMessage();
  message += str;
  if (_sMethod.empty() == false && _sUri.empty() == false) {
    message += "; ";
    message += _sMethod + " " + _sUri;
  }
  if (_statusCode != 0) {
    message += "; ";
    message += _response.getStatusCodeAndReasonPhrase(_statusCode);
  }
  errorServer(message);
}

void Client::logErrorChild(const std::string &str) const {
  std::string message = prepareLogMessage();
  message += str;
  errorChild(message);
}

void Client::setStatusCode(size_t exitStatus) {
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
  if (_statusCode >= 400) {
    logErrorChild("Client::setStatusCode: exit status of cgi != 0");
  }
}
