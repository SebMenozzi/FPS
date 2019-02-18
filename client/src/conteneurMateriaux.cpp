#include "conteneurMateriaux.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "traitementChemin.h"

void ConteneurMateriaux::ajouterBibliotheque(std::string cheminBibliotheque)
{
  // Ouvre le fichier en lecture
  std::ifstream fichier(cheminBibliotheque.c_str(), std::ios::in);

  // Si l'ouverture a echoue
  if(!fichier)
  {
    // Rien n'est charge
    std::cout << "Impossible d'ouvrir la bibliotheque de materiaux : " << cheminBibliotheque << std::endl;
    return;
  }

  Materiau* materiauCourant = NULL;

  // Pour chaque ligne
  std::string ligne;
  while(getline(fichier, ligne))
  {
    // Creation d'un flux d'extraction pour cette ligne
    std::istringstream flux(ligne);

    // Lecture du tag
    std::string tag;
    flux >> tag;

    // Recupere tout le reste, les parametres
    std::string parametres;
    std::getline(flux, parametres);
    deparasiterDebutEtFin(&parametres, " \t\r\n");

    if ("newmtl" == tag) // Creation d'un nouveau materiau ...
    {
      // ... qui devient le materiau courant
      materiauCourant = &this->_listeMateriaux[parametres];
      materiauCourant->cheminDeLaBibliotheque(cheminBibliotheque);
    }

    // TODO : Ne pas tapper dans un materiaux s'il n'est pas esistant
    // Faire un ELSE general dans lequel faire le test.

    else if ("Ns" == tag) // Brillance
    {
      // Creation d'un flux d'extraction
      std::istringstream flux(parametres);

      // Lecture de la brillance
      float16 brillance = 0.0;
      flux >> brillance;

      // Bride la valeur de la brillance dans la plage autorisee [0.0 .. 1000.0]
      #define BRILLANCE_MAX 1000.0
      if (0.0 > brillance) brillance = 0.0;
      if (BRILLANCE_MAX < brillance) brillance = BRILLANCE_MAX;

      // Definition de la brillance au "format OpenGL" [0.0 .. 128.0]
      materiauCourant->Ns(brillance / BRILLANCE_MAX * 128.0);
    }
    else if ("Ni" == tag)
    {
      std::cout << "La refraction n'est pas prise en charge" << std::endl;
    }
    else if ("Kd" == tag) // Couleur diffuse
    {
      // Creation d'un flux d'extraction
      std::istringstream flux(parametres);
      Color3Float couleurDiffuse;
      flux >> couleurDiffuse.r;
      flux >> couleurDiffuse.v;
      flux >> couleurDiffuse.b;
      materiauCourant->Kd(couleurDiffuse);
    }
    else if ("Ks" == tag) // Couleur speculaire
    {
      // Creation d'un flux d'extraction
      std::istringstream flux(parametres);
      Color3Float couleurSpeculaire;
      flux >> couleurSpeculaire.r;
      flux >> couleurSpeculaire.v;
      flux >> couleurSpeculaire.b;
      materiauCourant->Ks(couleurSpeculaire);
    }
    else if ("Ka" == tag) // Couleur ambiante
    {
      // Creation d'un flux d'extraction
      std::istringstream flux(parametres);
      Color3Float couleurAmbiante;
      flux >> couleurAmbiante.r;
      flux >> couleurAmbiante.v;
      flux >> couleurAmbiante.b;
      materiauCourant->Ka(couleurAmbiante);
    }
    else if ("d" == tag) // Transparence
    {
      // Creation d'un flux d'extraction
      std::istringstream flux(parametres);

      // Lecture de la transparence
      float16 transparence = 1.0;
      flux >> transparence;

      // Bride la valeur de la transparence dans la plage autorisee [0.0 .. 1.0]
      // 0.0 = transparent    1.0 = opaque
      if (0.0 > transparence) transparence = 0.0;
      if (1.0 < transparence) transparence = 1.0;

      // Definition de la transparence
      materiauCourant->d(transparence);
    }
    else if ("illum" == tag) // Modele d'eclairage
    {
      // Creation d'un flux d'extraction
      std::istringstream flux(parametres);

      // Lecture du modele d'eclairage
      uint32 modeleEclairage = 2;
      flux >> modeleEclairage;

      // Bride le numero a la page autorisee [0 .. 10]
      if (10 < modeleEclairage) modeleEclairage = 2;

      // Definition du numero du modele d'eclairage
      materiauCourant->illum(modeleEclairage);
    }
    else if ("map_Kd" == tag) // Texture diffuse
    {
      // TODO : Verifier le parametre
      // http://paulbourke.net/dataformats/mtl/


      //std::string cheminImage = cheminFinal(nomFichier, parametres);

      //materiauCourant->_conteneurTexture.ajouter(cheminImage, TRUE);
      materiauCourant->map_Kd(parametres);
    }
  }
}

Materiau* ConteneurMateriaux::materiau(std::string nom)
{
  //std::cout << "nom " << nom << std::endl;

  // Si le materiau demande n'existe pas
  if (this->_listeMateriaux.find(nom) == this->_listeMateriaux.end())
  {
    // Retourne un materiau par defaut
    return &this->materiauParDefaut;
  }

  // Retourne le materiau demande
  return &this->_listeMateriaux[nom];
}
