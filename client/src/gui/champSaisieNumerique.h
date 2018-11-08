#ifndef CHAMPSAISIENUMERIQUE_H_INCLUDED
#define CHAMPSAISIENUMERIQUE_H_INCLUDED

#include <SDL2/SDL_ttf.h>
#include "widget.h"
#include "../types.h"
#include "../conteneurTextures.h"

class ChampSaisieNumerique : public Widget
{
  public:
    ChampSaisieNumerique(sint32 x, sint32 y, sint32 valeur);
    ~ChampSaisieNumerique();
    void dessiner();
    void toucheEnfoncee(char c);
    void modifierValeur(sint32 valeur, bool8 curseur = FALSE);
    sint32 valeur();
    void prendLeFocus();
    void perdLeFocus();
    void modifierMin(sint32 min);
    void modifierMax(sint32 max);

  private:
    ConteneurTextures conteneurTextures;
    GLuint textureTexte;
    uint32 largeurTexte;
    uint32 hauteurTexte;
    uint32 largeurTexture;
    uint32 hauteurTexture;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    static TTF_Font* police;
    static uint32 compteurInstances;
    void dessinDuWidget();
    sint32 _valeur;
    bool8 flecheHautSurvolee();
    bool8 flecheBasSurvolee();
    void cliqueGaucheRelache();
    void sourisDeplacee(uint32 x, uint32 y);
    sint32 _min;
    sint32 _max;
};

#endif // CHAMPSAISIENUMERIQUE_H_INCLUDED
