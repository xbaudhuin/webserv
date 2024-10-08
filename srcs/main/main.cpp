#include "Webserv.hpp"
#include "cgiException.hpp"

int gSignal = 0;

static int initMain(int argc) {
  if (argc > 2) {
    std::cout << "webserv: takes at maximum one argument" << std::endl;
    return FAILURE;
  }
  int fd = open("./log/stderr.log", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd != BAD_FD) {
    if (dup2(fd, STDERR_FILENO) == BAD_FD) {
      int err = errno;
      std::cerr << "webserv: dup2: " << strerror(err) << std::endl;
    }
    close(fd);
  }
  fd = open("./log/stderr_child.log", O_CREAT | O_WRONLY | O_TRUNC, 00644);
  if (fd == BAD_FD) {
    int err = errno;
    std::cerr << "webserv: fail to open ./log/stderr_child.log: "
              << strerror(err) << std::endl;
  } else
    close(fd);
  if (handleSignal(SIGINT, STOP) != SUCCESS) {
    return FAILURE;
  }
  return SUCCESS;
}

int main(int argc, char **argv, char **env) {
  if (initMain(argc) != SUCCESS) {
    return FAILURE;
  }
  try {
    Webserv serv(argv[1]);
    serv.addEnv(env);
    serv.start();
  } catch (const cgiException &e) {
    return FAILURE;
  } catch (const Webserv::StopServer &e) {
    std::cout << "webserv: server is stopping" << std::endl;
    return SUCCESS;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return FAILURE;
  }
  return SUCCESS;
}
