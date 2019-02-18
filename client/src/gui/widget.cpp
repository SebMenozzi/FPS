#include <cmath>
#include <cstdio>
#include "widget.h"

Widget::Widget(sint32 x, sint32 y)
{
  this->x = x;
  this->y = y;
  this->stateChanged = TRUE;
  this->isVisible = TRUE;
  this->askFocus = FALSE;
  this->hasFocus = FALSE;
  this->state = PAS_SURVOLE;
  this->clicked = FALSE;
  this->width = 0;
  this->height = 0;
  this->focusable = TRUE;
}

Widget::~Widget()
{

}

void Widget::leftClickPressed()
{
  if(SURVOLE == this->state) {
    this->askFocus = TRUE;
    this->state = SURVOLE_ET_ENFONCE;
    this->stateChanged = TRUE;
  }
}

void Widget::leftClickReleased()
{
  if(SURVOLE_ET_ENFONCE == this->state) {
    this->state = SURVOLE;
    this->clicked = TRUE;
    this->stateChanged = TRUE;
  }
}

void Widget::cursorMoved(uint32 x, uint32 y)
{

  int widgetHovered = this->isVisible == TRUE &&
                (int) x > this->x - ((int) this->width / 2) &&
                (int) x < this->x + ((int) this->width / 2) &&
                (int) y > this->y - ((int) this->height / 2) &&
                (int) y < this->y + ((int) this->height / 2);

  // Si la souris est actuellement sur le widget
  if(widgetHovered) {
    // Si le widget n'etait pas survole
    if(PAS_SURVOLE == this->state) {
      this->state = SURVOLE;
      this->stateChanged = TRUE;
    }
  } else { // Si la souris n'est actuellement pas sur le widget
    // Si le widget etait survole ou enfonce
    if(SURVOLE == this->state || SURVOLE_ET_ENFONCE == this->state) {
      this->state = PAS_SURVOLE;
      this->stateChanged = TRUE;
    }
  }

  // Memorisation de l'emplacement de la souris pour donner acces a cette information aux widgets derives
  this->cursorX = x;
  this->cursorY = y;
}

void Widget::keyPressed(char key)
{

}

void Widget::keyReleased(char key)
{

}

bool8 Widget::click()
{
  bool8 clicked = this->clicked;
  this->clicked = FALSE;
  return clicked;
}

bool8 Widget::getAskFocus()
{
  bool8 askFocus = this->askFocus;
  this->askFocus = FALSE;
  return askFocus;
}

bool8 Widget::getStateChanged()
{
  bool8 stateChanged = this->stateChanged;
  this->stateChanged = FALSE;
  return stateChanged;
}

void Widget::focus()
{
  // ATTENTION : Reporte dans les classes derivees toutes modifications de cette methode
  if (TRUE == this->isFocusable()) {
    this->hasFocus = TRUE;
    this->stateChanged = TRUE;
  }
}

void Widget::blur()
{
  // ATTENTION : Reporte dans les classes derivees toutes modifications de cette methode
  this->hasFocus = FALSE;
  this->stateChanged = TRUE;
}

bool8 Widget::isFocused()
{
  return this->hasFocus;
}

bool8 Widget::isFocusable()
{
  if(FALSE == this->isVisible) {
    return FALSE;
  }

  return this->focusable;
}

void Widget::setVisibility(bool8 visible)
{
  this->isVisible = visible;

  if (FALSE == this->isVisible) {
    this->blur();
    this->state = PAS_SURVOLE;
  }

  this->stateChanged = TRUE;
}

void Widget::draw()
{
  if(TRUE == this->isVisible) {
    this->drawWidget();
  }
}

uint32 Widget::powerOfTwoGreater(uint32 i)
{
  double32 logbase2 = log(i) / log(2);
  return (uint32) round(pow(2.0, ceil(logbase2)));
}
