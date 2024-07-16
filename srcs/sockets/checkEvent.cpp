#include "Webserv.hpp"

/* Return true if eventToCheck bit mask is present in events */
bool	checkEvent(uint32_t events, uint32_t eventToCheck)
{
	if ((events & eventToCheck) != 0)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}