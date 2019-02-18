#include "button.h"
#include "../configuration.h"

TTF_Font* Button::font = NULL;
uint32 Button::compteurInstances = 0;

Button::Button(SDL_Window* window, sint32 x, sint32 y, std::string text) : Widget(x, y)
{
  this->windowWidth = 0;
  this->windowHeight = 0;

  // Recuperation des dimensions de la fenetre
  SDL_GetWindowSize(window, &this->windowWidth, &this->windowHeight);

  #define BOUTON_PAS_SURVOLE_SANS_FOCUS "bouton_pas_survole_sans_focus.bmp"
  #define BOUTON_PAS_SURVOLE_AVEC_FOCUS "bouton_pas_survole_avec_focus.bmp"
  #define BOUTON_SURVOLE_SANS_FOCUS "bouton_survole_sans_focus.bmp"
  #define BOUTON_SURVOLE_AVEC_FOCUS "bouton_survole_avec_focus.bmp"
  #define BOUTON_ENFONCE "bouton_enfonce.bmp"

  this->texturesContainer.ajouter(BOUTON_PAS_SURVOLE_SANS_FOCUS);
  this->texturesContainer.ajouter(BOUTON_PAS_SURVOLE_AVEC_FOCUS);
  this->texturesContainer.ajouter(BOUTON_SURVOLE_SANS_FOCUS);
  this->texturesContainer.ajouter(BOUTON_SURVOLE_AVEC_FOCUS);
  this->texturesContainer.ajouter(BOUTON_ENFONCE);

  this->width = this->texturesContainer.texture(BOUTON_ENFONCE).largeur;
  this->height = this->texturesContainer.texture(BOUTON_ENFONCE).hauteur;

  if(0 == this->compteurInstances) {
      this->font = TTF_OpenFont("assets/Cartonsix NC.ttf", 35);
  }
  this->compteurInstances++;

  this->textTexture = 0;

  // Creation de l'image du texte avec la police associee
  this->setText(text);
}

Button::~Button()
{
  this->texturesContainer.supprimer(BOUTON_PAS_SURVOLE_SANS_FOCUS);
  this->texturesContainer.supprimer(BOUTON_PAS_SURVOLE_AVEC_FOCUS);
  this->texturesContainer.supprimer(BOUTON_SURVOLE_SANS_FOCUS);
  this->texturesContainer.supprimer(BOUTON_SURVOLE_AVEC_FOCUS);
  this->texturesContainer.supprimer(BOUTON_ENFONCE);

  glDeleteTextures(1, &this->textTexture);

  this->compteurInstances--;
  if(0 == this->compteurInstances) {
    // Liberation de la police
    TTF_CloseFont(this->font);
  }
}

void Button::drawWidget()
{
  // Selection de l'image de fond
  switch (this->state) {
    case SURVOLE_ET_ENFONCE: {
      glBindTexture(GL_TEXTURE_2D, this->texturesContainer.texture(BOUTON_ENFONCE).texture);
    }
    break;

    case SURVOLE: {
      if(TRUE == this->hasFocus) {
        glBindTexture(GL_TEXTURE_2D, this->texturesContainer.texture(BOUTON_SURVOLE_AVEC_FOCUS).texture);
      } else {
        glBindTexture(GL_TEXTURE_2D, this->texturesContainer.texture(BOUTON_SURVOLE_SANS_FOCUS).texture);
      }
    }
    break;

    case PAS_SURVOLE: {
      if(TRUE == this->hasFocus) {
        glBindTexture(GL_TEXTURE_2D, this->texturesContainer.texture(BOUTON_PAS_SURVOLE_AVEC_FOCUS).texture);
      }
      else {
        glBindTexture(GL_TEXTURE_2D, this->texturesContainer.texture(BOUTON_PAS_SURVOLE_SANS_FOCUS).texture);
      }
    }
    break;
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

void Button::focus(char c)
{
  // Si le bouton a le focus et que la touche "Entree" est enfoncee
  if (TRUE == this->hasFocus && '\r' == c)
  {
    // Le bouton est clique
    this->clicked = TRUE;
  }
}

std::string Button::getText()
{
  return this->text;
}

void Button::setText(std::string text)
{
  this->text = text;

  // Destruction de la texture potentiellement existante
  glDeleteTextures(1, &this->textTexture);

  // Creation de l'image du texte avec la police associee
  SDL_Color colorText = { 50, 50, 50 };
  SDL_Surface* textButton = TTF_RenderText_Blended(this->font, text.c_str(), colorText);
  if (NULL == textButton) {
    printf("Failure to create the text of the button!");
  }

  // Activation de la transparence
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Generation de la texture pour le texte du bouton
  glGenTextures(1, &this->textTexture);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, this->textTexture);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum pixelEncoding;
  if (textButton->format->Rmask == 0x000000ff) {
    pixelEncoding = GL_RGBA;
  } else {
    #ifndef GL_BGRA
      #define GL_BGRA 0x80E1
    #endif
    pixelEncoding = GL_BGRA;
  }

  //Recuperation des dimensions du texte
  this->textWidth = textButton->w;
  this->textHeight = textButton->h;
  this->textureWidth = powerOfTwoGreater(textButton->w);
  this->textureHeight = powerOfTwoGreater(textButton->h);
  this->largeurPortionUtile = (float16) this->textWidth / this->textureWidth;
  this->hauteurPortionUtile = (float16) this->textHeight / this->textureHeight;

  // Chargement de la texture du texte precedament creee
  //glTexImage2D(GL_TEXTURE_2D, 0, 4, texteBouton->w, texteBouton->h, 0,
  //              codagePixel, GL_UNSIGNED_BYTE, texteBouton->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, this->textureWidth, this->textureHeight, 0, pixelEncoding, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textButton->w, textButton->h, pixelEncoding, GL_UNSIGNED_BYTE, textButton->pixels);

  //Recuperation des dimensions du texte
  this->textWidth = textButton->w;
  this->textHeight = textButton->h;

  // Liberation de l'image du texte du bouton
  SDL_FreeSurface(textButton);

  // L'apparance a ete modifiee, le bouton devra etre redessine
  stateChanged = TRUE;
}
