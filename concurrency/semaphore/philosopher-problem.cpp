#include <cstdio>
#include <unistd.h>
#ifdef __APPLE__
#include "zemaphore.h"
#elif __linux__
#include <semaphore.h>
#endif
