#include "bouton.h"
#include "../configuration.h"

TTF_Font* Bouton::police = NULL;
uint32 Bouton::compteurInstances = 0;

Bouton::Bouton(SDL_Window* fenetre, sint32 x, sint32 y, std::string texte) : Widget(x, y)
{
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);

  #define BOUTON_PAS_SURVOLE_SANS_FOCUS "bouton_pas_survole_sans_focus.bmp"
  #define BOUTON_PAS_SURVOLE_AVEC_FOCUS "bouton_pas_survole_avec_focus.bmp"
  #define BOUTON_SURVOLE_SANS_FOCUS "bouton_survole_sans_focus.bmp"
  #define BOUTON_SURVOLE_AVEC_FOCUS "bouton_survole_avec_focus.bmp"
  #define BOUTON_ENFONCE "bouton_enfonce.bmp"
  this->conteneurTextures.ajouter(BOUTON_PAS_SURVOLE_SANS_FOCUS);
  this->conteneurTextures.ajouter(BOUTON_PAS_SURVOLE_AVEC_FOCUS);
  this->conteneurTextures.ajouter(BOUTON_SURVOLE_SANS_FOCUS);
  this->conteneurTextures.ajouter(BOUTON_SURVOLE_AVEC_FOCUS);
  this->conteneurTextures.ajouter(BOUTON_ENFONCE);
  this->_largeur = this->conteneurTextures.texture(BOUTON_ENFONCE).largeur;
  this->_hauteur = this->conteneurTextures.texture(BOUTON_ENFONCE).hauteur;

  if(0 == this->compteurInstances)
  {
      this->police = TTF_OpenFont("Cartonsix NC.ttf", 35);
  }
  this->compteurInstances++;

  this->textureTexte = 0;

  // Creation de l'image du texte avec la police associee
  this->texte(texte);
}

Bouton::~Bouton()
{
  this->conteneurTextures.supprimer(BOUTON_PAS_SURVOLE_SANS_FOCUS);
  this->conteneurTextures.supprimer(BOUTON_PAS_SURVOLE_AVEC_FOCUS);
  this->conteneurTextures.supprimer(BOUTON_SURVOLE_SANS_FOCUS);
  this->conteneurTextures.supprimer(BOUTON_SURVOLE_AVEC_FOCUS);
  this->conteneurTextures.supprimer(BOUTON_ENFONCE);

  glDeleteTextures(1, &this->textureTexte);

  this->compteurInstances--;
  if(0 == this->compteurInstances)
  {
    // Liberation de la police
    TTF_CloseFont(this->police);
  }
}

void Bouton::dessinDuWidget()
{
  // Selection de l'image de fond
  switch (this->_etat)
  {
    case SURVOLE_ET_ENFONCE:
      {
        glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(BOUTON_ENFONCE).texture);
      }
      break;

    case SURVOLE:
      {
        if(TRUE == this->_aLeFocus)
        {
          glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(BOUTON_SURVOLE_AVEC_FOCUS).texture);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(BOUTON_SURVOLE_SANS_FOCUS).texture);
        }
      }
        break;

    case PAS_SURVOLE:
      {
        if(TRUE == this->_aLeFocus)
        {
          glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(BOUTON_PAS_SURVOLE_AVEC_FOCUS).texture);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(BOUTON_PAS_SURVOLE_SANS_FOCUS).texture);
        }
      }
      break;
  }

  // Application de l'image de fond
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i( this->_positionX - (this->_largeur/2), this->hauteurFenetre - this->_positionY + (this->_hauteur/2));
    glTexCoord2i(0, 1); glVertex2i( this->_positionX - (this->_largeur/2), this->hauteurFenetre - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 1); glVertex2i( this->_positionX + (this->_largeur/2), this->hauteurFenetre - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 0); glVertex2i( this->_positionX + (this->_largeur/2), this->hauteurFenetre - this->_positionY + (this->_hauteur/2));
  glEnd();

  // Selection de la texture du texte
  glBindTexture(GL_TEXTURE_2D, this->textureTexte);

  // Application du texte
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2i( this->_positionX - (this->largeurTexte/2), this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
    glTexCoord2f(0.0, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->largeurTexte/2), this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
    glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX + (this->largeurTexte/2), this->hauteurFenetre - this->_positionY - (this->hauteurTexte/2));
    glTexCoord2f(this->largeurPortionUtile, 0.0); glVertex2i( this->_positionX + (this->largeurTexte/2), this->hauteurFenetre - this->_positionY + (this->hauteurTexte/2));
  glEnd();
}

void Bouton::toucheEnfoncee(char c)
{
  // Si le bouton a le focus et que la touche "Entree" est enfoncee
  if (TRUE == this->_aLeFocus && '\r' == c)
  {
    // Le bouton est clique
    this->_clique = TRUE;
  }
}

std::string Bouton::texte()
{
  return this->_texte;
}

void Bouton::texte(std::string texte)
{
  this->_texte = texte;

  // Destruction de la texture potentiellement existante
  glDeleteTextures(1, &this->textureTexte);

  // Creation de l'image du texte avec la police associee
  SDL_Color couleurTexte = {50, 50, 50};
  SDL_Surface* texteBouton = TTF_RenderText_Blended(this->police, texte.c_str(), couleurTexte);
  if (NULL == texteBouton)
  {
    printf("Impossible de cr�er le texte du bouton");
  }

  // Activation de la transparence
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Generation de la texture pour le texte du bouton
  glGenTextures(1, &this->textureTexte);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, this->textureTexte);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum codagePixel;
  if (texteBouton->format->Rmask == 0x000000ff)
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
  this->largeurTexte = texteBouton->w;
  this->hauteurTexte = texteBouton->h;
  this->largeurTexture = puissanceDeDeuxSuperieure(texteBouton->w);
  this->hauteurTexture = puissanceDeDeuxSuperieure(texteBouton->h);
  this->largeurPortionUtile = (float16)this->largeurTexte / this->largeurTexture;
  this->hauteurPortionUtile = (float16)this->hauteurTexte / this->hauteurTexture;

  // Chargement de la texture du texte precedament creee
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, texteBouton->w, texteBouton->h, 0,
  //              codagePixel, GL_UNSIGNED_BYTE, texteBouton->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->largeurTexture, this->hauteurTexture, 0, codagePixel, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texteBouton->w, texteBouton->h, codagePixel, GL_UNSIGNED_BYTE, texteBouton->pixels);

  //Recuperation des dimentions du texte
  this->largeurTexte = texteBouton->w;
  this->hauteurTexte = texteBouton->h;

  // Liberation de l'image du texte du bouton
  SDL_FreeSurface(texteBouton);

  // L'apparance a ete modifiee, le bouton devra etre redessine
  _apparenceModifiee = TRUE;
}
