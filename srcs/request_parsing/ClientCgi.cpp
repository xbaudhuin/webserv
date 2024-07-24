#include "Client.hpp"
#include "cgiException.hpp"
#include <complex>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <new>
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
    throw std::runtime_error("fail to open infileCgi");
  }
  ssize_t writeBytes = write(fd, &_vBody[0], _vBody.size());
  close(fd);
  fd = -1;
  if (writeBytes == -1)
    throw std::runtime_error("fail to write to infileCgi");
  if (static_cast<size_t>(writeBytes) < _vBody.size()) {
    throw std::runtime_error("writeBytes < _body.size()");
  }
  fd = open(_infileCgi.c_str(), O_RDWR);
  if (fd == -1) {
    // unlink?
    throw std::runtime_error("fail to re-open infileCgi");
  }
  if (dup2(fd, STDIN_FILENO) == -1) {
    close(fd);
    throw std::runtime_error("fail to dup2 infileCgi, STDIN");
  }
  close(fd);
}

void Client::cgiOutfile(void) {
  int fd = open(_outfileCgi.c_str(), O_RDWR | O_CREAT | O_TRUNC, 00644);
  if (fd == -1) {
    perror("");
    std::string tmp = "fail to create outfileCgi: ";
    tmp += _outfileCgi;
    throw std::runtime_error(tmp.c_str());
  }
  if (dup2(fd, STDOUT_FILENO) == -1) {
    close(fd);
    throw std::runtime_error("fail to dup 2(outfileCgi, STDOUT)");
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

void Client::addVariableToEnv(std::vector<char *> &vEnv,
                              const std::string &envVariable) {
  vEnv.push_back(envVariable.c_str());
}

void Client::buildEnv(std::vector<char *> &vEnvariable) {
  std::vector<const char *> vEnv;
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
  vEnv.push_back(NULL);
  // char *dup;
  // for (size_t i = 0; i < vEnv.size(); i++) {
  //   dup = strdup(vEnv[i]);
  //   if (dup == NULL)
  //     throw std::bad_alloc();
  //   vEnvariable.push_back(dup);
  // }
  // vEnvariable.push_back(NULL);
  return;
}

void Client::freeVector(std::vector<char *> &vEnv,
                        std::vector<char *> &argument) {
  for (size_t i = 0; i < vEnv.size(); i++) {
    free(vEnv[i]);
  }
  for (size_t i = 0; i < argument.size(); i++) {
    free(argument[i]);
  }
}

void Client::buildArguments(std::vector<char *> &arg) {
  char *dup = strdup(_location->getExecutePath(_sUri).c_str());
  if (dup == NULL)
    throw std::bad_alloc();
  arg.push_back(dup);
  std::string pathScript = "./" + _location->getCgiPath(_sUri);
  dup = strdup(pathScript.c_str());
  if (dup == NULL)
    throw std::bad_alloc();
  arg.push_back(dup);
  arg.push_back(NULL);
}

void Client::setupChild(std::string &cgiPathScript) {
  std::vector<char *> vEnv;
  std::vector<char *> argument;
  try {
    if (chdir(cgiPathScript.c_str()) == -1) {
      throw std::runtime_error("fail to chdir");
    }
    if (_sMethod == "POST") {
      cgiPOSTMethod();
    }
    cgiOutfile();
    buildEnv(vEnv);
    buildArguments(argument);
    std::cerr << "starting execve" << std::endl;
    if (execve(argument[0], &argument[0], &vEnv[0]) == -1)
      throw std::runtime_error("fail to execve");
  } catch (std::exception &e) {
    freeVector(vEnv, argument);
    std::string tmp = "Client::setupChild: ";
    tmp += e.what();
    throw cgiException(tmp.c_str());
  }
}

void Client::setupCgi() {

  std::string cgiPathExec = _location->getExecutePath(_sUri);
  std::string cgiPathScript = _location->getCgiPath(_sUri);
  std::ostringstream ss;
  ss << _socket;
  ss.clear();
  _infileCgi = "webserv_in" + ss.str();
  _outfileCgi = "webserv_out" + ss.str();
  ss << _server->getHost();
  _infileCgi += "host" + ss.str();
  _outfileCgi += "host" + ss.str();
  ss.clear();
  ss << _server->getPort();
  _infileCgi += "port" + ss.str();
  _outfileCgi += "port" + ss.str();

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
