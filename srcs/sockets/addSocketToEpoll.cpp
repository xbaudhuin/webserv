#include "Webserv.hpp"

int	addSocketToEpoll(int epollFd, int socketFd, uint32_t events)
{
	struct epoll_event	epollEvent;
	int					status;
	
	std::memset(&epollEvent, 0, sizeof (epollEvent));
	epollEvent.events = events;
	epollEvent.data.fd = socketFd;
	status = epoll_ctl(epollFd, EPOLL_CTL_ADD, socketFd, &epollEvent);
	if (status != 0)
	{
		std::cerr << "webserv: Webserv::addSocketToEpoll: epoll_ctl: " << strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "webserv: successfully add socket fd " << socketFd << " to epoll fd " << epollFd << std::endl;
	return (0);
}