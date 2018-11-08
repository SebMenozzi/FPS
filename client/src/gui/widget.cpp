#include <cmath>
#include <cstdio>
#include "widget.h"

Widget::Widget(sint32 x, sint32 y)
{
  this->_positionX = x;
  this->_positionY = y;
  this->_apparenceModifiee = TRUE;
  this->_visible = TRUE;
  this->_demandeLeFocus = FALSE;
  this->_aLeFocus = FALSE;
  this->_etat = PAS_SURVOLE;
  this->_clique = FALSE;
  this->_largeur = 0;
  this->_hauteur = 0;
  this->_focussable = TRUE;
}

Widget::~Widget()
{

}

void Widget::cliqueGaucheEnfonce()
{
  if(SURVOLE == this->_etat)
  {
    this->_demandeLeFocus = TRUE;
    this->_etat = SURVOLE_ET_ENFONCE;
    this->_apparenceModifiee = TRUE;
  }
}

void Widget::cliqueGaucheRelache()
{
  if(SURVOLE_ET_ENFONCE == this->_etat)
  {
    this->_etat = SURVOLE;
    this->_clique = TRUE;
    this->_apparenceModifiee = TRUE;
  }
}

void Widget::sourisDeplacee(uint32 x, uint32 y)
{

  int survoleWidget = this->_visible == TRUE &&
                (int) x > this->_positionX - ((int) this->_largeur / 2) &&
                (int) x < this->_positionX + ((int) this->_largeur / 2) &&
                (int) y > this->_positionY - ((int) this->_hauteur / 2) &&
                (int) y < this->_positionY + ((int) this->_hauteur / 2);

  // Si la souris est actuellement sur le widget
  if(survoleWidget)
  {
    // Si le widget n'etait pas survole
    if(PAS_SURVOLE == this->_etat)
    {
      this->_etat = SURVOLE;
      this->_apparenceModifiee = TRUE;
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

void Widget::toucheEnfoncee(char c)
{

}

void Widget::toucheRelachee(char c)
{

}

bool8 Widget::clique()
{
  bool8 clique = this->_clique;
  this->_clique = FALSE;
  return clique;
}

bool8 Widget::demandeLeFocus()
{
  bool8 demandeLeFocus = this->_demandeLeFocus;
  this->_demandeLeFocus = FALSE;
  return demandeLeFocus;
}

bool8 Widget::apparenceModifiee()
{
  bool8 apparenceModifiee = this->_apparenceModifiee;
  this->_apparenceModifiee = FALSE;
  return apparenceModifiee;
}

void Widget::prendLeFocus()
{
  // ATTENTION : Reporte dans les classes derivees toutes modifications de cette methode
  if (TRUE == this->focussable())
  {
    this->_aLeFocus = TRUE;
    this->_apparenceModifiee = TRUE;
  }
}

void Widget::perdLeFocus()
{
  // ATTENTION : Reporte dans les classes derivees toutes modifications de cette methode
  this->_aLeFocus = FALSE;
  this->_apparenceModifiee = TRUE;
}

bool8 Widget::aLeFocus()
{
  return this->_aLeFocus;
}

bool8 Widget::focussable()
{
  if(FALSE == this->_visible)
  {
    return FALSE;
  }

  return this->_focussable;
}

void Widget::visible(bool8 visible)
{
  this->_visible = visible;

  if (FALSE == this->_visible)
  {
    this->perdLeFocus();
    this->_etat = PAS_SURVOLE;
  }

  this->_apparenceModifiee = TRUE;
}

void Widget::dessiner()
{
  if(TRUE == this->_visible)
  {
    this->dessinDuWidget();
  }
}

uint32 Widget::puissanceDeDeuxSuperieure(uint32 i)
{
  double32 logbase2 = log(i) / log(2);
  return (uint32)round(pow(2.0, ceil(logbase2)));
}
