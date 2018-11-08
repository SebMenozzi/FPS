#ifndef CASE_H_INCLUDED
#define CASE_H_INCLUDED

#include "widget.h"
#include "../conteneurTextures.h"

class CaseACocher : public Widget
{
  public:
    CaseACocher(sint32 x, sint32 y, bool8 coche = FALSE);
    ~CaseACocher();
    bool8 cochee();
    void toucheEnfoncee(char c);

  private:
    ConteneurTextures conteneurTextures;
    void dessinDuWidget();

    void cliqueGaucheEnfonce();
    bool8 _cochee;

    // TODO : Ajouter une methode clique() pour savoir si une action s'est produite
};

#endif // CASE_H_INCLUDED
