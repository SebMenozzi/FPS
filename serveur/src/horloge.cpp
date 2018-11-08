#include "horloge.h"

Horloge::Horloge()
{
    heureDepart = 0;
}

void Horloge::regler(sint32 heure)
{
    this->heureDepart = 0 - heure;
}

sint32 Horloge::heure(void)
{
    return 0 - this->heureDepart;
}
