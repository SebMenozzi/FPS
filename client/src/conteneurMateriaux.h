#ifndef CONTENEURMATERIAUX_H_INCLUDED
#define CONTENEURMATERIAUX_H_INCLUDED

#include "materiau.h"
#include <map>
#include <string>

class ConteneurMateriaux
{
  public:
    void ajouterBibliotheque(std::string cheminBibliotheque);
    Materiau* materiau(std::string nom);

  private:
    std::map<std::string,Materiau> _listeMateriaux;
    Materiau materiauParDefaut;
};

#endif // CONTENEURMATERIAUX_H_INCLUDED
