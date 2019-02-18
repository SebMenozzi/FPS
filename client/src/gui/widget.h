#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED

#include "../types.h"

class Widget
{
  // Permet au Menu d'acceder aux membres de Widget
  friend class Menu;

  public: // L'utilisateur a acces a ces membres
    Widget(sint32 x, sint32 y);
    virtual ~Widget();
    bool8 click();
    void setVisibility(bool8 visible);

  private: // Seul le Menu a une autorisation speciale d'acces a ces membres
    // Cursor
    virtual void leftClickPressed();
    virtual void leftClickReleased();
    virtual void cursorMoved(uint32 x, uint32 y);

    // Keyboard
    virtual void keyPressed(char key);
    virtual void keyReleased(char key);

    // Focus
    virtual void focus();
    virtual void blur();
    bool8 isFocused();
    bool8 getAskFocus();

    // Dessin
    bool8 getStateChanged();
    void draw();
    virtual void drawWidget() = 0;

  protected: // Seule les classes derivees ont acces a ces membres
    // Attention : Le Menu a acces a ces membres mais ne doit pas y toucher

    // Etat du widget par rapport a la souris
    enum Etat { PAS_SURVOLE, SURVOLE, SURVOLE_ET_ENFONCE };
    Etat state;
    bool8 clicked;

    // Position / dimentions
    sint32 x;
    sint32 y;
    uint32 height;
    uint32 width;

    // Focus
    bool8 hasFocus;
    bool8 focusable;
    bool8 askFocus;
    bool8 isFocusable();

    bool8 stateChanged;
    bool8 isVisible;

    // Calculs
    uint32 powerOfTwoGreater(uint32 i);

    // Cursor
    uint32 cursorX;
    uint32 cursorY;
};

#endif // WIDGET_H_INCLUDED
