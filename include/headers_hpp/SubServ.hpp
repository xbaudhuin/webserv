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
		int					removeClientSocket(int clientSocket);
		bool				isClientSocket(int fd);
		bool				isServerSocket(int fd);
		int					initPortSocket(void);
		const ServerConf	*getConf(const std::string &serverName);
		int					getPort(void);
		void				addClientsToBounce(std::vector<int> &clientsToBounce);

	protected:

	private:
		ServerConf			*_main;
		int					_port;
		int					_serverSocket;
		std::vector<int>	_clientSockets;
		mapClients			_clientRequests;
		int					removeSocketFromClientVector(int socket);
		int					removeSocketFromRequestMap(int socket);
};

#endif
