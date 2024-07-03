#include "Webserv.hpp"
#include "SubServ.hpp"

int	testRemoveClientSocket()
{
	SubServ	myServ(4245);
	myServ.tests();
	return (0);
}

int	main()
{
	testRemoveClientSocket();
}