#include "utils_so2_tp.h"

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max) {
	return (rand() % (max - min)) + min;
}
