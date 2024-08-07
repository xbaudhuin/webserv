#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/epoll.h>
#include <algorithm>
#include <map>
#include <fcntl.h>
#include "../include/headers_hpp/Port.hpp"


#define PORT_1 4244
#define PORT_2 4245
#define MAX_EVENTS 500

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

struct sockaddr_in	get_socket_address(in_addr_t raw_address, int raw_port)
{
	struct sockaddr_in	new_socket_address;

	memset(&new_socket_address, 0, sizeof (new_socket_address));
	new_socket_address.sin_family = AF_INET; /* sin_family is always AF_INET for sockaddr_in struct which support only IPv4 address */
	new_socket_address.sin_addr.s_addr = htonl(raw_address);
	new_socket_address.sin_port = htons(raw_port);
	return (new_socket_address);
}

int	create_server_socket(int port)
{
	struct sockaddr_in	socket_address;
	int					server_fd;

	socket_address = get_socket_address(INADDR_LOOPBACK, port);
	server_fd = socket(socket_address.sin_family, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cerr << "webserv: socket: " << strerror(errno) << std::endl;
		return (-1);
	}
	if (bind(server_fd, (struct sockaddr *)&socket_address, sizeof (socket_address)) == -1)
	{
		std::cerr << "webserv: bind: " << strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "webserv: created server socket on port " << port << std::endl;
	fcntl(server_fd, F_SETFL, O_NONBLOCK);
	return (server_fd);
}

void	read_data_from_socket(int sender_fd, int epoll_fd, std::vector<int> &client_sockets)
{
	#define BUFSIZ_2 4096
	char				buffer[BUFSIZ];
	int					bytes_read;
	int					status;
	int					dest_fd;
	std::stringstream	ss;

	bytes_read = recv(sender_fd, buffer, BUFSIZ - 1, 0);
	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
			std::cout << "[" << sender_fd << "] Client socket closed connection." << std::endl;
		else
			std::cerr << "[Server] Recv error: " << strerror(errno) << std::endl;
		epoll_ctl(epoll_fd, sender_fd, EPOLL_CTL_DEL, NULL);
		close(sender_fd);
		client_sockets.erase(std::find(client_sockets.begin(), client_sockets.end(), sender_fd));
		return ;
	}
	else
	{
		buffer[bytes_read] = '\0';
		std::cout << "[" << sender_fd << "] Got message: " << buffer;
		std::cout << "bytes read = " << bytes_read << std::endl;
		ss << "[" << sender_fd << "] says: " << buffer;
		for (size_t i = 0; i < client_sockets.size(); i++)
		{
			dest_fd = client_sockets[i];
			if (dest_fd != sender_fd)
			{
				status = send(dest_fd, ss.str().c_str(), ss.str().length(), 0);
				if (status == -1)
					std::cerr << "[Server] Send error to client fd [" << dest_fd << "]: " << strerror(errno) << std::endl; 	
			}
		}
		ss.clear();
	}
	changeEpollEvents(epoll_fd, sender_fd,  EPOLLOUT | EPOLLRDHUP | EPOLLIN);
}

void	add_client_socket_to_epoll(int epoll_fd, int client_socket, std::vector<int> &client_sockets, uint32_t events)
{
	struct epoll_event	e_event;

	e_event.events = events;
	e_event.data.fd =client_socket;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &e_event) != 0)
	{
		std::cerr << "webserv: epoll_ctl: " << strerror(errno) << std::endl;
		close(client_socket);
		return ;
	}
	client_sockets.push_back(client_socket);
}



void	accept_new_connection(int server_socket, int epoll_fd, std::vector<int> &client_sockets)
{
	int					client_fd;
	std::stringstream	string_stream;

	client_fd = accept(server_socket, NULL, NULL);
	if (client_fd == -1)
	{
		std::cerr << "webserv: accept: " << strerror(errno) << std::endl;
		return ;
	}
	add_client_socket_to_epoll(epoll_fd, client_fd, client_sockets, EPOLLIN | EPOLLRDHUP);
	std::cout << "webserv: accepted new client socket on fd " << client_fd << std::endl;
	string_stream << "webserv: welcome, you are client fd " << client_fd << std::endl;
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	if (send(client_fd, string_stream.str().c_str(), string_stream.str().length(), 0) == -1)
	{
		std::cerr << "webserv: send: " << strerror(errno) << std::endl;
		close(client_fd);
		client_sockets.erase(std::find(client_sockets.begin(), client_sockets.end(), client_fd));
	}
}

void	printEvent(uint32_t event)
{
	std::string	sEvent;

	if (event == EPOLLIN)
		sEvent = "EPOLLIN";
	else if (event == EPOLLOUT)
		sEvent = "EPOLLOUT";
	else if (event == (EPOLLIN & EPOLLOUT))
		sEvent = "EPOLLIN & EPOLLOUT";
	else if (event == (EPOLLIN | EPOLLOUT))
		sEvent = "EPOLLIN | EPOLLOUT";
	else if (event == EPOLLHUP)
		sEvent = "EPOLLHUP";
	else if (event == EPOLLRDHUP)
		sEvent = "EPOLLRDHUP";
	else if (event == EPOLLONESHOT)
		sEvent = "EPOLLONESHOT";
	else if (event == EPOLLET)
		sEvent = "EPOLLET";
	else if (event == (EPOLLIN | EPOLLRDHUP))
		sEvent = "EPOLLIN | EPOLLRDHUP";
	else
	{
		std::cout << "Event = " << event << std::endl;
		return ;
	}
	std::cout << "Event = " << sEvent << std::endl;
}

void	respondToClient(int epoll_fd, int client_socket)
{
	int status;
	std::string	response = "RESPONSE";

	status = send(client_socket, response.c_str(), response.size(), 0);
	if (status < 0)
	{
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
		close(client_socket);
		std::cout << "fd " << client_socket << " removed from epoll and closed" << std::endl;
		return ;
	}
	std::cout << "status send = " << status << std::endl;
	changeEpollEvents(epoll_fd, client_socket, EPOLLIN | EPOLLRDHUP);
	return ;
}

int	main()
{
	int	server_socket;
	std::vector<int>	server_sockets;
	std::vector<int>	client_sockets;
	int					epoll_fd;
	int					status;
	struct epoll_event	events[MAX_EVENTS];

	server_socket = create_server_socket(PORT_1);
	if (server_socket == -1)
	{
		return (1);
	}
	if (listen(server_socket, 10) != 0)
	{
		std::cerr << "webserv: listen: " << strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "webserv: listening on port " << PORT_1 << " for fd " << server_socket <<  std::endl;
	epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	add_client_socket_to_epoll(epoll_fd, server_socket, server_sockets, EPOLLIN);
	if (epoll_fd == -1)
	{
		std::cerr << "webserv: epoll_create: " << strerror(errno) << std::endl;
		return (1);
	}
	while (true)
	{
		status = epoll_wait(epoll_fd, events, MAX_EVENTS, 2000);
		if (status == -1)
		{
			std::cerr << "webserv: epoll_wait: " << strerror(errno) << std::endl;
			continue ;
		}
		else if (status == 0)
		{
			std::cout << "webserv: waiting..." << std::endl;
			continue ;
		}
		for (int i = 0; i < status; i++)
		{
			std::cout << "fd = " << events[i].data.fd << std::endl;
			printEvent(events[i].events);
			if (std::find(server_sockets.begin(), server_sockets.end(), events[i].data.fd) != server_sockets.end())
			{
				accept_new_connection(events[i].data.fd, epoll_fd, client_sockets);
			}
			else if (checkEvent(events[i].events, EPOLLRDHUP))
			{
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
				close(events[i].data.fd);
				std::cout << "fd " << events[i].data.fd << " removed from epoll and closed" << std::endl;
			}
			else if (checkEvent(events[i].events, EPOLLIN))
			{
				read_data_from_socket(events[i].data.fd, epoll_fd, client_sockets);	
			}
			else if (checkEvent(events[i].events, EPOLLOUT))
			{
				respondToClient(epoll_fd, events[i].data.fd);
			}
		}
		sleep(2);
	}
	close(server_socket);
	return (0);
}
