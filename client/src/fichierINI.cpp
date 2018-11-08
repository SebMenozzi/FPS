#include "fichierINI.h"
#include <fstream>
#include <sstream>

bool8 FichierINI::charger(std::string nomFichier)
{
  // Supprime les eventuelles variables deja en memoire
  this->vider();

  // Ouvre le fichier en lecture
  std::ifstream fichier(nomFichier.c_str(), std::ios::in);

  // Si l'ouverture a echoue
  if(! fichier)
  {
    // Rien n'est charge
    return FALSE;
  }

  // Pour chaque ligne
  std::string ligne;
  while(getline(fichier, ligne))
  {
    // Creation d'un flux d'extraction depuis cette ligne
    std::istringstream flux(ligne);

    // Recupere le nom de la variable jusqu'au '='
    std::string nom;
    std::getline(flux, nom, '=');

    // Recupere tout le reste, la valeur
    std::string valeur;
    std::getline(flux, valeur);

    // Ajout de la variable a la liste en memoire
    this->listeVariables[nom] = valeur;
  }

  return TRUE;
}

bool8 FichierINI::enregistrer(std::string nomFichier)
{
  // Ouvre / Cree le fichier en ecriture
  std::ofstream fichier(nomFichier.c_str(), std::ios::out | std::ios::trunc);

  // Si l'ouverture a echoue
  if(! fichier)
  {
    // Rien n'est enregistre
    return FALSE;
  }

  // Ajout des variables au fichier
  for (std::map<std::string,std::string>::iterator it = this->listeVariables.begin(); it != this->listeVariables.end(); it++)
  {
    fichier << it->first << "=" << it->second << '\n';
  }

  // Fermeture du fichier
  fichier.close();

  return TRUE;
}

void FichierINI::modifier(std::string nom, std::string valeur)
{
  // Modifie en memoire la variable
  // Si elle n'existe pas, elle sera ajoutee
  this->listeVariables[nom] = valeur;
}

void FichierINI::supprimer(std::string nom)
{
  // Supprime en memoire la variable nom
  this->listeVariables.erase(nom);
}

std::string FichierINI::lire(std::string nom)
{
  // Retourne la valeur de la variable
  // Si elle n'existe pas, retourne une chaine vide
  return this->listeVariables[nom];
}

void FichierINI::vider(void)
{
  // Supprime en memoire toutes les variables
  this->listeVariables.clear();
}
