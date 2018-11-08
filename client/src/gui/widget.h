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
    bool8 clique();
    void visible(bool8 visible);

  private: // Seul le Menu a une autorisation speciale d'acces a ces membres
    // Souris
    virtual void cliqueGaucheEnfonce();
    virtual void cliqueGaucheRelache();
    virtual void sourisDeplacee(uint32 x, uint32 y);

    // Clavier
    virtual void toucheEnfoncee(char c);
    virtual void toucheRelachee(char c);

    // Focus
    virtual void prendLeFocus();
    virtual void perdLeFocus();
    bool8 aLeFocus();
    bool8 demandeLeFocus();

    // Dessin
    bool8 apparenceModifiee();
    void dessiner();
    virtual void dessinDuWidget() = 0;

  protected: // Seule les classes derivees ont acces a ces membres
    // Attention : Le Menu a acces a ces membres mais ne doit pas y toucher

    // Etat du widget par rapport a la souris
    enum Etat {PAS_SURVOLE, SURVOLE, SURVOLE_ET_ENFONCE};
    Etat _etat;
    bool8 _clique;

    // Position / dimentions
    sint32 _positionX;
    sint32 _positionY;
    uint32 _hauteur;
    uint32 _largeur;

    // Focus
    bool8 _aLeFocus;
    bool8 _focussable;
    bool8 _demandeLeFocus;
    bool8 focussable();

    bool8 _apparenceModifiee;
    bool8 _visible;

    // Calculs
    uint32 puissanceDeDeuxSuperieure(uint32 i);

    // Souris
    uint32 _sourisX;
    uint32 _sourisY;
};

#endif // WIDGET_H_INCLUDED
