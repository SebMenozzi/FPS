#include "caseACocher.h"
#include "../configuration.h"

CaseACocher::CaseACocher(sint32 x, sint32 y, bool8 cochee) : Widget(x, y)
{
  #define CASE_COCHEE_SANS_FOCUS "case_cochee_sans_focus.bmp"
  #define CASE_PAS_COCHEE_SANS_FOCUS "case_non_cochee_sans_focus.bmp"
  #define CASE_COCHEE_AVEC_FOCUS "case_cochee_avec_focus.bmp"
  #define CASE_PAS_COCHEE_AVEC_FOCUS "case_non_cochee_avec_focus.bmp"
  this->conteneurTextures.ajouter(CASE_COCHEE_SANS_FOCUS);
  this->conteneurTextures.ajouter(CASE_PAS_COCHEE_SANS_FOCUS);
  this->conteneurTextures.ajouter(CASE_COCHEE_AVEC_FOCUS);
  this->conteneurTextures.ajouter(CASE_PAS_COCHEE_AVEC_FOCUS);
  this->_largeur = this->conteneurTextures.texture(CASE_COCHEE_SANS_FOCUS).largeur;
  this->_hauteur = this->conteneurTextures.texture(CASE_COCHEE_SANS_FOCUS).hauteur;

  this->_cochee = cochee;
}

CaseACocher::~CaseACocher()
{

}

void CaseACocher::dessinDuWidget()
{
  // Selection de l'image
  if (TRUE == this->_aLeFocus)
  {
    if (this->_cochee)
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CASE_COCHEE_AVEC_FOCUS).texture);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CASE_PAS_COCHEE_AVEC_FOCUS).texture);
    }
  }
  else
  {
    if (this->_cochee)
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CASE_COCHEE_SANS_FOCUS).texture);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(CASE_PAS_COCHEE_SANS_FOCUS).texture);
    }
  }

  // Application de l'image
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i( this->_positionX - (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY + (this->_hauteur/2));
    glTexCoord2i(0, 1); glVertex2i( this->_positionX - (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 1); glVertex2i( this->_positionX + (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY - (this->_hauteur/2));
    glTexCoord2i(1, 0); glVertex2i( this->_positionX + (this->_largeur/2), HAUTEUR_FENETRE - this->_positionY + (this->_hauteur/2));
  glEnd();
}

bool8 CaseACocher::cochee()
{
  return this->_cochee;
}

void CaseACocher::toucheEnfoncee(char c)
{
  // La case a le focus
  if (TRUE == this->_aLeFocus)
  {
    // Touche espace enfonce
    if (' ' == c)
    {
      // Code / decoche la case
      this->_cochee = ! this->_cochee;
      // L'apparance est modifiee, la case devra etre redessinee
      this->_apparenceModifiee = TRUE;
    }
  }
}

void CaseACocher::cliqueGaucheEnfonce()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  if(SURVOLE == this->_etat)
  {
    this->_demandeLeFocus = TRUE;
    this->_etat = SURVOLE_ET_ENFONCE;
    this->_apparenceModifiee = TRUE;

    // Coche / Decoche la case
    this->_cochee = !this->_cochee;
  }
}
