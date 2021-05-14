#include "rand.h"
int _time_seed = 0;

void srand(int seed) {
    _time_seed = seed;
}

int rand() {
	_time_seed = _time_seed * 1103515245 + 12345;
	return (unsigned int)(_time_seed / 65536) % rand_max; 
}