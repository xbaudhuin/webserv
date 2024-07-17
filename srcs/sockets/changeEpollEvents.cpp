#include "sockets.hpp"

int	changeEpollEvents(int epollFd, int socket, uint32_t	events)
{
	struct epoll_event epollEvent;

	std::memset(&epollEvent, 0, sizeof (epollEvent));
	epollEvent.events = events;
	epollEvent.data.fd = socket;
	if (epoll_ctl(epollFd, EPOLL_CTL_MOD, socket, &epollEvent) != SUCCESS)
	{
		std::cerr << "webserv: Webserv::changeEpollEvents: epoll_ctl: " << strerror(errno) << std::endl;
		return (FAILURE);
	}
	return (SUCCESS);	
}