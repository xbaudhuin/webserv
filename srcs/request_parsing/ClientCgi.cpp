#include "Client.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

extern char **environ;

bool Client::isTimedOutCgi(void) const {
  time_t current;
  time(&current);
  double timeOut = std::difftime(current, _time);
  if (timeOut >= _timeOutCgi)
    return (true);
  return (false);
}

void Client::cgiPOSTMethod(void) {
  int fd = open(_infileCgi.c_str(), O_RDWR | O_CREAT | O_TRUNC, 00644);
  if (fd == -1) {
    throw std::runtime_error("500");
  }
  ssize_t writeBytes = write(fd, &_vBody[0], _vBody.size());
  close(fd);
  fd = -1;
  if (writeBytes < _vBody.size()) {
    throw std::runtime_error("500");
  }
  fd = open(_infileCgi.c_str(), O_RDWR);
  if (fd == -1) {
    // unlink?
    throw std::runtime_error("500");
  }
  if (dup2(fd, STDIN_FILENO) == -1) {
    close(fd);
    throw std::runtime_error("500");
  }
  close(fd);
}

void Client::cgiOutfile(void) {
  int fd = open(_outfileCgi.c_str(), O_RDWR | O_CREAT | O_TRUNC, 00644);
  if (fd == -1) {
    throw std::runtime_error("500");
  }
  if (dup2(fd, STDOUT_FILENO) == -1) {
    close(fd);
    throw std::runtime_error("500");
  }
  close(fd);
}

void Client::addHeaderToEnv(std::vector<const char *> &vEnv,
                            const std::string &envVariable,
                            const std::string &headerKey) {
  std::map<std::string, std::string>::iterator it;
  it = _headers.find(headerKey);
  if (it != _headers.end()) {
    std::string tmp = envVariable + (*it).second;
    vEnv.push_back(tmp.c_str());
  }
}

void Client::addVariableToEnv(std::vector<const char *> &vEnv,
                              const std::string &envVariable) {
  vEnv.push_back(envVariable.c_str());
}

void Client::buildEnv(std::vector<const char *> &vEnv) {
  for (size_t i = 0; environ[i]; i++) {
    vEnv.push_back(environ[i]);
  }
  addVariableToEnv(vEnv, "REQUEST_METHOD" + _sMethod);
  std::string envType = "CONTENT_TYPE=";
  if (_sMethod == "GET") {
    addVariableToEnv(vEnv, "QUERY_STRING=" + _sQueryUri);
    envType += "application/x-www-form-urlencoded";
  } else {
    std::map<std::string, std::string>::iterator it =
        _headers.find("content_type");
    if (it == _headers.end())
      throw std::runtime_error("500");
    envType += (*it).second;
    it = _headers.find("content_length");
    if (it == _headers.end())
      throw std::runtime_error("500");
    addVariableToEnv(vEnv, "CONTENT_LENGTH=" + (*it).second);
  }
  addVariableToEnv(vEnv, envType);
  addHeaderToEnv(vEnv, "HTTP_COOKIE", "cookie");
  addHeaderToEnv(vEnv, "HTTP_USER_AGENT", "user-agent");
  addVariableToEnv(vEnv, "SERVER_SOFTWARE=Webserv/1.0.0");
  addVariableToEnv(vEnv, "SERVER_NAME=" + _server->getServerNames()[0]);
  addVariableToEnv(vEnv, "SERVER_PROTOCOL=HTTP/1.1");
  addHeaderToEnv(vEnv, "HTTP_ACCEPT", "accept");
  addVariableToEnv(vEnv, "DOCUMENT_ROOT=" + _location->getRoot());
  std::stringstream ss;
  ss << _server->getPort();
  addVariableToEnv(vEnv, "SERVER_PORT=" + ss.str());
  addVariableToEnv(vEnv, "SCRIPT_NAME=" + _location->getCgiFile(_sUri));
  if (_sPathInfo.empty() == false) {
    addVariableToEnv(vEnv, "PATH_INFO=" + _sPathInfo);
    addVariableToEnv(vEnv,
                     "PATH_TRANSLATED=" + _location->getRoot() + _sPathInfo);
  }
  char buf[4096];
  if (getcwd(buf, 4096) == NULL)
    throw std::runtime_error("500");
  std::string scriptFilename = "SCRIPT_FILENAME=";
  scriptFilename += buf;
  addVariableToEnv(vEnv, scriptFilename + _location->getCgiFile(_sUri));
  for (size_t i = 0; i < vEnv.size(); i++) {
    std::cout << YELLOW << "vEnv[i=" << i << "]: " << vEnv[0] << RESET
              << std::endl;
  }
  return;
}

void Client::setupChild(std::string &cgiPathScript) {
  std::vector<const char *> vEnv;
  if (chdir(cgiPathScript.c_str()) == -1) {
    throw std::runtime_error("500");
  }
  if (_sMethod == "{POST}") {
    cgiPOSTMethod();
  }
  cgiOutfile();
  buildEnv(vEnv);
  if (execve() == -1)
    throw std::runtime_error("500");
}

void Client::setupCgi() {

  std::string cgiPathExec = _location->getExecutePath(_sUri);
  std::string cgiPathScript = _location->getCgiFile(_sUri);

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
    setupChild(cgiPathScript);
  }
  if (pid > 0) {
    _cgiPid = pid;
    _infileCgi = cgiPathScript + _infileCgi;
    _outfileCgi = cgiPathScript + _outfileCgi;
    return;
  }
}
