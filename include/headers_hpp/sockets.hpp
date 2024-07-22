#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include <errno.h>
# include <netdb.h>
# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <cstring>
# include <sys/wait.h>
# include "signal.h"

const int BACKLOG_LISTEN = 100;

enum	signal_handler
{
	IGNORE = 0,
	DEFAULT = 1,
	STOP = 2,
};

enum	status
{
	SUCCESS = 0,
	FAILURE = 1,
	BAD_FD = -1,
};

int		createServerSocket(int port, uint32_t address);
int		changeEpollEvents(int epollFd, int socket, uint32_t	events);
int		addSocketToEpoll(int epollFd, int socketFd, uint32_t events);
int		protectedClose(int fd);
int		getExitStatus(int codeExit);
int		handleSignal(int signal, int option);
bool	checkEvent(uint32_t events, uint32_t eventToCheck);


#endif
