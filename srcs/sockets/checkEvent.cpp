#include "sockets.hpp"

bool	checkEvent(uint32_t events, uint32_t eventToCheck)
{
	if ((events & eventToCheck) != 0) {
		return true;
	}
	else {
		return false;
	}
}