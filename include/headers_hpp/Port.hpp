#ifndef Port_HPP
# define Port_HPP

# include "sockets.hpp"
# include "Client.hpp"
# include "Typedef.hpp"
# include "ServerConf.hpp"

class	Port
{
	public:
							Port(void);
							Port(ServerConf &conf);
							~Port(void);
							Port(const Port &otherPort);
		Port				&operator=(const Port &otherPort);
		int					acceptNewConnection(void);
		int					removeClientSocket(int clientSocket);
		bool				isClientSocket(int fd);
		bool				isServerSocket(int fd);
		int					initPortSocket(void);
		int					getPort(void);
		Client				*getClient(int clientSocket);
		int					addClientsToBounce(std::vector<int> &clientsToBounce);
		void				printPortConfs(void);
		void				addToConf(const std::string &name, ServerConf *newConf);

	protected:

	private:
		ServerConf			*_main;
		mapConfs			_portConfs;
		uint32_t			_address;
		int					_port;
		int					_serverSocket;
		std::vector<int>	_clientSockets;
		mapClients			_clientRequests;
		int					removeSocketFromClientVector(int socket);
		int					removeSocketFromRequestMap(int socket);
};

#endif
