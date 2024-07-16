#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include <errno.h>
# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string>
# include <iostream>
# include <sstream>
# include <vector>
# include <unistd.h>
# include <sys/socket.h>
# include <string.h>
# include <sys/epoll.h>
# include <algorithm>
# include <cstring>
# include <iterator>
# include <fcntl.h>

int		createServerSocket(int port);
int		changeEpollEvents(int epollFd, int socket, uint32_t	events);
int		addSocketToEpoll(int epollFd, int socketFd, uint32_t events);
int		protectedClose(int fd);
int		handleSignal(int signal, int option);
bool	checkEvent(uint32_t events, uint32_t eventToCheck);


#endif
