#ifndef HORLOGE_H_INCLUDED
#define HORLOGE_H_INCLUDED
#include "types.h"

class Horloge
{
  public:
    Horloge();
    void regler(sint32 heure);
    sint32 heure(void);
  private:
    sint32 heureDepart;
};

#endif // HORLOGE_H_INCLUDED
