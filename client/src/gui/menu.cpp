#include <sstream>
#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "menu.h"
#include "../configuration.h"

Menu::Menu(SDL_Window* window, std::string backgroundImage)
{
  this->backgroundImage = backgroundImage;
  this->texturesContainer.ajouter(this->backgroundImage);
  this->stateChanged = TRUE;
  this->window = window;
  this->windowWidth = 0;
  this->windowHeight = 0;

  glDisable(GL_DEPTH_TEST);

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(window, &this->windowWidth, &this->windowHeight);

  // Activation de la "traduction" touche->lettre
  //SDL_EnableUNICODE(1);

  // Activation de la repetition de touches
  //SDL_EnableKeyRepeat(300, 50);
}

Menu::~Menu()
{
  this->texturesContainer.supprimer(this->backgroundImage);
}

void Menu::draw(bool8 force)
{
  // Si l'utilisateur demande au menu d'etre entierement redessine
  if (TRUE == force) {
    // Force le menu a se redessiner en simulant une modification
    SDL_Event event;
    event.type = SDL_WINDOWEVENT;
    SDL_PushEvent(&event);

    // Envoi des informations sur l'etat de la souris
    SDL_Event cursorEvent;
    cursorEvent.type = SDL_MOUSEMOTION;
    int cursorX, cursorY;
    cursorEvent.motion.state = SDL_GetMouseState(&cursorX, &cursorY);
    cursorEvent.motion.x = cursorX;
    cursorEvent.motion.y = cursorY;
    cursorEvent.motion.xrel = 0;
    cursorEvent.motion.yrel = 0;
    SDL_PushEvent(&cursorEvent);
  }

  this->animate();

  // Si le menu a ete graphiquement modifie
  if(TRUE == this->change()) {
    // Dessin du menu

    // Activation des textures
    glEnable(GL_TEXTURE_2D);

    // Desactivation du test de profondeur
    glDisable(GL_DEPTH_TEST);

    // Vidage de l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // En 2D
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);

    // Definition de la fenetre
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble) this->windowWidth, 0.0, (GLdouble) this->windowHeight);

    /*

    // Desactivation du test de profondeur
    glDisable(GL_DEPTH_TEST);

    // Vidage de l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // En 2D
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);

    // Definition de la fenetre
    gluOrtho2D(0.0, (GLdouble)LARGEUR_FENETRE, 0.0, (GLdouble)HAUTEUR_FENETRE);
    */

    // Dessin de fond du menu
    this->drawBackground();

    // Dessin des widgets
    for(uint32 i = 0; i < this->widgetsList.size(); ++i) {
      this->widgetsList[i]->draw();
    }

    // Affichage du dessin
    glFlush();
    SDL_GL_SwapWindow(this->window);
  }
}

void Menu::add(Widget* widget)
{
  this->widgetsList.push_back(widget);
}

bool8 Menu::change()
{
  for(uint32 i = 0; i < this->widgetsList.size(); ++i) {
    if(this->widgetsList[i]->getStateChanged()) {
      this->stateChanged = TRUE;
    }
  }

  bool8 modifie = this->stateChanged;
  this->stateChanged = FALSE;
  return modifie;
}

void Menu::animate()
{
  // For each event
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    // Distribution de l'evenement aux widgets
    this->distributeEventToWidgets(event);

    // Si la fenetre est alteree par quelquechose d'exterieur au programme
    if(SDL_WINDOWEVENT == event.type) {
      // L'apparence est modifiee
      this->stateChanged = TRUE;
      // Le menu sera redessine
    }

    // Si la touche tabulation s'enfonce
    if(SDL_KEYDOWN == event.type && SDLK_TAB == event.key.keysym.sym) {
      // Passage du focus au widget suivant
      this->focusToNext();
    }
  }

  // Recherche d'une demande de focus
  uint32 focus = (uint32)-1;
  for(uint32 i = 0 ; i < this->widgetsList.size(); ++i) {
    if(this->widgetsList[i]->getAskFocus()) {
      focus = i;
    }
  }

  // Si un widget a demande le focus
  if(((uint32) -1) != focus) {
    // Attribution du focus
    for(uint32 i = 0 ; i < this->widgetsList.size(); ++i) {
      if(focus == i) {
        this->widgetsList[i]->focus();
      } else {
        this->widgetsList[i]->blur();
      }
    }
  }
}

void Menu::distributeEventToWidgets(SDL_Event event)
{
  // Pour chaque widgets
  for(uint32 i = 0; i < this->widgetsList.size(); ++i) {
    // Distribution de l'evenement
    switch (event.type) {
      // Souris deplacee
      case SDL_MOUSEMOTION:
        {
          sint32 x, y;
          SDL_GetMouseState((int*) &x, (int*) &y);
          this->widgetsList[i]->cursorMoved(x, y);
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        {
          // Clique gauche enfonce
          if (event.button.button == SDL_BUTTON_LEFT) {
            this->widgetsList[i]->leftClickPressed();
          }
        }
        break;

      case SDL_MOUSEBUTTONUP:
        {
          // Clique gauche relache
          if (event.button.button == SDL_BUTTON_LEFT)  {
            this->widgetsList[i]->leftClickReleased();
          }
        }
        break;

      // Touche enfoncee
      case SDL_KEYDOWN:
        {
          this->widgetsList[i]->keyPressed((char) event.key.keysym.sym);
        }
        break;

      // Touche relachee
      case SDL_KEYUP:
        {
          this->widgetsList[i]->keyReleased((char) event.key.keysym.sym);
        }
        break;
    }
  }
}

void Menu::focusToNext()
{
  // Recherche du pocesseur du focus
  uint32 focusedWidget;
  for(focusedWidget = 0; focusedWidget < this->widgetsList.size(); ++focusedWidget) {
    if(this->widgetsList[focusedWidget]->isFocused()) {
      break;
    }
  }

  // Recherche du prochain pocesseur du focus
  uint32 nextFocusedWidget;
  for(nextFocusedWidget = nextFocusedWidget + 1; nextFocusedWidget < this->widgetsList.size(); ++nextFocusedWidget) {
    if(this->widgetsList[nextFocusedWidget]->isFocusable()) {
      break;
    }
  }

  // Si un widget a le focus
  if (focusedWidget < this->widgetsList.size()) {
    // Retire le focus de l'ancien Widget
    this->widgetsList[focusedWidget]->blur();

    // Si le prochain pocesseur du focus existe
    if (nextFocusedWidget < this->widgetsList.size()) {
      // Donne le focus au nouveau widget
      this->widgetsList[nextFocusedWidget]->focus();
    } else { // Si le focus ne peut pas etre donne au widget suivant
      // Recherche du premier widget focussable
      uint32 firstWidgetFocusable;
      for(firstWidgetFocusable = 0; firstWidgetFocusable < this->widgetsList.size(); ++firstWidgetFocusable) {
        if(this->widgetsList[firstWidgetFocusable]->isFocusable()) {
          break;
        }
      }

      // Donne le focus au premier widget focusssable
      this->widgetsList[firstWidgetFocusable]->focus();
    }
  }
}

void Menu::drawBackground(void)
{
  // Selection de l'image de fond
  glBindTexture(GL_TEXTURE_2D, this->texturesContainer.texture(this->backgroundImage).texture);

  // Application de l'image de fond
  glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex2f(0, this->windowHeight);

    glTexCoord2d(0, 1);
    glVertex2f(0, 0);

    glTexCoord2d(1, 1);
    glVertex2f(this->windowWidth, 0);

    glTexCoord2d(1, 0);
    glVertex2f(this->windowWidth, this->windowHeight);
  glEnd();
}
