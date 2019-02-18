#include "fonctionsUtiles.h"
#include <vector>
#include <sstream>
#include <algorithm>

void deparasiterFin(std::string* chaineADeparasiter, std::string caracteresParasitants)
{
  // Recherche de la position du dernier caractere non parasitant
  std::size_t positionDernierNonParasitant = chaineADeparasiter->find_last_not_of(caracteresParasitants);

  // Si un caractere non parasitant existe
  if (std::string::npos != positionDernierNonParasitant)
  {
    // Suppression des caracteres suivants
    chaineADeparasiter->erase(positionDernierNonParasitant + 1);
  }
  // S'il n'y a que des caracteres parasitants
  else
  {
    // Chaine vide
    chaineADeparasiter->clear();
  }
}

void deparasiterDebut(std::string* chaineADeparasiter, std::string caracteresParasitants)
{
  // Recherche de la position du dernier caractere non parasitant
  std::size_t positionPremierNonParasitant = chaineADeparasiter->find_first_not_of(caracteresParasitants);

  // Si un caractere non parasitant existe
  if (std::string::npos != positionPremierNonParasitant)
  {
    // Suppression des caracteres precedants
    chaineADeparasiter->erase(0, positionPremierNonParasitant);
  }
  // S'il n'y a que des caracteres parasitants
  else
  {
    // Chaine vide
    chaineADeparasiter->clear();
  }
}

void deparasiterDebutEtFin(std::string* chaineADeparasiter, std::string caracteresParasitants)
{
  // Deparasitage du debut
  deparasiterDebut(chaineADeparasiter, caracteresParasitants);

  // Deparasitage de la fin
  deparasiterFin(chaineADeparasiter, caracteresParasitants);
}

std::string cheminFinal(std::string cheminDeBase, std::string cheminRelatif)
{
  std::string cheminFinal;

  // Remplace les '\' par des '/'
  std::replace( cheminDeBase.begin(), cheminDeBase.end(), '\\', '/');
  std::replace( cheminRelatif.begin(), cheminRelatif.end(), '\\', '/');

  if (0 == cheminRelatif.size())
  {
    return "";
  }

  // S'il s'agit d'un chemin absolu
  if ("/" == cheminRelatif.substr(0, 1))
  {
    // Retourne ce chemin comme etant le chemin final
    return cheminRelatif;
  }

  // Si le chemin de base est donne par un fichier
  if ( (0 < cheminDeBase.size()) && ('/' != *cheminDeBase.end()))
  {
    // Calcule du dossier qui le contient

    unsigned int found = cheminDeBase.find_last_of("/");
    if (found != std::string::npos)
    {
      cheminDeBase = cheminDeBase.substr(0, found+1);
    }
    else
    {
      cheminDeBase = "";
    }
  }

  // Concatenation du chemin relatif au chemin de base
  cheminFinal = cheminDeBase + cheminRelatif;

  // Mise en tableau de chaque element du chemin
  std::vector<std::string> elements;
  std::stringstream stream(cheminFinal);
  std::string item;
  while (std::getline(stream, item, '/'))
    elements.push_back(item);

  // simplification du chemin final
  cheminFinal = "";
  for (std::vector<std::string>::iterator it = elements.begin(); it != elements.end(); ++it)
  {
    // Si c'est un point, on ne fait rien
    if ("." == *it) continue;

    // Si c'est un dossier parent
    if (".." == *it)
    {
      // Suppression du dernier dossier ajoute
      if (std::string::npos != cheminFinal.find("/")) // TODO : relatif au dessus a gerer (car ici on brid juste)
      {
        cheminFinal.resize(cheminFinal.size()-1);
        unsigned int found = cheminFinal.find_last_of("/");
        cheminFinal = cheminFinal.substr(0, found+1);
      }

      continue;
    }

    // Ajout du dossier
    cheminFinal += *it + "/";
  }

  // Suppression du '/' final
  cheminFinal.resize(cheminFinal.size()-1);

  return cheminFinal;
}
