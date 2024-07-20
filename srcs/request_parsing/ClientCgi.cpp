#include "Client.hpp"

void Client::setupCgi() {

  std::string cgiPathExec = _location.getExecutePath(_sUri);
  std::string cgiPathScript = _location.getCgiFile(_sUri);

  std::ostringstream ss;
  ss << _socket;
  _infileCgi = "webservin" + ss.str() + _sUri.substr(0, 230);
  _outfileCgi = "webservout " + ss.str() + _sUri.substr(0, 230);

  pid_t pid = fork();
  if (pid < 0) {
    _statusCode = 500;
    return;
  }
  if (pid == 0) {
  }
  if (pid > 0) {
    _cgiPid = pid;
    _infileCgi = cgiPathScript + _infileCgi;
    _outfileCgi = cgiPathScript + _outfileCgi;
    return;
  }
}
