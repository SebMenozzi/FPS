#include "champSaisieNumerique.h"
#include "../configuration.h"
#include <sstream>

TTF_Font* ChampSaisieNumerique::police = NULL;
uint32 ChampSaisieNumerique::compteurInstances = 0;

ChampSaisieNumerique::ChampSaisieNumerique(sint32 x, sint32 y, sint32 valeur) : Widget(x, y)
{
  #define CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS "champ_saisie_numerique_sans_focus.bmp"
  #define CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS "champ_saisie_numerique_avec_focus.bmp"
  #define CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE "champ_saisie_numerique_bouton_bas_enfonce.bmp"
  #define CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE "champ_saisie_numerique_bouton_haut_enfonce.bmp"
  this->conteneurTextures.ajouter(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS);
  this->conteneurTextures.ajouter(CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS);
  this->conteneurTextures.ajouter(CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE);
  this->conteneurTextures.ajouter(CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE);
  this->_largeur = this->conteneurTextures.texture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS).largeur;
  this->_hauteur = this->conteneurTextures.texture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS).hauteur;

  // Valeur min et max acceptable par defaut
  this->_min = 0;
  this->_max = 100;

  if(0 == this->compteurInstances)
  {
      this->police = TTF_OpenFont("Cartonsix NC.ttf", 35);
  }
  this->compteurInstances++;

  // Creation de l'image du texte avec la police associee
  this->modifierValeur(valeur);
}

ChampSaisieNumerique::~ChampSaisieNumerique()
{
  this->conteneurTextures.supprimer(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS);
  this->conteneurTextures.supprimer(CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS);
  this->conteneurTextures.supprimer(CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE);
  this->conteneurTextures.supprimer(CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE);

  glDeleteTextures(1, &this->textureTexte);

  this->compteurInstances--;
  if(0 == this->compteurInstances)
  {
    // Liberation de la police
    TTF_CloseFont(this->police);
  }
}

void ChampSaisieNumerique::toucheEnfoncee(char c)
{
  // Si le champ de saisie numerique a le focus
  if (TRUE == this->_aLeFocus)
  {
    // ... et que la touche "+" est enfoncee
    if ('+' == c)
    {
      // Modification de la valeur
      this->modifierValeur(this->_valeur + 1, TRUE);
    }

    // La touche "-" est enfoncee
    if ('-' == c)
    {
      // Modification de la valeur
      this->modifierValeur(this->_valeur - 1, TRUE);
    }

    // La touche d'un chiffre
    if ('0' <= c || '9' >= c)
    {
      // Ajout du chiffre au nombre actuel

      std::stringstream texte;
      texte << this->_valeur;
      texte << c;

      sint32 valeur;
      texte >> valeur;
      this->modifierValeur(valeur, TRUE);
    }

    // Touche "effacer"
    if ('\b' == c)
    {
      // Revient a diviser par 10
      this->modifierValeur(this->_valeur / 10, TRUE);
    }
  }
}

void ChampSaisieNumerique::modifierValeur(sint32 valeur, bool8 curseur)
{
  // Assure que la valeur soit dans l'intervalle accepte
  if (this->_min > valeur)
  {
    valeur = this->_min;
  }
  else if (this->_max < valeur)
  {
    valeur = this->_max;
  }

  this->_valeur = valeur;
  std::stringstream texteAAfficher;
  if(curseur)
  {
    texteAAfficher << this->_valeur << '|';
  }
  else
  {
    texteAAfficher << this->_valeur;
  }

  if(0 == texteAAfficher.str().size())
  {
    texteAAfficher << " ";
  }

  // Creation de l'image du texte avec la police associee
  SDL_Color couleurTexte = {50, 50, 50};
  SDL_Surface* surfaceTexte = TTF_RenderText_Blended(this->police, texteAAfficher.str().c_str(), couleurTexte);

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
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, surfaceTexte->w, surfaceTexte->h, 0,
  //              codagePixel, GL_UNSIGNED_BYTE, surfaceTexte->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->largeurTexture, this->hauteurTexture, 0, codagePixel, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->largeurTexte, this->hauteurTexte, codagePixel, GL_UNSIGNED_BYTE, surfaceTexte->pixels);

  // Liberation de l'image du texte
  SDL_FreeSurface(surfaceTexte);

  // L'apparance est modifiee, la zone de texte devra etre redessinee
  this->_apparenceModifiee = TRUE;
}

sint32 ChampSaisieNumerique::valeur()
{
  return this->_valeur;
}

void ChampSaisieNumerique::prendLeFocus()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  if (TRUE == this->focussable())
  {
    this->_aLeFocus = TRUE;
    this->modifierValeur(this->_valeur, TRUE); // Ajout du curseur
  }
}

void ChampSaisieNumerique::perdLeFocus()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  this->_aLeFocus = FALSE;

  this->modifierValeur(this->_valeur, FALSE); // Retire le curseur
}

void ChampSaisieNumerique::dessinDuWidget()
{
  // Selection de l'image de fond
  if(TRUE == this->_aLeFocus)
  {
    // Fleche du haut survolee et enfoncee
    if(this->flecheHautSurvolee() && SURVOLE_ET_ENFONCE == this->_etat)
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE).texture);
    }
    // Fleche du bas survolee et enfoncee
    else if (this->flecheBasSurvolee() && SURVOLE_ET_ENFONCE == this->_etat)
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE).texture);
    }
    // Simplement en pocession du focus
    else
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS).texture);
    }
  }
  // Sans focus
  else
  {
    glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS).texture);
  }

  // Application de l'image de fond
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i( this->_positionX - (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY + (this->_hauteur/2));
    glTexCoord2i(0, 1); glVertex2i( this->_positionX - (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 1); glVertex2i( this->_positionX + (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 0); glVertex2i( this->_positionX + (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY + (this->_hauteur/2));
  glEnd();

  // Selection de la texture du texte
  glBindTexture(GL_TEXTURE_2D, this->textureTexte);

  // Application du texte
  glBegin(GL_QUADS);
    #define BORDURE 8
    glTexCoord2f(0.0, 0.0); glVertex2i( this->_positionX - (this->_largeur/2) + BORDURE, HAUTEUR_FENETRE - this->_positionY + (this->hauteurTexte/2));
    glTexCoord2f(0.0, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->_largeur/2) + BORDURE, HAUTEUR_FENETRE - this->_positionY - (this->hauteurTexte/2));
    glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->_largeur/2) + (this->largeurTexte) + BORDURE, HAUTEUR_FENETRE - this->_positionY - (this->hauteurTexte/2));
    glTexCoord2f(this->largeurPortionUtile, 0.0); glVertex2i( this->_positionX - (this->_largeur/2) + (this->largeurTexte) + BORDURE, HAUTEUR_FENETRE - this->_positionY + (this->hauteurTexte/2));
  glEnd();
}

bool8 ChampSaisieNumerique::flecheHautSurvolee()
{
  // Si une fleche est survolee
  if (this->_sourisX > this->_positionX + (this->_largeur/2) - 32)
  {
    // Si la fleche du haut est survolee
    if ((sint32)this->_sourisY < this->_positionY)
    {
      return TRUE;
    }
  }

  return FALSE;
}

bool8 ChampSaisieNumerique::flecheBasSurvolee()
{
  // Si une fleche est survolee
  if (this->_sourisX > this->_positionX + (this->_largeur/2) - 32)
  {
    // Si la fleche du bas est survolee
    if ((sint32)this->_sourisY >= this->_positionY)
    {
      return TRUE;
    }
  }

  return FALSE;
}

void ChampSaisieNumerique::cliqueGaucheRelache()
{
  // ATTENTION : Ne pas casser ce qui est fait dans Widget
  if(SURVOLE_ET_ENFONCE == this->_etat)
  {
    this->_etat = SURVOLE;
    this->_clique = TRUE;
    this->_apparenceModifiee = TRUE;

    if (this->flecheHautSurvolee())
    {
      this->modifierValeur(this->_valeur + 1, TRUE);
    }
    else if (this->flecheBasSurvolee())
    {
      this->modifierValeur(this->_valeur - 1, TRUE);
    }
  }
}

void ChampSaisieNumerique::sourisDeplacee(uint32 x, uint32 y)
{
  // ATTENTION : Ne pas casser ce qui est fait dans Widget

  // Si la souris est actuellement sur le widget
  if( TRUE == this->_visible
    && (sint32)x > this->_positionX - ((sint32)this->_largeur / 2)
    && (sint32)x < this->_positionX + ((sint32)this->_largeur / 2)
    && (sint32)y > this->_positionY - ((sint32)this->_hauteur / 2)
    && (sint32)y < this->_positionY + ((sint32)this->_hauteur / 2) )
  {
    // Si le widget n'etait pas survole
    if(PAS_SURVOLE == this->_etat)
    {
        this->_etat = SURVOLE;
        this->_apparenceModifiee = TRUE;
    }

    static bool8 flecheHautAnciennementSurvolee = FALSE;
    static bool8 flecheBasAnciennementSurvolee = FALSE;

    // Si la fleche du haut vient d'etre ou non survolee
    if (flecheHautAnciennementSurvolee != this->flecheHautSurvolee())
    {
      // Le champ devra etre redessine
      this->_apparenceModifiee = TRUE;

      // Memorisation du nouvel etat (forcement le contraire)
      flecheHautAnciennementSurvolee =  ! flecheHautAnciennementSurvolee;
    }

    // Si la fleche du haut vient d'etre ou non survolee
    if (flecheBasAnciennementSurvolee != this->flecheBasSurvolee())
    {
      // Le champ devra etre redessine
      this->_apparenceModifiee = TRUE;

      // Memorisation du nouvel etat (forcement le contraire)
      flecheBasAnciennementSurvolee =  ! flecheBasAnciennementSurvolee;
    }
  }
  // Si la souris n'est actuellement pas sur le widget
  else
  {
    // Si le widget etait survole ou enfonce
    if(SURVOLE == this->_etat || SURVOLE_ET_ENFONCE == this->_etat)
    {
      this->_etat = PAS_SURVOLE;
      this->_apparenceModifiee = TRUE;
    }
  }

  // Memorisation de l'emplacement de la souris pour donner acces a cette information aux widgets derives
  this->_sourisX = x;
  this->_sourisY = y;
}

void ChampSaisieNumerique::modifierMin(sint32 min)
{
  // Evite que le min soit supperieur au max
  if (min > this->_max)
  {
    this->_max = min;
  }

  this->_min = min;

  // Modification eventuelle de la valeur
  if (this->_valeur < min)
  {
    this->modifierValeur(min, this->_aLeFocus);
  }
}

void ChampSaisieNumerique::modifierMax(sint32 max)
{
  // Evite que le min soit supperieur au max
  if (max < this->_min)
  {
    this->_min = max;
  }

  this->_max = max;

  // Modification eventuelle de la valeur
  if (this->_valeur > max)
  {
    this->modifierValeur(max, this->_aLeFocus);
  }
}
