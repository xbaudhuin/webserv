#include "sockets.hpp"

int	protectedClose(int fd)
{
	int	status = close(fd);

	if (status == BAD_FD) {
		std::cerr << "webserv: close on fd " << fd << ": " << strerror(errno) << std::endl;
	}
	return status;
}