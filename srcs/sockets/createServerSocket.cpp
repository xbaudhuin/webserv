#include "Webserv.hpp"

static struct sockaddr_in	getSocketAddress(in_addr_t	rawAddress, int rawPort)
{
	struct sockaddr_in	newSocketAddress;

	std::memset(&newSocketAddress, 0, sizeof (newSocketAddress));
	newSocketAddress.sin_family = AF_INET;
	newSocketAddress.sin_addr.s_addr = htonl(rawAddress);
	newSocketAddress.sin_port = htons(rawPort);
	return (newSocketAddress);	
}

/* Creates a server socket and binds it on the port argument (ex: 4243).
We assume IP adress is IPv4 and 127.0.0.1 */
int	createServerSocket(int port)
{
	int					serverSocket;
	struct sockaddr_in	socketAddress;

	socketAddress = getSocketAddress(INADDR_LOOPBACK, port);
	serverSocket = socket(socketAddress.sin_family, SOCK_STREAM, 0);
	if (serverSocket ==-1)
	{
		std::cerr << "webserv: createServerSocket: socket: " << strerror(errno) << std::endl;
		return (-1);
	}
	if (bind(serverSocket, (struct sockaddr *)&socketAddress, sizeof (socketAddress)) != 0)
	{
		std::cerr << "webserv: bind: " << strerror(errno) << std::endl;
		close(serverSocket);
		return (-1);
	}
	if (listen(serverSocket, BACKLOG_LISTEN) != 0)
	{
		std::cerr << "webserv: createServerSocket: listen: " << strerror(errno) << std::endl;
		close(serverSocket);
		return (-1);
	}
	std::cout << "webserv: created server socket on fd " << serverSocket << " for port " << port << std::endl;
	return (serverSocket);
}