#include <SDL2/SDL.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include "overlay.h"
#include "../configuration.h"

Overlay::Overlay(SDL_Window* fenetre)
{
  this->fenetre = fenetre;
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);
}

Overlay::~Overlay()
{

}

void Overlay::ajouter(WidgetOverlay* widget)
{
  this->listeWidgets.push_back(widget);
}

void Overlay::dessiner()
{
 // En 2D
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);

  // Definition de la fenetre
  gluOrtho2D(0.0, (GLdouble)this->largeurFenetre, 0.0, (GLdouble)this->hauteurFenetre);

  //?
  //glLoadIdentity();
  //glMatrixMode(GL_PROJECTION);

  // Desactivation du test de profondeur
  glDisable(GL_DEPTH_TEST);

  // Dessin des widgets
  for(uint32 i = 0; i < this->listeWidgets.size(); i++)
  {
    this->listeWidgets[i]->dessiner();
  }

  // Affichage du dessin
  //glFlush();
  //SDL_GL_SwapBuffers();
}
