#include <cmath>
#include "widgetOverlay.h"

WidgetOverlay::WidgetOverlay(SDL_Window* fenetre, sint32 x, sint32 y)
{
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);

  this->_positionX = x;
  this->_positionY = y;

  this->visible = TRUE;
}

WidgetOverlay::~WidgetOverlay()
{

}

uint32 WidgetOverlay::puissanceDeDeuxSuperieure(uint32 i)
{
  double32 logbase2 = log(i) / log(2);
  return (uint32)round(pow(2.0, ceil(logbase2)));
}

void WidgetOverlay::rendreVisible(void)
{
  this->visible = TRUE;
}

void WidgetOverlay::rendreInvisible(void)
{
  this->visible = FALSE;
}
