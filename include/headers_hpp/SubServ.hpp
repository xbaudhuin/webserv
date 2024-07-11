#ifndef SUBSERV_HPP
# define SUBSERV_HPP

# include "Config.hpp"
# include <vector>
# include "sockets.hpp"

typedef std::map<std::string, ServerConf*> mapConfs;

class	SubServ
{
	public:
							SubServ(void);
							SubServ(ServerConf &conf);
							~SubServ(void);
							// SubServ(const SubServ &otherSubServ);
		SubServ				&operator=(const SubServ &otherSubServ);
		mapConfs			_portConfs;
		int					acceptNewConnection(void);
		int					removeClientSocket(int socketFd);
		bool				isClientSocket(int socketFd);
		bool				isServerSocket(int socketFd);
		int					initServerSocket(void);
		const ServerConf	*getConf(const std::string &serverName);
		int					getPort(void);

	protected:

	private:
		const ServerConf	*_main;
		int					_port;
		int					_serverSocket;
		std::vector<int>	_clientSockets;

};

#endif
