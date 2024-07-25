#include "sockets.hpp"

int	getExitStatus(int codeExit)
{
	int	exitStatus;

	if (WIFEXITED(codeExit))
	{
		exitStatus = WEXITSTATUS(codeExit);
	}
	else if (WIFSIGNALED(codeExit))
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