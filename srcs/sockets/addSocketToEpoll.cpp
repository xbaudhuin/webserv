#include "sockets.hpp"

int	addSocketToEpoll(int epollFd, int socketFd, uint32_t events) {
	struct epoll_event	epollEvent;
	
	std::memset(&epollEvent, 0, sizeof (epollEvent));
	epollEvent.events = events;
	epollEvent.data.fd = socketFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socketFd, &epollEvent) != SUCCESS) {
		std::cerr << "webserv: addSocketToEpoll: epoll_ctl: " << strerror(errno) << std::endl;
		return FAILURE;
	}
	else {
		std::cout << "webserv: successfully add socket fd " << socketFd << " to epoll fd " << epollFd << std::endl;
		return SUCCESS;
	}
}