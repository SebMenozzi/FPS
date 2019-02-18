#include "clock.h"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <time.h>
#endif

Clock::Clock()
{
  start_time = this->getTicks();
}

void Clock::adjust(sint32 time)
{
  this->start_time = this->getTicks() - time;
}

sint32 Clock::getTime(void)
{
  return this->getTicks() - this->start_time;
}

sint32 Clock::getTicks(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000);
}
