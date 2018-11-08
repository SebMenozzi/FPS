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

Menu::Menu(SDL_Window* fenetre, std::string imageFond)
{
  this->imageFond = imageFond;
  this->conteneurTextures.ajouter(this->imageFond);
  this->apparenceModifiee = TRUE;
  this->fenetre = fenetre;
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;

  glDisable(GL_DEPTH_TEST);

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);

  // Activation de la "traduction" touche->lettre
  //SDL_EnableUNICODE(1);

  // Activation de la repetition de touches
  //SDL_EnableKeyRepeat(300, 50);
}

Menu::~Menu()
{
  this->conteneurTextures.supprimer(this->imageFond);
}

void Menu::dessiner(bool8 force)
{
  // Si l'utilisateur demande au menu d'etre entierement redessine
  if (TRUE == force)
  {
    // Force le menu a se redessiner en simulant une modification
    SDL_Event evenement;
    evenement.type = SDL_WINDOWEVENT;
    SDL_PushEvent(&evenement);

    // Envoi des informations sur l'etat de la souris
    SDL_Event evenementSouris;
    evenementSouris.type = SDL_MOUSEMOTION;
    int xSouris, ySouris;
    evenementSouris.motion.state = SDL_GetMouseState(&xSouris,&ySouris);
    evenementSouris.motion.x = xSouris;
    evenementSouris.motion.y = ySouris;
    evenementSouris.motion.xrel = 0;
    evenementSouris.motion.yrel = 0;
    SDL_PushEvent(&evenementSouris);
  }

  this->animer();

  // Si le menu a ete graphiquement modifie
  if(TRUE == this->modifie())
  {
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
    gluOrtho2D(0.0, (GLdouble)this->largeurFenetre, 0.0, (GLdouble)this->hauteurFenetre);

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
    this->dessinerFond();

    // Dessin des widgets
    for(uint32 i = 0; i < this->listeWidgets.size(); i++)
    {
      this->listeWidgets[i]->dessiner();
    }

    // Affichage du dessin
    glFlush();
    SDL_GL_SwapWindow(this->fenetre);
  }
}

void Menu::ajouter(Widget* widget)
{
  this->listeWidgets.push_back(widget);
}

bool8 Menu::modifie()
{
  for(uint32 i = 0; i < this->listeWidgets.size(); i++)
  {
    if(this->listeWidgets[i]->apparenceModifiee())
    {
      this->apparenceModifiee = TRUE;
    }
  }

  bool8 modifie = this->apparenceModifiee;
  this->apparenceModifiee = FALSE;
  return modifie;
}

void Menu::animer()
{
  // Pour chaque evenement
  SDL_Event evenement;
  while(SDL_PollEvent(&evenement))
  {
    // Distribution de l'evenement aux widgets
    this->distribuerEvenementAuxWidgets(evenement);

    // Si la fenetre est alteree par quelquechose d'exterieur au programme
    if(SDL_WINDOWEVENT == evenement.type)
    {
      // L'apparence est modifiee
      this->apparenceModifiee = TRUE;
      // Le menu sera redessine
    }

    // Si la touche tabulation s'enfonce
    if(SDL_KEYDOWN == evenement.type && SDLK_TAB == evenement.key.keysym.sym)
    {
      // Passage du focus au widget suivant
      this->passerLeFocusAuWidgetSuivant();
    }
  }

  // Recherche d'une demande de focus
  uint32 focus = (uint32)-1;
  for(uint32 i = 0 ; i < this->listeWidgets.size() ; i++)
  {
    if(this->listeWidgets[i]->demandeLeFocus())
    {
      focus = i;
    }
  }

  // Si un widget a demande le focus
  if( ((uint32)-1) != focus)
  {
    // Attribution du focus
    for(uint32 i = 0 ; i < this->listeWidgets.size() ; i++)
    {
      if(focus == i)
      {
        this->listeWidgets[i]->prendLeFocus();
      }
      else
      {
        this->listeWidgets[i]->perdLeFocus();
      }
    }
  }
}

void Menu::distribuerEvenementAuxWidgets(SDL_Event evenement)
{
  // Pour chaque widgets
  for(uint32 i = 0; i < this->listeWidgets.size(); i++)
  {
    // Distribution de l'evenement
    switch (evenement.type)
    {
      // Souris deplacee
      case SDL_MOUSEMOTION:
        {
          sint32 x, y;
          SDL_GetMouseState((int*) &x, (int*) &y);
          this->listeWidgets[i]->sourisDeplacee(x, y);
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        {
          // Clique gauche enfonce
          if (evenement.button.button == SDL_BUTTON_LEFT)
          {
            this->listeWidgets[i]->cliqueGaucheEnfonce();
          }
        }
        break;

      case SDL_MOUSEBUTTONUP:
        {
          // Clique gauche relache
          if (evenement.button.button == SDL_BUTTON_LEFT)
          {
            this->listeWidgets[i]->cliqueGaucheRelache();
          }
        }
        break;

      // Touche enfoncee
      case SDL_KEYDOWN:
        {
          this->listeWidgets[i]->toucheEnfoncee((char)evenement.key.keysym.sym);
        }
        break;

      // Touche relachee
      case SDL_KEYUP:
        {
          this->listeWidgets[i]->toucheRelachee((char)evenement.key.keysym.sym);
        }
        break;
    }
  }
}

void Menu::passerLeFocusAuWidgetSuivant()
{
  // Recherche du pocesseur du focus
  uint32 pocesseurFocus;
  for(pocesseurFocus = 0; pocesseurFocus < this->listeWidgets.size(); pocesseurFocus++)
  {
    if(this->listeWidgets[pocesseurFocus]->aLeFocus())
    {
      break;
    }
  }

  // Recherche du prochain pocesseur du focus
  uint32 prochainPocesseurFocus;
  for(prochainPocesseurFocus = pocesseurFocus + 1; prochainPocesseurFocus < this->listeWidgets.size(); prochainPocesseurFocus++)
  {
    if(this->listeWidgets[prochainPocesseurFocus]->focussable())
    {
      break;
    }
  }

  // Si un widget a le focus
  if (pocesseurFocus < this->listeWidgets.size())
  {
    // Retire le focus de l'ancien Widget
    this->listeWidgets[pocesseurFocus]->perdLeFocus();

    // Si le prochain pocesseur du focus existe
    if (prochainPocesseurFocus < this->listeWidgets.size())
    {
      // Donne le focus au nouveau widget
      this->listeWidgets[prochainPocesseurFocus]->prendLeFocus();
    }

    // Si le focus ne peut pas �tre donne au widget suivant
    else
    {
      // Recherche du premier widget focussable
      uint32 premierWidgetFocussable;
      for(premierWidgetFocussable = 0; premierWidgetFocussable < this->listeWidgets.size(); premierWidgetFocussable++)
      {
        if(this->listeWidgets[premierWidgetFocussable]->focussable())
        {
          break;
        }
      }

      // Donne le focus au premier widget focusssable
      this->listeWidgets[premierWidgetFocussable]->prendLeFocus();
    }
  }
}

void Menu::dessinerFond(void)
{
  // Selection de l'image de fond
  glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(this->imageFond).texture);

  // Application de l'image de fond
  glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex2f(0, this->hauteurFenetre);
    glTexCoord2d(0, 1); glVertex2f(0, 0);
    glTexCoord2d(1, 1); glVertex2f(this->largeurFenetre, 0);
    glTexCoord2d(1, 0); glVertex2f(this->largeurFenetre, this->hauteurFenetre);
  glEnd();
}
