#include "numericInput.h"
#include "../configuration.h"
#include <sstream>

TTF_Font* NumericInput::font = NULL;
uint32 NumericInput::compteurInstances = 0;

NumericInput::NumericInput(sint32 x, sint32 y, sint32 value) : Widget(x, y)
{
  #define CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS "champ_saisie_numerique_sans_focus.bmp"
  #define CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS "champ_saisie_numerique_avec_focus.bmp"
  #define CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE "champ_saisie_numerique_bouton_bas_enfonce.bmp"
  #define CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE "champ_saisie_numerique_bouton_haut_enfonce.bmp"

  this->texturesContainer.addTexture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS);
  this->texturesContainer.addTexture(CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS);
  this->texturesContainer.addTexture(CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE);
  this->texturesContainer.addTexture(CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE);

  this->width = this->texturesContainer.getTexture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS).width;
  this->height = this->texturesContainer.getTexture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS).height;
  // by default
  this->min = 0;
  this->max = 100;

  if(0 == this->compteurInstances) {
      this->font = TTF_OpenFont("assets/Cartonsix NC.ttf", 35);
  }
  this->compteurInstances++;

  this->changeValue(value);
}

NumericInput::~NumericInput()
{
  this->texturesContainer.deleteTexture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS);
  this->texturesContainer.deleteTexture(CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS);
  this->texturesContainer.deleteTexture(CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE);
  this->texturesContainer.deleteTexture(CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE);

  glDeleteTextures(1, &this->textTexture);

  this->compteurInstances--;
  if(0 == this->compteurInstances) {
    // free the font
    TTF_CloseFont(this->font);
  }
}

void NumericInput::keyPressed(char key)
{
  // if the numeric input has focus
  if (TRUE == this->hasFocus) {
    if ('+' == key) { // and we press "+" key
      // Increment the value
      this->changeValue(this->value + 1, TRUE);
    }

    // key "-" pressed
    if ('-' == key) {
      // Decrement the value
      this->changeValue(this->value - 1, TRUE);
    }

    // the key of a digit (0-9)
    if ('0' <= key || '9' >= key) {
      std::stringstream text;
      text << this->value;
      text << key;

      sint32 value;
      text >> value;
      this->changeValue(value, TRUE);
    }

    // press delete key
    if ('\b' == key) {
      // divide by 10
      this->changeValue(this->value / 10, TRUE);
    }
  }
}

void NumericInput::changeValue(sint32 value, bool8 cursor)
{
  if (this->min > value) {
    value = this->min;
  } else if (this->max < value) {
    value = this->max;
  }

  this->value = value;
  std::stringstream textToDisplay;
  if(cursor) {
    textToDisplay << this->value << '|';
  } else {
    textToDisplay << this->value;
  }

  if(0 == textToDisplay.str().size()) {
    textToDisplay << " ";
  }

  SDL_Color textColor = {50, 50, 50};
  SDL_Surface* textSurface = TTF_RenderText_Blended(
    this->font,
    textToDisplay.str().c_str(),
    textColor
  );

  // alpha activation
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Remplacement de la texture
  glDeleteTextures(1, &this->textTexture);
  glGenTextures(1, &this->textTexture);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, this->textTexture);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum pixelEncoding;
  if (textSurface->format->Rmask == 0x000000ff) {
    pixelEncoding = GL_RGBA;
  } else {
    #ifndef GL_BGRA
      #define GL_BGRA 0x80E1
    #endif
    pixelEncoding = GL_BGRA;
  }

  //Recuperation des dimentions du texte
  this->textWidth = textSurface->w;
  this->textHeight = textSurface->h;
  this->textureWidth = powerOfTwoGreater(textSurface->w);
  this->textureHeight = powerOfTwoGreater(textSurface->h);
  this->largeurPortionUtile = (float16) this->textWidth / this->textureWidth;
  this->hauteurPortionUtile = (float16) this->textHeight / this->textureHeight;

  // Chargement de la texture du texte precedament creee
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, surfaceTexte->w, surfaceTexte->h, 0,
  //              codagePixel, GL_UNSIGNED_BYTE, surfaceTexte->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->textureWidth, this->textureHeight, 0, pixelEncoding, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->textureWidth, this->textureHeight, pixelEncoding, GL_UNSIGNED_BYTE, textSurface->pixels);

  // Liberation de l'image du texte
  SDL_FreeSurface(textSurface);

  // L'apparance est modifiee, la zone de texte devra etre redessinee
  this->stateChanged = TRUE;
}

sint32 NumericInput::getValue()
{
  return this->value;
}

void NumericInput::focus()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  if (TRUE == this->isFocusable()) {
    this->hasFocus = TRUE;
    this->changeValue(this->value, TRUE); // Ajout du curseur
  }
}

void NumericInput::blur()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  this->hasFocus = FALSE;

  this->changeValue(this->value, FALSE); // Retire le curseur
}

void NumericInput::drawWidget()
{
  // Selection de l'image de fond
  if(TRUE == this->hasFocus) {
    if(this->topArrowHovered() && SURVOLE_ET_ENFONCE == this->state) { // Fleche du haut survolee et enfoncee
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CHAMP_SAISIE_NUMERIQUE_BOUTON_HAUT_ENFONCE).texture);
    } else if (this->bottomArrowHovered() && SURVOLE_ET_ENFONCE == this->state) { // Fleche du bas survolee et enfoncee
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CHAMP_SAISIE_NUMERIQUE_BOUTON_BAS_ENFONCE).texture);
    } else { // Simplement en pocession du focus
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CHAMP_SAISIE_NUMERIQUE_AVEC_FOCUS).texture);
    }
  } else { // Sans focus
    glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(CHAMP_SAISIE_NUMERIQUE_SANS_FOCUS).texture);
  }

  // Application de l'image de fond
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
    glVertex2i
      (this->x + (this->width / 2),
      WINDOW_HEIGHT - this->y - (this->height / 2)
    );

    glTexCoord2i(1, 0);
    glVertex2i(
      this->x + (this->width / 2),
      WINDOW_HEIGHT - this->y + (this->height / 2)
    );

  glEnd();

  // Selection de la texture du texte
  glBindTexture(GL_TEXTURE_2D, this->textTexture);

  // Application du texte
  glBegin(GL_QUADS);
    #define BORDURE 8

    glTexCoord2f(0.0, 0.0);
    glVertex2i(
      this->x - (this->width / 2) + BORDURE,
      WINDOW_HEIGHT - this->y + (this->textHeight / 2)
    );

    glTexCoord2f(0.0, this->hauteurPortionUtile);
    glVertex2i(
      this->x - (this->width / 2) + BORDURE,
      WINDOW_HEIGHT - this->y - (this->textHeight / 2)
    );

    glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile);
    glVertex2i(
      this->x - (this->width / 2) + (this->textWidth) + BORDURE,
      WINDOW_HEIGHT - this->y - (this->textHeight / 2)
    );

    glTexCoord2f(this->largeurPortionUtile, 0.0);
    glVertex2i(
      this->x - (this->width / 2) + (this->textWidth) + BORDURE,
      WINDOW_HEIGHT - this->y + (this->textHeight / 2)
    );

  glEnd();
}

bool8 NumericInput::topArrowHovered()
{
  // If the arrow is hovered
  if (this->cursorX > this->x + (this->width / 2) - 32) {
    // If the top arrow is hovered
    if ((sint32) this->cursorX < this->x) {
      return TRUE;
    }
  }

  return FALSE;
}

bool8 NumericInput::bottomArrowHovered()
{
  // If the arrow is hovered
  if (this->cursorX > this->x + (this->width / 2) - 32) {
    // If the bottom arrow is hovered
    if ((sint32) this->cursorX >= this->x) {
      return TRUE;
    }
  }

  return FALSE;
}

void NumericInput::leftClickReleased()
{
  // ATTENTION : Ne pas casser ce qui est fait dans Widget
  if(SURVOLE_ET_ENFONCE == this->state)
  {
    this->state = SURVOLE;
    this->clicked = TRUE;
    this->stateChanged = TRUE;

    if (this->topArrowHovered()) {
      // Increment value
      this->changeValue(this->value + 1, TRUE);
    } else if (this->bottomArrowHovered()) {
      // Decrement value
      this->changeValue(this->value - 1, TRUE);
    }
  }
}

void NumericInput::cursorMoved(uint32 x, uint32 y)
{
  // ATTENTION : Ne pas casser ce qui est fait dans Widget

  // Si la souris est actuellement sur le widget
  if(TRUE == this->isVisible
    && (sint32) x > this->x - ((sint32) this->width / 2)
    && (sint32) x < this->x + ((sint32) this->width / 2)
    && (sint32) y > this->y - ((sint32) this->height / 2)
    && (sint32) y < this->y + ((sint32) this->height / 2) ) {

    // Si le widget n'etait pas survole
    if(PAS_SURVOLE == this->state) {
        this->state = SURVOLE;
        this->stateChanged = TRUE;
    }

    static bool8 topArrowHasBeenHovered = FALSE;
    static bool8 bottomArrowHasBeenHovered = FALSE;

    // Si la fleche du haut vient d'etre ou non survolee
    if (topArrowHasBeenHovered != this->topArrowHovered()) {
      // Le champ devra etre redessine
      this->stateChanged = TRUE;

      // Memorisation du nouvel etat (forcement le contraire)
      topArrowHasBeenHovered =  !topArrowHasBeenHovered;
    }

    // Si la fleche du haut vient d'etre ou non survolee
    if (bottomArrowHasBeenHovered != this->bottomArrowHovered()) {
      // Le champ devra etre redessine
      this->stateChanged = TRUE;

      // Memorisation du nouvel etat (forcement le contraire)
      bottomArrowHasBeenHovered =  !bottomArrowHasBeenHovered;
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

void NumericInput::changeMin(sint32 min)
{
  if (min > this->max) {
    this->max = min;
  }

  this->min = min;

  if (this->value < min) {
    this->changeValue(min, this->hasFocus);
  }
}

void NumericInput::changeMax(sint32 max)
{
  if (max < this->min) {
    this->min = max;
  }
  this->max = max;

  if (this->value > max) {
    this->changeValue(max, this->hasFocus);
  }
}
