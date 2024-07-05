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

int	createServerSocket(int port);

#endif
