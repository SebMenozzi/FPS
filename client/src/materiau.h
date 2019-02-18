#ifndef MATERIAU_H_INCLUDED
#define MATERIAU_H_INCLUDED

#include "color3Float.h"
#include "conteneurTextures.h"

class Materiau
{
  public:
    Materiau();
    Color3Float Ka();
    void Ka(Color3Float);
    Color3Float Kd();
    void Kd(Color3Float);
    Color3Float Ks();
    void Ks(Color3Float);
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
    Color3Float _Ka; // couleur ambiente
    Color3Float _Kd; // couleur diffuse
    Color3Float _Ks; // couleur speculaire
    float16 _d; // Transparence
    float16 _Ns; // Brillance
    uint32 _illum; //
    std::string _map_Kd;

    std::string _cheminDeLaBibliotheque;

    ConteneurTextures _conteneurTexture;
};

#endif // MATERIAU_H_INCLUDED
