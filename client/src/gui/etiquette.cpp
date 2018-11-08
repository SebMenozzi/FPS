#include <SDL2/SDL.h>
#include <iostream>
#include "etiquette.h"
#include "../configuration.h"

Etiquette::Etiquette(SDL_Window* fenetre, sint32 x, sint32 y, std::string texte, uint32 taille) : Widget (x, y)
{
  this->police = 0;
  this->textureTexte = 0;
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);

  this->police = TTF_OpenFont("Cartonsix NC.ttf", taille);
  if (NULL == this->police)
  {
    std::cerr << "Impossible de charger la police" << std::endl;
  }

  this->modifierTexte(texte);

  this->_focussable = FALSE;
}

Etiquette::~Etiquette()
{
  glDeleteTextures(1, &this->textureTexte);
  TTF_CloseFont(this->police);
}

void Etiquette::dessinDuWidget()
{
  // Selection de la texture du texte
  glBindTexture( GL_TEXTURE_2D, this->textureTexte );

  // Application du texte
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2i( this->_positionX - (this->largeurTexte/2), this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
    glTexCoord2f(0.0, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->largeurTexte/2), this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
    glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX + (this->largeurTexte/2), this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
    glTexCoord2f(this->largeurPortionUtile, 0.0); glVertex2i( this->_positionX + (this->largeurTexte/2), this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
  glEnd();
}

void Etiquette::modifierTexte(std::string texte)
{
  // S'il n'y a pas de texte, on prend du blanc
  if (0 == texte.size())
  {
    texte = " ";
  }

  // Creation de l'image du texte avec la police associee
  SDL_Color couleurTexte = {50, 50, 50};
  SDL_Surface* texteEtiquette = TTF_RenderText_Blended(this->police, texte.c_str(), couleurTexte);
  if (NULL == texteEtiquette)
  {
    printf("Impossible de cr�er le texte de l'etiquette");
    return;
  }

  // Activation de la transparence
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Regeneration de la texture du texte
  glDeleteTextures(1, &this->textureTexte);
  glGenTextures(1, &this->textureTexte);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, this->textureTexte);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum codagePixel;
  if (texteEtiquette->format->Rmask == 0x000000ff)
  {
    codagePixel = GL_RGBA;
  }
  else
  {
    #ifndef GL_BGRA
      #define GL_BGRA 0x80E1
    #endif
    codagePixel = GL_BGRA;
  }

  //Recuperation des dimentions du texte
  this->largeurTexte = texteEtiquette->w;
  this->hauteurTexte = texteEtiquette->h;

  this->largeurTexture = puissanceDeDeuxSuperieure(texteEtiquette->w);
  this->hauteurTexture = puissanceDeDeuxSuperieure(texteEtiquette->h);
  this->largeurPortionUtile = (float16)this->largeurTexte / this->largeurTexture;
  this->hauteurPortionUtile = (float16)this->hauteurTexte / this->hauteurTexture;

  // Chargement de la texture du texte precedament creee
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->largeurTexture, this->hauteurTexture, 0, codagePixel, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->largeurTexte, this->hauteurTexte, codagePixel, GL_UNSIGNED_BYTE, texteEtiquette->pixels);
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, texteEtiquette->w, texteEtiquette->h, 0, codagePixel, GL_UNSIGNED_BYTE, texteEtiquette->pixels);

  // Liberation de l'image du texte du bouton
  SDL_FreeSurface(texteEtiquette);

  // L'apparence etant modifiee, l'etiquette devra etre redessinee
  this->_apparenceModifiee = TRUE;
}
