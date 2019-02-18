#include "etiquetteOverlay.h"
#include "../configuration.h"

EtiquetteOverlay::EtiquetteOverlay(SDL_Window* fenetre, sint32 x, sint32 y, std::string texte, uint32 taille, uint8 r, uint8 v, uint8 b, std::string police) : WidgetOverlay (fenetre, x, y)
{
  // Police et taille utilisee
  this->police = TTF_OpenFont(police.c_str(), taille);

  // Coleur utilisee
  this->couleur.r = r;
  this->couleur.g = v;
  this->couleur.b = b;

  // Texte a afficher
  this->texte = texte;

  // Generation de la texture du texte a afficher avec les parametres indiques precedement
  this->regenererTexture();
}

EtiquetteOverlay::~EtiquetteOverlay()
{
  // Destruction de la texture
  glDeleteTextures(1, &this->textureTexte);
}

void EtiquetteOverlay::modifierTexte(std::string texte)
{
  // Texte a afficher
  this->texte = texte;

  this->regenererTexture();
}

void EtiquetteOverlay::modifierCouleur(uint8 r, uint8 v, uint8 b)
{
  // Coleur utilisee
  this->couleur.r = r;
  this->couleur.g = v;
  this->couleur.b = b;

  this->regenererTexture();
}

bool EtiquetteOverlay::modifierPoliceTaille(std::string police, uint32 taille)
{
  // Police et taille utilisee
  this->police = TTF_OpenFont(police.c_str(), taille);

  if (this->police == NULL)
  {
    return FALSE;
  }

  this->regenererTexture();

  return TRUE;
}

void EtiquetteOverlay::regenererTexture(void)
{
  // S'il n'y a pas de texte, on prend du blanc
  if (0 == this->texte.size())
  {
    this->texte = " ";
  }

  // Creation de l'image du texte avec la police associee
  SDL_Surface* texteEtiquette = TTF_RenderText_Blended(this->police, texte.c_str(), this->couleur);
  if (NULL == texteEtiquette)
  {
    printf("Impossible de creer le texte de l'etiquette");
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
}

void EtiquetteOverlay::dessiner()
{
  if (this->visible)
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
}
