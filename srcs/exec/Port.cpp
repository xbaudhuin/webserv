#include "Port.hpp"

Port::Port() : _address(INADDR_LOOPBACK), _port(4243) {
	return ;
}

Port::Port(ServerConf &serv) : _main(&serv), _address(serv.getHost()), _port(serv.getPort()) {
	for (size_t i = 0; i < serv.getServerNames().size(); i++) {
    	this->_portConfs[serv.getServerNames()[i]] = &serv;
	}
}

Port::~Port(void) {
	return ;
}

Port::Port(const Port &otherPort) {
	if (this != &otherPort) {
		*this = otherPort;
	}
	return ;
}

Port &Port::operator=(const Port &otherPort) {
	if (this != &otherPort) {
		this->_port = otherPort._port;
		this->_serverSocket = otherPort._serverSocket;
		this->_clientSockets = otherPort._clientSockets;
		this->_clientRequests = otherPort._clientRequests;
		this->_portConfs = otherPort._portConfs;
		this->_main = otherPort._main;
		this->_address = otherPort._address;
	}		
	return *this;
}

int	Port::acceptNewConnection(void) {
	int clientSocket = accept4(this->_serverSocket, NULL, NULL, (SOCK_NONBLOCK | SOCK_CLOEXEC));

	if (clientSocket == BAD_FD) {
		std::cerr << "webserv: Port::acceptNewConncetion: accept4: " << strerror(errno) << std::endl;
		return BAD_FD;
	}
	try {
		this->_clientSockets.push_back(clientSocket);
	}
	catch(const std::exception& e) {
		protectedClose(clientSocket);
		std::cerr << "webserv: Port::acceptNewConnection: " << e.what() << std::endl;
		return BAD_FD;
	}
	try {
		this->_clientRequests.insert(std::make_pair(clientSocket, Client(clientSocket, this->_portConfs, this->_main)));
	}
	catch(const std::exception& e) {
		protectedClose(clientSocket);
		std::cerr << "webserv: Port::acceptNewConnection: " << e.what() << std::endl;
		this->removeSocketFromClientVector(clientSocket);
		return BAD_FD;
	}
	std::cout << "webserv: new client connection accepted on " << this->_address << ":" << this->_port
		<< " to socket fd " << clientSocket << std::endl;
	return clientSocket;
}

int	Port::removeSocketFromRequestMap(int socket) {
	size_t	n = this->_clientRequests.erase(socket);

	if (n == 0) {
		std::cerr << "webserv: Port::removeSocketFromRequestMap: trying to remove non existing client socket "
			<< socket << " from request map of " << this->_address << ":" << this->_port << std::endl;
		return FAILURE;
	}
	else {
		std::cout << "webserv: successfully remove client socket " << socket << " in Port request map listening on "
			<< this->_address << ":" << this->_port << std::endl;
		return SUCCESS;
	}
}

int	Port::removeSocketFromClientVector(int socket) {
	std::vector<int>::iterator	iter = std::find(this->_clientSockets.begin(), this->_clientSockets.end(), socket);

	if (iter != this->_clientSockets.end()) {
		this->_clientSockets.erase(iter);
		std::cout << "webserv: successfully remove client socket " << socket
			<< " in Port vector listening on port " << this->_address << ":" << this->_port << std::endl;
		return SUCCESS;
	}
	else {
		std::cerr << "webserv: Port::removeClientSocket: trying to remove non existing client socket from vector of port "
			<< this->_address << ":" << this->_port << std::endl;
		return FAILURE;
	}
}

int	Port::removeClientSocket(int clientSocket) {
	int status = SUCCESS;

	status += this->removeSocketFromClientVector(clientSocket);
	status += this->removeSocketFromRequestMap(clientSocket);
	return status;
}

bool	Port::isClientSocket(int fd) const {
	if (std::find(this->_clientSockets.begin(), this->_clientSockets.end(), fd) != this->_clientSockets.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool	Port::isServerSocket(int fd) const {
	if (fd == this->_serverSocket) {
		return true;
	}
	else {
		return false;
	}
}

int	Port::initPortSocket(void) {
	this->_serverSocket = createServerSocket(this->_port, this->_address);
	return this->_serverSocket;
}

int	Port::getPort(void) const {
	return this->_port;
}

bool	Port::isOldClient(int fd) const {
	mapClients::const_iterator	iter = this->_clientRequests.find(fd);

	if (iter == this->_clientRequests.end()) {
		std::cerr << "webserv: Port::isOldClient: fd " << fd  << " is not in map of requests" << std::endl;
		return false;
	}
	else {
		return (*iter).second.isTimedOut();
	}
}

bool	Port::isOldChild(int fd) const {
	mapClients::const_iterator	iter = this->_clientRequests.find(fd);

	if (iter == this->_clientRequests.end()) {
		std::cerr << "webserv: Port::isOldChid: fd " << fd  << " is not in map of requests" << std::endl;
		return false;
	}
	else {
		return (*iter).second.isTimedOutCgi();
	}
}

Client	*Port::getClient(int clientSocket) {
	mapClients::iterator	iter = this->_clientRequests.find(clientSocket);

	if (iter == this->_clientRequests.end()) {
		return NULL;
	}
	else {
		return &(*iter).second;
	}
}

void	Port::printPortConfs(void) {
	std::cout << "Main = "<< *(this->_main) << std::endl;
	mapConfs::iterator	iter = this->_portConfs.begin();
	while (iter != this->_portConfs.end()) {
		std::cout << "---SERVER NAME : " <<(*iter).first << "---" << std::endl << std::endl;
		std::cout << *(*iter).second << std::endl;
		iter++;
	}
}

void	Port::addToConf(const std::string &name, ServerConf *newConf) {
	this->_portConfs[name] = newConf;
}
