#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED
#include "types.h"

class Clock
{
  public:
    Clock();
    void adjust(sint32 heure);
    sint32 get_time(void);
  private:
  	sint32 get_ticks(void);

    sint32 start_time;
};

#endif
