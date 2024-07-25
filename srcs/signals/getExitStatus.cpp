#include "sockets.hpp"

int	getExitStatus(int codeExit)
{
	int	exitStatus;

	if (WIFSIGNALED(codeExit))
	{
		exitStatus = WEXITSTATUS(codeExit) + 128;
	}
	else if (WIFEXITED(codeExit))
	{
		exitStatus = WTERMSIG(codeExit);
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