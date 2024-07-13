#include "Webserv.hpp"

int	changeEpollEvents(int epollFd, int socket, uint32_t	events)
{
	struct epoll_event epollEvent;

	epollEvent.events = events;
	epollEvent.data.fd = socket;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, socket, &epollEvent) != 0)
	{
		std::cerr << "webserv: Webserv::changeEpollEvents: epoll_ctl: " << strerror(errno) << std::endl;
		return (1);
	}
	return (0);	
}