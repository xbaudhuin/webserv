#ifndef SubServ_HPP
# define SubServ_HPP

# include <vector> 
# include "Config.hpp"

class	SubServ
{
	public:
				SubServ(void);
				~SubServ(void);
				SubServ(const SubServ &otherSubServ);
		SubServ	&operator=(const SubServ &otherSubServ);

	protected:

	private:
		int					_port;
		int					_serverSocket;
		std::vector<int>	_clientSockets;
		map_confs			_configs;
		
};

#endif
