#ifndef MATERIAU_H_INCLUDED
#define MATERIAU_H_INCLUDED

#include "couleur3Float.h"
#include "conteneurTextures.h"

class Materiau
{
  public:
    Materiau();
    Couleur3Float Ka();
    void Ka(Couleur3Float);
    Couleur3Float Kd();
    void Kd(Couleur3Float);
    Couleur3Float Ks();
    void Ks(Couleur3Float);
    float16 d();
    void d(float16);
    float16 Ns();
    void Ns(float16);
    uint32 illum();
    void illum(uint32);
    std::string map_Kd();
    void map_Kd(std::string);
    void cheminDeLaBibliotheque(std::string chemin);

    void selectionner();
    uint32 tex();

  private:
    Couleur3Float _Ka; // couleur ambiente
    Couleur3Float _Kd; // couleur diffuse
    Couleur3Float _Ks; // couleur speculaire
    float16 _d; // Transparence
    float16 _Ns; // Brillance
    uint32 _illum; //
    std::string _map_Kd;

    std::string _cheminDeLaBibliotheque;

    ConteneurTextures _conteneurTexture;
};

#endif // MATERIAU_H_INCLUDED
