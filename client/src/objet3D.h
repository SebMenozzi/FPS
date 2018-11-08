#ifndef OBJET3D_H_INCLUDED
#define OBJET3D_H_INCLUDED

#include <string>
#include <vector>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif
#include "point3Float.h"
#include "point2Float.h"
#include "conteneurMateriaux.h"

class Objet3D
{
  public:
    // Constructeur de l'objet tridimensionnel dans le contexte OpenGL
    Objet3D(std::string cheminObjet);

    // Affiche l'objet tridimensionnel dans le contexte OpenGL
    void afficher(void);

  private:
    // Traitement des tags v, vt, vn et f
    void ajouterPointTridimensionnel(std::string parametres);
    void ajouterPointTexture(std::string parametres);
    void ajouterNormale(std::string parametres);
    void ajouterPolygone(std::string parametres);

    // Traitement des tags mtllib et usemtl
    void chargerBibliothequesMateriaux(std::string ligne);
    void selectionnerMateriau(std::string nomMateriau);

    // Mise en vecteur d'une liste de fichiers portant la meme extension (cas du tag mtllib)
    std::vector<std::string> fractionnerFichiers(std::string listeFichiers, std::string extension);

    // Liste des points tridimensionnels de l'objet
    std::vector<Point3Float> _listePointsTridimensionnels;

    // Liste des normales qui definissent l'orientation des polygones
    std::vector<Point3Float> _listeNormales;

    // Liste des points remarquables sur les textures
    std::vector<Point2Float> _listePointsTextures;

    // Le conteneur des materiaux
    ConteneurMateriaux _conteneurMateriaux;

    // La liste d'affichage
    GLuint _listeAffichage;

    // Le chemin du fichier OBJ
    std::string _cheminObjet;

    bool8 _charge;
};

#endif // OBJET3D_H_INCLUDED
