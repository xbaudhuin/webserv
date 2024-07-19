#include "Client.hpp"
#include "Error.hpp"
#include "Port.hpp"
#include "ServerConf.hpp"
#include <stdexcept>

Client::Client(int fd, mapConfs &mapConfs, ServerConf *defaultConf)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf), _server(NULL),
      _response(), _location(NULL), _statusCode(0), _sMethod(""), _sUri(""),
      _version(0), _sHost(""), _requestSize(0), _bodyToRead(-1),
      _keepConnectionAlive(true), _chunkRequest(false), _epollIn(false),
      _leftToRead(0), _nbRead(0) {
  _time = getTime();
  if (defaultConf == NULL)
    throw(std::logic_error("Default server is NULL"));
  return;
}

Client::~Client(void) { return; }

Client::Client(Client const &copy)
    : _socket(copy._socket), _mapConf(copy._mapConf) {
  if (this != &copy)
    *this = copy;
  return;
}

Client &Client::operator=(Client const &rhs) {
  if (this != &rhs) {
    _defaultConf = rhs._defaultConf;
    _server = rhs._server;
    _location = rhs._location;
    _time = rhs._time;
    _statusCode = rhs._statusCode;
    _sMethod = rhs._sMethod;
    _sUri = rhs._sUri;
    _sQueryUri = rhs._sQueryUri;
    _version = rhs._version;
    _sHost = rhs._sHost;
    _headers = rhs._headers;
    _requestSize = rhs._requestSize;
    _bodyToRead = rhs._bodyToRead;
    _vBuffer = rhs._vBuffer;
    _keepConnectionAlive = rhs._keepConnectionAlive;
    _chunkRequest = rhs._chunkRequest;
    _response = rhs._response;
    _epollIn = rhs._epollIn;
    _leftToRead = rhs._leftToRead;
    _nbRead = rhs._nbRead;
    if (_file.is_open())
      _file.close();
    if (rhs._file.is_open())
      _file.open(rhs._sPath.c_str());
    _sPath = rhs._sPath;
  }
  return (*this);
}

time_t Client::getTime(void) { return (std::time(0)); }

bool Client::isTimedOut(void)const {
  time_t current;
  time(&current);
  double timeOut = std::difftime(current, _time);
  if (timeOut >= 10.0)
    return (true);
  return (false);
}

void Client::resetClient(void) {
  std::cout << YELLOW << "reset client + response" << RESET << std::endl;
  _server = NULL;
  _location = NULL;
  _time = getTime();
  _statusCode = 0;
  _sMethod = "";
  _sUri = "";
  _sQueryUri = "";
  _version = 0;
  _sHost = "";
  _headers.clear();
  _requestSize = 0;
  _bodyToRead = -1;
  _vBuffer.clear();
  std::vector<char> tmp;
  _vBuffer.swap(tmp);
  _chunkRequest = false;
  _response.reset();
  _epollIn = false;
  _leftToRead = 0;
  _nbRead = 0;
  _sPath = "";
  if (_file.is_open()) {
    std::cout << GREEN << "closing _file" << RESET << std::endl;
    _file.close();
  }
}

ServerConf *Client::getServerConf(void) {
  mapConfs::const_iterator it;
  it = _mapConf.find(_sHost);
  if (it != _mapConf.end()) {
    std::cout << YELLOW << "found via host: " << _sHost
              << "; server name: " << ((*it).second)->getMainServerName()
              << RESET << std::endl;
    return ((*it).second);
  }
  std::cout << YELLOW << "return default server" << RESET << std::endl;
  return (_defaultConf);
}

const std::vector<char> &Client::getBuffer(void) const { return (_vBuffer); }

int Client::getBodyToRead(void) const { return (_bodyToRead); }

bool Client::keepConnectionOpen(void) const { return (_keepConnectionAlive); }

std::string Client::getDateOfFile(time_t rawtime) const {

  tm *gmtTime = gmtime(&rawtime);
  char buffer[80] = {0};
  std::strftime(buffer, 80, "%d-%m-%y %H:%M:%S GMT;", gmtTime);
  return (buffer);
}

void Client::print() {

  if (_server == NULL)
    std::cout << RED << "no server\n" << RESET;
  else
    std::cout << GREEN "server    = " << _server->getServerNames()[0] << RESET
              << "\n";
  if (_location == NULL)
    std::cout << RED << "no location\n" << RESET;
  else
    std::cout << GREEN "location    = " << _location->getUrl() << RESET << "\n";
  std::cout << "time        = " << _time << "\n";
  std::cout << "time out ?  = " << isTimedOut() << "\n";

  std::cout << "_socket     = " << _socket << "\n";
  std::cout << "status code = " << _statusCode << "\n";
  std::cout << "method      = " << _sMethod << "\n";
  std::cout << "uri         = " << _sUri << "\n";
  std::cout << "queryUri    = " << _sQueryUri << "\n";
  std::cout << "_requestSize= " << _requestSize << "\n";
  std::cout << "_version    = " << _version << "\n";
  std::cout << "_sHost       = " << _sHost << "\n";
  std::cout << "_headers: key = value" << "\n";
  for (std::map<std::string, std::string>::iterator i = _headers.begin();
       i != _headers.end(); i++) {
    std::cout << (*i).first << " = " << (*i).second << "\n";
  }
  std::cout << "_bodyToRead    = " << _bodyToRead << "\n";
  std::cout << "remain buffer = " << _vBuffer << std::endl;
}
