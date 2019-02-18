#include "objet3D.h"
#include <fstream>
#include <sstream>
#include "point3Float.h"
#include <iostream>
#include "point2Float.h"
#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif
#include "types.h"
#include <cmath>
#include <cstdlib>
#include "traitementChemin.h"

Objet3D::Objet3D(std::string cheminObjet)
{
  this->_cheminObjet = cheminObjet;
  this->_listeAffichage = 0;
  this->_charge = FALSE;

  // Ouvre le fichier OBJ en lecture seule
  std::ifstream fichierOBJ(this->_cheminObjet.c_str(), std::ios::in);

  // Si l'ouverture a echoue
  if(!fichierOBJ)
  {
    // Rien n'est charge
    std::cout << "Impossible d'ouvrir l'objet 3D : " << this->_cheminObjet << std::endl;
    return;
  }

  // Chargement des bibliotheques de materiaux avant la creation de la liste d'affichage
  std::string ligne;
  while(getline(fichierOBJ, ligne))
  {
    this->chargerBibliothequesMateriaux(ligne);
  }

  // Replace le curseur au debut du fichier
  fichierOBJ.clear();
  fichierOBJ.seekg(0, std::ios::beg);

  // Creation des premiers elements pour commencer les vraies listes a [1]
  this->ajouterPointTridimensionnel("0 0 0");
  this->ajouterPointTexture("0 0");
  this->ajouterNormale("0 0 0");

  // Debut de la creation de la liste d'affichage
  this->_listeAffichage = glGenLists(1);
  glNewList(this->_listeAffichage, GL_COMPILE);
  {
    // Au debut, selection du materiau par defaut
    this->selectionnerMateriau("");

    while(getline(fichierOBJ, ligne))
    {
      // Creation d'un flux d'extraction pour cette ligne
      std::istringstream flux(ligne);

      // Lecture du tag
      std::string tag;
      flux >> tag;

      // Recupere les parametres
      std::string parametres;
      std::getline(flux, parametres);
      deparasiterDebutEtFin(&parametres, " \t\r\n");

      // Selon le tag rencontre, traitement des parametres
      if      ("v"      == tag) this->ajouterPointTridimensionnel(parametres);
      else if ("vt"     == tag) this->ajouterPointTexture(parametres);
      else if ("vn"     == tag) this->ajouterNormale(parametres);
      else if ("f"      == tag) this->ajouterPolygone(parametres);
      else if ("usemtl" == tag) this->selectionnerMateriau(parametres);
      else if ("mtllib" == tag) ; // Les bibliotheques sont deja chargees
    }
      // Si le dernier materiaux etait transparent, il faut remettre opaque, activer le depth test
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  // Fin de la creation de la liste d'affichage
  glEndList();

  // Chargement termine
  this->_charge = TRUE;
}

void Objet3D::ajouterPointTridimensionnel(std::string parametres)
{
  // Creation d'un flux d'extraction
  std::istringstream flux(parametres);

  // Construction du point tridimensionnel
  Point3Float pointTridimensionnel;
  flux >> pointTridimensionnel.x;
  flux >> pointTridimensionnel.y;
  flux >> pointTridimensionnel.z;

  // Ajout du point a la liste des points tridimensionnel
  this->_listePointsTridimensionnels.push_back(pointTridimensionnel);
}

void Objet3D::ajouterPointTexture(std::string parametres)
{
  // Creation d'un flux d'extraction
  std::istringstream flux(parametres);

  // Construction du point de texture
  Point2Float pointTexture;
  flux >> pointTexture.x;
  flux >> pointTexture.y;

  // Ajout du point a la liste des points de textures
  this->_listePointsTextures.push_back(pointTexture);
}

void Objet3D::ajouterPolygone(std::string parametres)
{
  // Creation d'un flux d'extraction
  std::istringstream flux(parametres);

  // Les points du polygone
  std::vector<std::string> pointsDuPolygone;
  while(TRUE)
  {
    // Lecture d'un eventuel point
    std::string nouveauPoint;
    flux >> nouveauPoint;

    // S'il un nouveau point a ete rencontre
    if ("" == nouveauPoint)
    {
      break;
    }

    // Ajout du nouveau point a ce polygone
    pointsDuPolygone.push_back(nouveauPoint);
  }

  // Liste des indexs des points tridimentionnels, des
  // points de texture et des normales pour ce polygone
  std::vector<long> v;
  std::vector<long> t;
  std::vector<long> n;

  // Pour chacun des points lecture de :
  //  - l'index de sa position tridimentionnelle
  //  - l'index de sa position sur la texture
  //  - l'index de sa normale
  for(uint32 i = 0; i < pointsDuPolygone.size(); i++)
  {
    // Variables temporaires pour les tests d'extractions formatees
    long v_tmp;
    long t_tmp;
    long n_tmp;

    // Test d'extraction au format "v/vt/vn"
    if (sscanf(pointsDuPolygone[i].c_str(), "%ld/%ld/%ld", &v_tmp, &t_tmp, &n_tmp) == 3)
    {
      v.push_back(v_tmp);
      t.push_back(t_tmp);
      n.push_back(n_tmp);

      continue;
    }

    // Test d'extraction au format "v/vt"
    if (sscanf(pointsDuPolygone[i].c_str(), "%ld/%ld", &v_tmp, &t_tmp) == 2)
    {
      v.push_back(v_tmp);
      t.push_back(t_tmp);
      n.push_back(0); // Normale inconnue

      continue;
    }

    // Test d'extraction au format "v//vn"
    if (sscanf(pointsDuPolygone[i].c_str(), "%ld//%ld", &v_tmp, &n_tmp) == 2)
    {
      v.push_back(v_tmp);
      t.push_back(0); // Point de texture inconnue
      n.push_back(n_tmp);

      continue;
    }

    // Test d'extraction au format "v"
    if (sscanf(pointsDuPolygone[i].c_str(), "%ld", &v_tmp) == 1)
    {
      v.push_back(v_tmp);
      t.push_back(0); // Point de texture inconnue
      n.push_back(0); // Normale inconnue

      continue;
    }
  }

  // Detection des anomalies

  // S'il n'y a pas au moins 3 points
  if (pointsDuPolygone.size() < 3) return;

  // Si un des points n'a pas repondu au format attendu
  if (pointsDuPolygone.size() != v.size()) return;
  if (pointsDuPolygone.size() != t.size()) return;
  if (pointsDuPolygone.size() != n.size()) return;

  // Pour chacun des points
  for(uint32 i = 0; i < pointsDuPolygone.size(); i++)
  {
    // Si le point ou la normale n'existe pas, on ne dessine pas le polygone
    if (abs(v[i]) >= this->_listePointsTridimensionnels.size()) return;
    if (abs(t[i]) >= this->_listePointsTextures.size()) return;
    if (abs(n[i]) >= this->_listeNormales.size()) return;
    if (v[i] == 0) return;

    // Si les points ne sont pas renseignes de facon identique
    if (0 < i)
    {
      if (0 == v[i] && 0 != v[0]) return;
      if (0 == v[0] && 0 != v[i]) return;
      if (0 == t[i] && 0 != t[0]) return;
      if (0 == t[0] && 0 != t[i]) return;
      if (0 == n[i] && 0 != n[0]) return;
      if (0 == n[0] && 0 != n[i]) return;
    }
  }

  // Fin de detection des anomalies

  // Pour chacun des points
  for(uint32 i = 0; i < pointsDuPolygone.size(); i++)
  {
    // Pour les points renseignes en position relative
    if (0 > v[i])
    {
      // Modification pour enseigner la position absolue
      v[i] = this->_listePointsTridimensionnels.size() + v[i];
    }

    // Idem pour les normales et les coordonnees de texture
    if (0 > n[i]) n[i] = this->_listeNormales.size() + n[i];
    if (0 > t[i]) t[i] = this->_listePointsTridimensionnels.size() + t[i];
  }

  // Si les normales ne sont pas renseignees
  float nx = 0, ny = 0, nz = 0;
  if(0 == n[0])
  {
    // Calcul d'une normale generale pour tout les points a partir des 3 premiers points

    // Calcul de deux vecteurs
    float v1x = this->_listePointsTridimensionnels[v[1]].x - this->_listePointsTridimensionnels[v[0]].x;
    float v1y = this->_listePointsTridimensionnels[v[1]].y - this->_listePointsTridimensionnels[v[0]].y;
    float v1z = this->_listePointsTridimensionnels[v[1]].z - this->_listePointsTridimensionnels[v[0]].z;
    float v2x = this->_listePointsTridimensionnels[v[2]].x - this->_listePointsTridimensionnels[v[0]].x;
    float v2y = this->_listePointsTridimensionnels[v[2]].y - this->_listePointsTridimensionnels[v[0]].y;
    float v2z = this->_listePointsTridimensionnels[v[2]].z - this->_listePointsTridimensionnels[v[0]].z;

    // Calcule de la normale
    nx = v1y*v2z-v1z*v2y;
    ny = v1z*v2x-v1x*v2z;
    nz = v1x*v2y-v1y*v2x;

    // Calcule de la norme
    float norme = sqrt(nx*nx+ny*ny+nz*nz);

    // Normalisation de la normale
    nx /= norme;
    ny /= norme;
    nz /= norme;
  }

  // Dessin du polygone
  glBegin(GL_POLYGON);

  // Pour chaque point du polygone
  for(uint32 i = 0; i < pointsDuPolygone.size(); i++)
  {
    // Si les normales sont renseignees
    if(0 != n[0])
    {
      // Utilisation de la normale renseigne
      glNormal3f(this->_listeNormales[n[i]].x, this->_listeNormales[n[i]].y, this->_listeNormales[n[i]].z);
    }
    else
    {
      // Utilisation d'une normale generale pour ce polygone
      glNormal3f(nx, ny, nz);
    }

    // Si la texture est renseignee
    if(0 != t[0])
    {
      // Utilisation de la texture
      glTexCoord2f(this->_listePointsTextures[t[i]].x,this->_listePointsTextures[t[i]].y);
    }

    // Positionnement du point tridimensionnel
    glVertex3f( this->_listePointsTridimensionnels[v[i]].x,
                this->_listePointsTridimensionnels[v[i]].y,
                this->_listePointsTridimensionnels[v[i]].z );
  }
  glEnd();
}

void Objet3D::chargerBibliothequesMateriaux(std::string ligne)
{
  // Creation d'un flux d'extraction pour cette ligne
  std::istringstream flux(ligne);

  // Lecture du tag
  std::string tag;
  flux >> tag;

  // S'il s'agit d'un tag designant des bibliotheques de materiaux
  if ("mtllib" == tag)
  {
    // Lecture de la liste des fichiers MTL
    std::string cheminsFichiersMtl;
    std::getline(flux, cheminsFichiersMtl);

    // Extraction de la liste des chemins des fichiers MTL
    std::vector<std::string> listeCheminsFichiersMtl = this->fractionnerFichiers(cheminsFichiersMtl, ".mtl");

    // Pour chaque fichier MTL
    for(uint32 i = 0; i < listeCheminsFichiersMtl.size(); i++)
    {
      // Calcule du chemin relatif au fichier OBJ
      std::string cheminFichierMtl = cheminFinal(this->_cheminObjet, listeCheminsFichiersMtl[i]);

      // Ajout de la bibliotheque de materiaux
      this->_conteneurMateriaux.ajouterBibliotheque(cheminFichierMtl);
    }
  }
}

void Objet3D::afficher(void)
{
  // Si l'objet est bien charge
  if (TRUE == this->_charge)
  {
    // Affichage
    glCallList(this->_listeAffichage);
  }
}

void Objet3D::selectionnerMateriau(std::string nomMateriau)
{
  // Selection du materiau demande
  this->_conteneurMateriaux.materiau(nomMateriau)->selectionner();
}

void Objet3D::ajouterNormale(std::string parametres)
{
  // Creation d'un flux d'extraction
  std::istringstream flux(parametres);

  // Construction de la normale
  Point3Float normale;
  flux >> normale.x;
  flux >> normale.y;
  flux >> normale.z;

  // Ajout de la normale a la liste des normales
  this->_listeNormales.push_back(normale);
}

std::vector<std::string> Objet3D::fractionnerFichiers(std::string listeFichiers, std::string extension)
{
  // Si l'extension n'est pas renseigne, on ne fait rien
  if (0 == extension.size())
  {
    std::vector<std::string> vecteurVide;
    return vecteurVide;
  }

  // Mise en minuscule des noms de fichiers
  for(uint32 i = 0; i < listeFichiers.size(); i++)
  {
    if ('A' <= listeFichiers[i] && 'Z' >= listeFichiers[i])
    {
      listeFichiers[i] = listeFichiers[i] + 'a' - 'A';
    }
  }

  // Mise en minuscule de l'extension
  for(uint32 i = 0; i < extension.size(); i++)
  {
    if ('A' <= extension[i] && 'Z' >= extension[i])
    {
      extension[i] = extension[i] + 'a' - 'A';
    }
  }

  // Si l'extension ne comporte pas de point au debut
  if ('.' != extension[0])
  {
    // Ajout du point
    extension = "." + extension;
  }

  std::vector<std::string> listeFinale;

  while(TRUE)
  {
    size_t pos = listeFichiers.find(extension + " ");
    if (std::string::npos == pos)
    {
      break;
    }

    // Recuperation / ajout
    std::string fichierCourant("");
    fichierCourant = listeFichiers.substr(0, pos+extension.size());
    listeFinale.push_back(fichierCourant);

    // Fichier traite, suppression
    listeFichiers = listeFichiers.substr(pos+extension.size()+1);
  }

  // Si le reste se termine par ".obj", ajout au vecteur
  if ( 4 <= listeFichiers.size() && extension == listeFichiers.substr(listeFichiers.size()-extension.size()) )
  {
    listeFinale.push_back(listeFichiers);
  }

  // Deparasitage de la liste
  for(uint32 i = 0; i < listeFinale.size(); i++)
  {
    deparasiterDebut(&listeFinale[i], " \t\r\n");
  }

  return listeFinale;
}
