#include "sockets.hpp"

int	getExitStatus(int codeExit) {
	if (WIFEXITED(codeExit)) {
		return WEXITSTATUS(codeExit);
	}
	else if (WIFSIGNALED(codeExit)) {
		return (WTERMSIG(codeExit) + 128);
	}
	else if (WIFSTOPPED(codeExit)) {
		return (WSTOPSIG(codeExit) + 128);
	}
	else {
		return codeExit;
	}
}