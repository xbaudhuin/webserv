#include "sockets.hpp"

static struct sockaddr_in	getSocketAddress(in_addr_t	address, int rawPort)
{
	struct sockaddr_in	newSocketAddress;

	std::memset(&newSocketAddress, 0, sizeof (newSocketAddress));
	newSocketAddress.sin_family = AF_INET;
	newSocketAddress.sin_addr.s_addr = address;
	newSocketAddress.sin_port = htons(rawPort);
	return (newSocketAddress);	
}

int	createServerSocket(int port, uint32_t address)
{
	int					serverSocket;
	struct sockaddr_in	socketAddress;

	socketAddress = getSocketAddress(address, port);
	serverSocket = socket(socketAddress.sin_family, SOCK_STREAM, 0);
	if (serverSocket == BAD_FD)
	{
		std::cerr << "webserv: createServerSocket: socket: " << strerror(errno) << std::endl;
		return (BAD_FD);
	}
	if (bind(serverSocket, (struct sockaddr *)&socketAddress, sizeof (socketAddress)) != SUCCESS)
	{
		std::cerr << "webserv: bind: " << strerror(errno) << std::endl;
		protectedClose(serverSocket);
		return (BAD_FD);
	}
	if (listen(serverSocket, BACKLOG_LISTEN) != SUCCESS)
	{
		std::cerr << "webserv: createServerSocket: listen: " << strerror(errno) << std::endl;
		protectedClose(serverSocket);
		return (BAD_FD);
	}
	std::cout << "webserv: created server socket on fd " << serverSocket << " for port " << port << std::endl;
	return (serverSocket);
}