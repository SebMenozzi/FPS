#include <cmath>
#include "zoneTexte.h"
#include "../configuration.h"

TTF_Font* ZoneTexte::police = NULL;
uint32 ZoneTexte::compteurInstance = 0;

ZoneTexte::ZoneTexte(SDL_Window* fenetre, sint32 x, sint32 y) : Widget(x, y)
{
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);

  #define ZONE_TEXTE_AVEC_FOCUS "zone_texte_avec_focus.bmp"
  #define ZONE_TEXTE_SANS_FOCUS "zone_texte_sans_focus.bmp"

  this->conteneurTextures.ajouter(ZONE_TEXTE_AVEC_FOCUS);
  this->conteneurTextures.ajouter(ZONE_TEXTE_SANS_FOCUS);
  this->_largeur = this->conteneurTextures.texture(ZONE_TEXTE_SANS_FOCUS).largeur;
  this->_hauteur = this->conteneurTextures.texture(ZONE_TEXTE_SANS_FOCUS).hauteur;

  if(0 == this->compteurInstance)
  {
    this->police = TTF_OpenFont("Cartonsix NC.ttf", 35);
  }
  this->compteurInstance++;

  this->modifierTexte("", FALSE);
}

ZoneTexte::~ZoneTexte()
{
  this->conteneurTextures.supprimer(ZONE_TEXTE_AVEC_FOCUS);
  this->conteneurTextures.supprimer(ZONE_TEXTE_SANS_FOCUS);

  glDeleteTextures(1, &this->textureTexte);

  this->compteurInstance--;
  if(0 == this->compteurInstance)
  {
    // Liberation de la police
    TTF_CloseFont(this->police);
  }
}

void ZoneTexte::toucheEnfoncee(char touche)
{
  if (TRUE == this->_aLeFocus)
  {
    std::string nouveauTexte = this->_texte;

    if ('\b' == touche)
    {
      if(this->_texte.size() > 0)
      {
        nouveauTexte.resize(nouveauTexte.size() - 1);
      }
    }
    else
    {
      if (touche >= 0x20 /* espace */)
      {
        nouveauTexte += touche;
      }
    }

    this->modifierTexte(nouveauTexte, TRUE);
  }
}

void ZoneTexte::dessinDuWidget()
{
  if(TRUE == this->_aLeFocus)
  {
    // Selection de l'image de fond
    glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(ZONE_TEXTE_AVEC_FOCUS).texture);
  }
  else
  {
    // Selection de l'image de fond
    glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(ZONE_TEXTE_SANS_FOCUS).texture);
  }

  // Application de l'image de fond
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i( this->_positionX - (this->_largeur/2), this->hauteurFenetre - this->_positionY + (this->_hauteur/2));
    glTexCoord2i(0, 1); glVertex2i( this->_positionX - (this->_largeur/2), this->hauteurFenetre - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 1); glVertex2i( this->_positionX + (this->_largeur/2), this->hauteurFenetre - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 0); glVertex2i( this->_positionX + (this->_largeur/2), this->hauteurFenetre - this->_positionY + (this->_hauteur/2));
  glEnd();


  #define BORDURE 3

  // Si le texte deborde de la zone de texte
  if(this->largeurTexte + (2 * BORDURE) > this->_largeur)
  {
    float16 portionASupprimer = ((this->largeurTexte - ((float16)this->_largeur - 2 * BORDURE)) / this->largeurTexture);

     // Selection de la texture du texte
    glBindTexture( GL_TEXTURE_2D, this->textureTexte );

    // Application du texte
    glBegin(GL_QUADS);
      glTexCoord2f(portionASupprimer, 0); glVertex2i( this->_positionX - (this->_largeur/2) + BORDURE, this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
      glTexCoord2f(portionASupprimer, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->_largeur/2) + BORDURE, this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
      glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX + (this->_largeur/2) - BORDURE, this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
      glTexCoord2f(this->largeurPortionUtile, 0); glVertex2i( this->_positionX + (this->_largeur/2) - BORDURE, this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
    glEnd();
  }
  else
  {
    // Selection de la texture du texte
    glBindTexture( GL_TEXTURE_2D, this->textureTexte );

    // Application du texte
    glBegin(GL_QUADS);
      //glTexCoord2f(0.0, 0.0); glVertex2i( this->_positionX - (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY + (this->hauteurTexte/2));
      //glTexCoord2f(0.0, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY - (this->hauteurTexte/2));
      //glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX + (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY - (this->hauteurTexte/2));
      //glTexCoord2f(this->largeurPortionUtile, 0.0); glVertex2i( this->_positionX + (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY + (this->hauteurTexte/2));

      glTexCoord2f(0.0, 0.0); glVertex2i( this->_positionX - (this->_largeur/2) + BORDURE, this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
      glTexCoord2f(0.0, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->_largeur/2) + BORDURE, this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
      glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->_largeur/2) + (this->largeurTexte) + BORDURE, this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
      glTexCoord2f(this->largeurPortionUtile, 0.0); glVertex2i( this->_positionX - (this->_largeur/2) + (this->largeurTexte) + BORDURE, this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
    glEnd();
  }
}

void ZoneTexte::modifierTexte(std::string texte, bool8 curseur)
{
  this->_texte = texte;
  std::string texteAAfficher;
  if(curseur)
  {
    texteAAfficher = this->_texte + '|';
  }
  else
  {
    texteAAfficher = this->_texte;
  }

  if(0 == texteAAfficher.size())
  {
    texteAAfficher = " ";
  }

  // Creation de l'image du texte avec la police associee
  SDL_Color couleurTexte = { 50, 50, 50 };
  SDL_Surface* surfaceTexte = TTF_RenderText_Blended(this->police, texteAAfficher.c_str(), couleurTexte);

  // Activation de la transparence
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Remplacement de la texture
  glDeleteTextures(1, &this->textureTexte);
  glGenTextures(1, &this->textureTexte);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, this->textureTexte);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum codagePixel;
  if (surfaceTexte->format->Rmask == 0x000000ff)
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
  this->largeurTexte = surfaceTexte->w;
  this->hauteurTexte = surfaceTexte->h;
  this->largeurTexture = puissanceDeDeuxSuperieure(surfaceTexte->w);
  this->hauteurTexture = puissanceDeDeuxSuperieure(surfaceTexte->h);
  this->largeurPortionUtile = (float16)this->largeurTexte / this->largeurTexture;
  this->hauteurPortionUtile = (float16)this->hauteurTexte / this->hauteurTexture;

  // Chargement de la texture du texte precedament creee
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, surfaceTexte->w, surfaceTexte->h, 0, codagePixel, GL_UNSIGNED_BYTE, surfaceTexte->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->largeurTexture, this->hauteurTexture, 0, codagePixel, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->largeurTexte, this->hauteurTexte, codagePixel, GL_UNSIGNED_BYTE, surfaceTexte->pixels);

  // Liberation de l'image du texte
  SDL_FreeSurface(surfaceTexte);

  // L'apparance est modifiee, la zone de texte devra etre redessinee
  this->_apparenceModifiee = TRUE;
}

std::string ZoneTexte::texte()
{
  return this->_texte;
}

void ZoneTexte::prendLeFocus()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  if (TRUE == this->focussable())
  {
    this->_aLeFocus = TRUE;
    this->modifierTexte(this->_texte, TRUE); // Ajout du curseur
  }
}

void ZoneTexte::perdLeFocus()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  this->_aLeFocus = FALSE;

  this->modifierTexte(this->_texte, FALSE); // Retire le curseur
}
