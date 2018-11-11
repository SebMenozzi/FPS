#include "horloge.h"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <time.h>
#endif

Horloge::Horloge()
{
  heureDepart = this->getTicks();
}

void Horloge::regler(sint32 heure)
{
  this->heureDepart = this->getTicks() - heure;
}

sint32 Horloge::heure(void)
{
  return this->getTicks() - this->heureDepart;
}

sint32 Horloge::getTicks(void)
{
  #ifdef _WIN32
    DWORD time;
  	time = timeGetTime(); // Linker : -lwinmm
  	return time;
  #else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000);
  #endif
}
