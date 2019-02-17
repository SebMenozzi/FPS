#include "clock.h"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <time.h>
#endif

Clock::Clock()
{
  start_time = this->get_ticks();
}

void Clock::adjust(sint32 heure)
{
  this->start_time = this->get_ticks() - heure;
}

sint32 Clock::get_time(void)
{
  return this->get_ticks() - this->start_time;
}

sint32 Clock::get_ticks(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000);
}
