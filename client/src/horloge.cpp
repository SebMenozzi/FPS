#include "horloge.h"
#include <SDL2/SDL.h>

Horloge::Horloge()
{
  heureDepart = SDL_GetTicks();
}

void Horloge::regler(sint32 heure)
{
  this->heureDepart = SDL_GetTicks() - heure;
}

sint32 Horloge::heure(void)
{
  return SDL_GetTicks() - this->heureDepart;
}
