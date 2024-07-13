#include "Webserv.hpp"

extern int gSignal;

void	setGlobalSignal(int signal)
{
	gSignal = signal;
	std::cout << "webserv: sigint received" << std::endl;
}

int	handleSignal(int signal, int option)
{
	struct sigaction	sig;

	std::memset(&sig, 0, sizeof (sig));
	if (option == DEFAULT)
	{
		sig.sa_handler = SIG_DFL;
	}
	else if (option == IGNORE)
	{
		sig.sa_handler = SIG_IGN;
	}
	else if (option == STOP)
	{
		sig.sa_handler = &setGlobalSignal;
	}
	else
	{
		std::cerr << "webserv: handleSignal: unknown option" << std::endl;
		return (FAILURE);
	}
	if (sigaction(signal, &sig, NULL) != 0)
	{
		std::cerr << "webserv: handleSignal: sigaction: " << strerror(errno) << std::endl;
		return (FAILURE);
	}
	return (SUCCESS);
}
