#include <cmath>
#include "TextInput.h"
#include "../configuration.h"

TTF_Font* TextInput::font = NULL;
uint32 TextInput::compteurInstance = 0;

TextInput::TextInput(SDL_Window* window, sint32 x, sint32 y) : Widget(x, y)
{
  this->windowWidth = 0;
  this->windowHeight = 0;

  SDL_GetWindowSize(window, &this->windowWidth, &this->windowHeight);

  #define INPUT_TEXT_WITH_FOCUS "input_text_with_focus.bmp"
  #define INPUT_TEXT_WITHOUT_FOCUS "input_text_without_focus.bmp"

  this->texturesContainer.addTexture(INPUT_TEXT_WITH_FOCUS);
  this->texturesContainer.addTexture(INPUT_TEXT_WITHOUT_FOCUS);
  this->width = this->texturesContainer.getTexture(INPUT_TEXT_WITHOUT_FOCUS).width;
  this->height = this->texturesContainer.getTexture(INPUT_TEXT_WITHOUT_FOCUS).height;

  if(0 == this->compteurInstance) {
    this->font = TTF_OpenFont("assets/Cartonsix NC.ttf", 35);
  }
  this->compteurInstance++;

  this->changeText("", FALSE);
}

TextInput::~TextInput()
{
  this->texturesContainer.deleteTexture(INPUT_TEXT_WITH_FOCUS);
  this->texturesContainer.deleteTexture(INPUT_TEXT_WITHOUT_FOCUS);

  glDeleteTextures(1, &this->textTexture);

  this->compteurInstance--;
  if(0 == this->compteurInstance) {
    TTF_CloseFont(this->font);
  }
}

void TextInput::keyPressed(char key)
{
  if (TRUE == this->hasFocus) {
    std::string newText = this->text;

    if ('\b' == key) {
      if(this->text.size() > 0) {
        newText.resize(newText.size() - 1);
      }
    } else {
      if (key >= 0x20) { // space
        newText += key;
      }
    }

    this->changeText(newText, TRUE);
  }
}

void TextInput::drawWidget()
{
  if(TRUE == this->hasFocus) {
    // Selection de l'image de fond
    glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(INPUT_TEXT_WITH_FOCUS).texture);
  } else {
    // Selection de l'image de fond
    glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture(INPUT_TEXT_WITHOUT_FOCUS).texture);
  }

  // Application de l'image de fond
  glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2i(
      this->x - (this->width / 2),
      this->windowHeight - this->y + (this->height / 2)
    );

    glTexCoord2i(0, 1);
    glVertex2i(
      this->x - (this->width / 2),
      this->windowHeight - this->y - (this->height / 2)
    );

    glTexCoord2i(1, 1);
    glVertex2i(
      this->x + (this->width / 2),
      this->windowHeight - this->y - (this->height / 2)
    );

    glTexCoord2i(1, 0);
    glVertex2i(
      this->x + (this->width / 2),
      this->windowHeight - this->y + (this->height / 2)
    );

  glEnd();


  #define BORDURE 3

  // Si le texte deborde de la zone de texte
  if(this->textWidth + (2 * BORDURE) > this->width) {
    float16 portionASupprimer = (
      (this->textWidth - ((float16) this->width - 2 * BORDURE)) / this->textureWidth
    );

     // Selection de la texture du texte
    glBindTexture(GL_TEXTURE_2D, this->textTexture);

    // Application du texte
    glBegin(GL_QUADS);

      glTexCoord2f(portionASupprimer, 0);
      glVertex2i(
        this->x - (this->width / 2) + BORDURE,
        this->windowHeight - this->y + (this->textHeight / 2)
      );

      glTexCoord2f(portionASupprimer, this->hauteurPortionUtile);
      glVertex2i(
        this->x - (this->width / 2) + BORDURE,
        this->windowHeight - this->y - (this->textHeight / 2)
      );

      glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile);
      glVertex2i(
        this->x + (this->width / 2) - BORDURE,
        this->windowHeight - this->y - (this->textHeight / 2)
      );

      glTexCoord2f(this->largeurPortionUtile, 0);
      glVertex2i(
        this->x + (this->width / 2) - BORDURE,
        this->windowHeight - this->y + (this->textHeight / 2)
      );

    glEnd();

  } else {
    // Selection de la texture du texte
    glBindTexture(GL_TEXTURE_2D, this->textTexture);

    // Application du texte
    glBegin(GL_QUADS);
      //glTexCoord2f(0.0, 0.0); glVertex2i( this->_positionX - (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY + (this->hauteurTexte/2));
      //glTexCoord2f(0.0, this->hauteurPortionUtile); glVertex2i( this->_positionX - (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY - (this->hauteurTexte/2));
      //glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile); glVertex2i( this->_positionX + (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY - (this->hauteurTexte/2));
      //glTexCoord2f(this->largeurPortionUtile, 0.0); glVertex2i( this->_positionX + (this->largeurTexte/2), HAUTEUR_FENETRE - this->_positionY + (this->hauteurTexte/2));

      glTexCoord2f(0.0, 0.0);
      glVertex2i(
        this->x - (this->width / 2) + BORDURE,
        this->windowHeight - this->y + (this->textHeight / 2)
      );

      glTexCoord2f(0.0, this->hauteurPortionUtile);
      glVertex2i(
        this->x - (this->width / 2) + BORDURE,
        this->windowHeight - this->y - (this->textHeight / 2)
      );

      glTexCoord2f(
        this->largeurPortionUtile,
        this->hauteurPortionUtile
      );
      glVertex2i(
        this->x - (this->width / 2) + (this->textWidth) + BORDURE,
        this->windowHeight - this->y - (this->textHeight / 2)
      );

      glTexCoord2f(this->largeurPortionUtile, 0.0);
      glVertex2i(
        this->x - (this->width / 2) + (this->textWidth) + BORDURE,
        this->windowHeight - this->y + (this->textHeight / 2)
      );

    glEnd();
  }
}

void TextInput::changeText(std::string text, bool8 cursor)
{
  this->text = text;
  std::string textToDisplay;

  if(cursor) {
    textToDisplay = this->text + '|';
  } else {
    textToDisplay = this->text;
  }

  if(0 == textToDisplay.size()) {
    textToDisplay = " ";
  }

  // Creation de l'image du texte avec la font associee
  SDL_Color textColor = { 50, 50, 50 };
  SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, textToDisplay.c_str(), textColor);

  // Activation de la transparence
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
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, surfaceTexte->w, surfaceTexte->h, 0, codagePixel, GL_UNSIGNED_BYTE, surfaceTexte->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->textureWidth, this->textureHeight, 0, pixelEncoding, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->textWidth, this->textHeight, pixelEncoding, GL_UNSIGNED_BYTE, textSurface->pixels);

  // Liberation de l'image du texte
  SDL_FreeSurface(textSurface);

  // L'apparance est modifiee, la zone de texte devra etre redessinee
  this->stateChanged = TRUE;
}

std::string TextInput::getText()
{
  return this->text;
}

void TextInput::focus()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  if (TRUE == this->isFocusable()) {
    this->hasFocus = TRUE;
    this->changeText(this->text, TRUE); // Ajout du curseur
  }
}

void TextInput::blur()
{
  // ATTENTION : Ne pas detruire ce qui est fait dans Widget
  this->hasFocus = FALSE;
  this->changeText(this->text, FALSE); // Retire le curseur
}
