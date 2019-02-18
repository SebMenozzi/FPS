#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED
#include "types.h"

class Clock
{
  public:
    Clock();
    void adjust(sint32 time);
    sint32 getTime(void);
    sint32 getTicks(void);
  private:
    sint32 start_time;
};

#endif
