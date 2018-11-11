#ifndef PERSONNAGE_H_INCLUDED
#define PERSONNAGE_H_INCLUDED

#include "objet3DStatique.h"
#include <string>
#include "historiqueDesPositions.h"
#include "point3Float.h"

#include "objet3D.h"

class Personnage : public Objet3DStatique
{
  private:
    float16 rayon;
    float16 angleVertical;

    HistoriqueDesPositions historiqueDesPositions;

    sint32 m_heureReapparition;
    Point3Float m_pointReapparition;
    float16 m_angleReapparition;

    uint32 nbTirGagnant;
    uint32 nbTirRecu;

    std::string m_pseudo;

    Objet3D* objet3D;

  public:
    Personnage(float16 positionX, float16 positionY, float16 positionZ, float16 angleHorizontal, float16 angleVertical, float16 rayon, std::string nomFichier);
    ~Personnage(void);

    // Deplacement
    void deplacer(float16 distance, float16 direction, bool8 entourage[8], float16 hauteurHeightmap);
    void tournerHorizontalement(float16 angle);
    void tournerVerticalement(float16 angle);
    void goUp();
    void goDown();

    void positionSurLaCarte(float16* x, float16* y, float16* z);
    void regarder(void);

    // Positionne le personnage
    void positionner(float16 x, float16 y, float16 z);
    void orienter(float16 angleHorizontal, float16 angleVertical);

    // Lire la position du personnage
    float16 lirePositionX();
    float16 lirePositionY();
    float16 lirePositionZ();
    float16 lireAngleHorizontal();
    float16 lireAngleVertical();

    sint32 heureReapparition(void);
    void heureReapparition(sint32 heure);

    Point3Float pointReapparition(void);
    void pointReapparition(Point3Float point);

    float16 angleReapparition(void);
    void angleReapparition(float16 angle);

    void ajouterPosition(sint32 heure, float16 x, float16 y, float z, float16 angleHorizontal, float16 angleVertical);
    bool8 positionnerDepuisLHistorique(sint32 heure);
    //bool8 heureDernierePositionConnue(sint32* heure);

    void afficherHistorique(void);

    uint32 emis();
    uint32 recu();
    void mettreAJourScore(uint32 emis, uint32 recu);

    void pseudo(std::string pseudo);
    std::string pseudo(void);

    void dessiner();
};

#endif // PERSONNAGE_H_INCLUDED
