#include <SDL2/SDL.h>
#include <iostream>
#include "label.h"
#include "../configuration.h"

Label::Label(SDL_Window* window, sint32 x, sint32 y, std::string text, uint32 size) : Widget (x, y)
{
  this->font = 0;
  this->textTexture = 0;
  this->windowWidth = 0;
  this->windowHeight = 0;

  SDL_GetWindowSize(window, &this->windowWidth, &this->windowHeight);

  this->font = TTF_OpenFont("assets/Cartonsix NC.ttf", size);
  if (NULL == this->font) {
    std::cerr << "Impossible de charger la police" << std::endl;
  }

  this->changeText(text);

  this->focusable = FALSE;
}

Label::~Label()
{
  glDeleteTextures(1, &this->textTexture);
  TTF_CloseFont(this->font);
}

void Label::drawWidget()
{
  // Selection de la texture du texte
  glBindTexture(GL_TEXTURE_2D, this->textTexture);

  // Application du texte
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2i(
      this->x - (this->textWidth / 2),
      this->windowHeight - this->y + (this->textHeight / 2)
    );

    glTexCoord2f(0.0, this->hauteurPortionUtile);
    glVertex2i(
      this->x - (this->textWidth / 2),
      this->windowHeight - this->y - (this->textHeight / 2)
    );

    glTexCoord2f(this->largeurPortionUtile, this->hauteurPortionUtile);
    glVertex2i(
      this->x + (this->textWidth / 2),
      this->windowHeight - this->y - (this->textHeight / 2)
    );

    glTexCoord2f(this->largeurPortionUtile, 0.0);
    glVertex2i(
      this->x + (this->textWidth / 2),
      this->windowHeight - this->y + (this->textHeight / 2)
    );

  glEnd();
}

void Label::changeText(std::string text)
{
  // S'il n'y a pas de texte, on prend du blanc
  if (0 == text.size()) {
    text = " ";
  }

  // Creation de l'image du texte avec la police associee
  SDL_Color textColor = { 50, 50, 50 };
  SDL_Surface* textLabel = TTF_RenderText_Blended(this->font, text.c_str(), textColor);
  if (NULL == textLabel) {
    printf("Impossible de creer le texte de l'etiquette");
    return;
  }

  // Apha activation
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Regeneration de la texture du texte
  glDeleteTextures(1, &this->textTexture);
  glGenTextures(1, &this->textTexture);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, this->textTexture);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum pixelEncoding;
  if (textLabel->format->Rmask == 0x000000ff) {
    pixelEncoding = GL_RGBA;
  } else {
    #ifndef GL_BGRA
      #define GL_BGRA 0x80E1
    #endif
    pixelEncoding = GL_BGRA;
  }

  //Recuperation des dimentions du texte
  this->textWidth = textLabel->w;
  this->textHeight = textLabel->h;

  this->textureWidth = powerOfTwoGreater(textLabel->w);
  this->textureHeight = powerOfTwoGreater(textLabel->h);
  this->largeurPortionUtile = (float16) this->textWidth / this->textureWidth;
  this->hauteurPortionUtile = (float16) this->textHeight / this->textureHeight;

  // Chargement de la texture du texte precedament creee
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->textureWidth, this->textureHeight, 0, pixelEncoding, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->textWidth, this->textHeight, pixelEncoding, GL_UNSIGNED_BYTE, textLabel->pixels);
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, texteEtiquette->w, texteEtiquette->h, 0, codagePixel, GL_UNSIGNED_BYTE, texteEtiquette->pixels);

  // Liberation de l'image du texte du bouton
  SDL_FreeSurface(textLabel);

  // L'apparence etant modifiee, l'etiquette devra etre redessinee
  this->stateChanged = TRUE;
}
