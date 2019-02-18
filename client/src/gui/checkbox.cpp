#include "checkbox.h"
#include "../configuration.h"

Checkbox::Checkbox(sint32 x, sint32 y, bool8 checked) : Widget(x, y)
{
  #define CASE_COCHEE_SANS_FOCUS "case_cochee_sans_focus.bmp"
  #define CASE_PAS_COCHEE_SANS_FOCUS "case_non_cochee_sans_focus.bmp"
  #define CASE_COCHEE_AVEC_FOCUS "case_cochee_avec_focus.bmp"
  #define CASE_PAS_COCHEE_AVEC_FOCUS "case_non_cochee_avec_focus.bmp"

  this->texturesContainer.addTexture(CASE_COCHEE_SANS_FOCUS);
  this->texturesContainer.addTexture(CASE_PAS_COCHEE_SANS_FOCUS);
  this->texturesContainer.addTexture(CASE_COCHEE_AVEC_FOCUS);
  this->texturesContainer.addTexture(CASE_PAS_COCHEE_AVEC_FOCUS);

  this->width = this->texturesContainer.getTexture(CASE_COCHEE_SANS_FOCUS).width;
  this->height = this->texturesContainer.getTexture(CASE_COCHEE_SANS_FOCUS).height;

  this->checked = checked;
}

Checkbox::~Checkbox()
{

}

void Checkbox::drawWidget()
{
  // Selection de l'image
  if (TRUE == this->hasFocus) {
    if (this->checked) {
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CASE_COCHEE_AVEC_FOCUS).texture);
    } else {
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CASE_PAS_COCHEE_AVEC_FOCUS).texture);
    }
  }
  else
  {
    if (this->checked) {
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CASE_COCHEE_SANS_FOCUS).texture);
    } else {
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CASE_PAS_COCHEE_SANS_FOCUS).texture);
    }
  }

  // Application de l'image
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(
      this->x - (this->width / 2),
      WINDOW_HEIGHT - this->y + (this->height / 2)
    );

    glTexCoord2i(0, 1);
    glVertex2i(
      this->x - (this->width / 2),
      WINDOW_HEIGHT - this->y - (this->height / 2)
    );

    glTexCoord2i(1, 1);
    glVertex2i(
      this->x + (this->width / 2),
      WINDOW_HEIGHT - this->y - (this->height / 2)
    );

    glTexCoord2i(1, 0);
    glVertex2i(
      this->x + (this->width / 2),
      WINDOW_HEIGHT - this->y + (this->height / 2)
    );

  glEnd();
}

bool8 Checkbox::isChecked()
{
  return this->checked;
}

void Checkbox::keyPressed(char key)
{
  // La case a le focus
  if (TRUE == this->hasFocus) {
    // Touche espace enfonce
    if (' ' == key) {
      // Toggle checkbox
      this->checked = !this->checked;
      // L'apparance est modifiee, la case devra etre redessinee
      this->stateChanged = TRUE;
    }
  }
}

void Checkbox::leftClickPressed()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  if(SURVOLE == this->state) {
    this->askFocus = TRUE;
    this->state = SURVOLE_ET_ENFONCE;
    this->stateChanged = TRUE;

    // Toggle checkbox
    this->checked = !this->checked;
  }
}
