#ifndef SUBSERV_HPP
# define SUBSERV_HPP

# include "Config.hpp"
# include <vector>

typedef std::map<std::string, ServerConf&> mapConfs;

class	SubServ
{
	public:
							SubServ(void);
							SubServ(int port);
							~SubServ(void);
							SubServ(const SubServ &otherSubServ);
		SubServ				&operator=(const SubServ &otherSubServ);
		mapConfs			_portConfs;

	protected:

	private:
		int					_port;
		int					_serverSocket;
		std::vector<int>	_clientSockets;

};

#endif
