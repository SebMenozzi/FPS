#ifndef FICHIERINI_H_INCLUDED
#define FICHIERINI_H_INCLUDED

#include "types.h"
#include <string>
#include <map>

class FichierINI
{
  public:
    // Chargement / Enregistrement des variables d'initialisation
    bool8 charger(std::string nomFichier);
    bool8 enregistrer(std::string nomFichier);

    // Modification / Suppression / Lecture de variables d'initialisation
    void modifier(std::string nom, std::string valeur);
    void supprimer(std::string nom);
    std::string lire(std::string nom);
    void vider(void);

  private:
    // La liste des variables
    std::map<std::string, std::string> listeVariables;
};

#endif // FICHIERINI_H_INCLUDED
