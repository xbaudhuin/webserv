#include "sockets.hpp"

int	getExitStatus(int codeExit)
{
	int	exitStatus;

	if (WIFSIGNALED(codeExit))
	{
		exitStatus = WEXITSTATUS(codeExit);
	}
	else if (WIFEXITED(codeExit))
	{
		exitStatus = WTERMSIG(codeExit) + 128;
	}
	else if (WIFSTOPPED(codeExit))
	{
		exitStatus = WSTOPSIG(codeExit) + 128;
	}
	else
	{
		exitStatus = codeExit;
	}
	return (exitStatus);
}