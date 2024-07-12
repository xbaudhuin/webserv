#include "Webserv.hpp"

int	protectedClose(int fd)
{
	int	status;

	status = close(fd);
	if (status == -1)
	{
		std::cerr << "webserv: close on fd " << fd << ": " << strerror(errno) << std::endl;
	}
	return (status);
}