#include "Client.hpp"

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
    throw std::runtime_error("Client::cgiPOSTMethod: fail to open infileCgi");
  }
  ssize_t writeBytes = write(fd, &_vBody[0], _vBody.size());
  close(fd);
  fd = -1;
  if (writeBytes == -1)
    throw std::runtime_error(
        "Client::cgiPOSTMethod: fail to write to infileCgi");
  if (static_cast<size_t>(writeBytes) < _vBody.size()) {
    throw std::runtime_error(
        "Client::cgiPOSTMethod: writeBytes < _body.size()");
  }
  fd = open(_infileCgi.c_str(), O_RDWR);
  if (fd == -1) {
    throw std::runtime_error(
        "Client::cgiPOSTMethod: fail to re-open infileCgi");
  }
  if (dup2(fd, STDIN_FILENO) == -1) {
    close(fd);
    throw std::runtime_error(
        "Client::cgiPOSTMethod: fail to dup2 infileCgi, STDIN");
  }
  close(fd);
}

void Client::cgiOutfile(void) {
  char buf[1000];
  getcwd(buf, 1000);
  int fd = open(_outfileCgi.c_str(), O_RDWR | O_CREAT | O_TRUNC, 00644);
  if (fd == -1) {
    std::string tmp = "Client::cgiOutfile: fail to create outfileCgi: ";
    tmp += _outfileCgi;
    throw std::runtime_error(tmp.c_str());
  }
  ssize_t ret = write(fd, "HTTP/1.1 200 OK\r\n", 17);
  if (ret < 17) {
    close(fd);
    throw std::runtime_error("Client::cgiOutfile: fail to write to outfile");
  }
  if (dup2(fd, STDOUT_FILENO) == -1) {
    close(fd);
    throw std::runtime_error(
        "Client::cgiOutfile: fail to dup 2(outfileCgi, STDOUT)");
  }
  close(fd);
}

void Client::addHeaderToEnv(std::vector<char *> &vEnv,
                            const std::string &envVariable,
                            const std::string &headerKey) {
  std::map<std::string, std::string>::iterator it;
  char *str = NULL;
  it = _headers.find(headerKey);
  if (it != _headers.end()) {
    std::string tmp = envVariable + "=" + (*it).second;
    str = strdup(tmp.c_str());
    if (str == NULL)
      throw std::bad_alloc();
    try {
      vEnv.push_back(str);
    } catch (std::exception &e) {
      free(str);
      throw std::bad_alloc();
    }
  }
}

void Client::addVariableToEnv(std::vector<char *> &vEnv,
                              const std::string &envVariable) {
  char *str = strdup(envVariable.c_str());
  if (str == NULL)
    throw std::bad_alloc();
  try {
    vEnv.push_back(str);
  } catch (std::exception &e) {
    free(str);
    throw std::bad_alloc();
  }
}

void Client::buildEnv(std::vector<char *> &vEnv) {
  for (size_t i = 0; environ[i]; i++) {
    addVariableToEnv(vEnv, environ[i]);
  }
  addVariableToEnv(vEnv, "REQUEST_METHOD=" + _sMethod);
  std::string envType = "CONTENT_TYPE=";
  if (_sMethod == "GET") {
    addVariableToEnv(vEnv, "QUERY_STRING=" + _sQueryUri);
    envType += "application/x-www-form-urlencoded";
  } else {
    std::map<std::string, std::string>::iterator it =
        _headers.find("content-type");
    if (it == _headers.end())
      throw std::runtime_error("Client::buildEnv: no content-type header");
    std::stringstream ss;
    ss << _vBody.size();
    addVariableToEnv(vEnv, "CONTENT_LENGTH=" + ss.str());
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
    throw std::runtime_error("Client::buildEnv: fail to getcwd");
  std::string scriptFilename = "SCRIPT_FILENAME=";
  scriptFilename += buf;
  addVariableToEnv(vEnv, scriptFilename + _location->getCgiFile(_sUri));
  vEnv.push_back(NULL);
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
  addVariableToEnv(arg, _location->getExecutePath(_sUri).c_str());
  addVariableToEnv(arg, _location->getCgiFile(_sUri));
  arg.push_back(NULL);
}

void Client::setupChild(std::string &cgiPathScript) {
  std::vector<char *> vEnv;
  std::vector<char *> argument;
  try {
    if (chdir(cgiPathScript.c_str()) == -1) {
      std::string s = "Client::setupChild: fail to chdir to: " + cgiPathScript;
      throw std::runtime_error(s);
    }
    if (_sMethod == "POST") {
      cgiPOSTMethod();
    }
    cgiOutfile();
    buildEnv(vEnv);
    buildArguments(argument);
    std::cerr << "Client::setupChild: starting execve" << std::endl;
    if (execve(argument[0], &argument[0], &vEnv[0]) == -1) {
      std::string info = "Client::setupChild: fail to excve: ";
      info += "arg[0] = ";
      info += argument[0];
      info += "; arg[1] = ";
      info += argument[1];
      throw std::runtime_error(info);
    }
  } catch (std::exception &e) {
    freeVector(vEnv, argument);
    throw cgiException(e.what());
  }
}

void Client::setupCgi() {

  std::string cgiPathExec = _location->getExecutePath(_sUri);
  std::string cgiPathScript = _location->getCgiPath(_sUri);
  std::ostringstream ss;
  ss << _socket;
  ss.str("");
  ss.clear();
  _infileCgi = "webserv_in" + ss.str();
  _outfileCgi = "webserv_out" + ss.str();
  ss << _server->getHost();
  _infileCgi += "_host" + ss.str();
  _outfileCgi += "_host" + ss.str();
  ss.str("");
  ss.clear();
  ss << _server->getPort();
  _infileCgi += "_port" + ss.str();
  _outfileCgi += "_port" + ss.str();
  time_t time = getTime();
  ss.str("");
  ss.clear();
  ss << time;
  _infileCgi += "t" + ss.str();
  _outfileCgi += "t" + ss.str();

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
