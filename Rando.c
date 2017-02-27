#include <time.h>
#include <stdlib.h>

int _rando_init = 0;
void rando_init()
{
	if (!_rando_init)
	{
		srand(clock());
		_rando_init = 1;
	}
}

/* 
    Returns low <= num <= high on success
    Returns -1 on error
*/
int rando_me(int low, int high)
{
	if ((high - low) == 0)
	{
		return -1;			// Div by 0 error
	}
	else if (high == low)
	{
		return -1;
	}
	else if (low > high)
	{
		return -1;			// Input error
	}
	//initialize the random number generator
	rando_init();

//	int randoNum = 72;  // DEBUGGING
	int randoNum = (rand() % (high - low)) + low;

	return randoNum;
}
