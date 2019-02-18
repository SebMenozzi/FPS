#ifndef CHECKBOX_H_INCLUDED
#define CHECKBOX_H_INCLUDED

#include "widget.h"
#include "../conteneurTextures.h"

class Checkbox : public Widget
{
  public:
    Checkbox(sint32 x, sint32 y, bool8 checked = FALSE);
    ~Checkbox();
    bool8 isChecked();
    void keyPressed(char key);

  private:
    ConteneurTextures texturesContainer;
    void drawWidget();

    void leftClickPressed();
    bool8 checked;

    // TODO : Ajouter une methode clique() pour savoir si une action s'est produite
};

#endif // CHECKBOX_H_INCLUDED
