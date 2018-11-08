#include "personnage.h"
#include <math.h>

#ifndef INT_MIN
  #define INT_MIN ((int)0x80000000)
#endif

Personnage::Personnage(float16 positionX, float16 positionY, float16 positionZ, float16 angleHorizontal, std::string nomFichier) : Objet3DStatique(nomFichier)
{
  this->positionX = positionX;
  this->positionY = positionY;
  this->positionZ = positionZ;
  this->angleHorizontal = angleHorizontal;
  this->rayon = 0.3f;
  this->angleVertical = 0.0f;

  this->m_heureReapparition = INT_MIN;

  this->nbTirGagnant = 0;
  this->nbTirRecu = 0;

  this->m_pseudo = "x";

  this->objet3D = new Objet3D("Dragon/dragon.obj");
}

Personnage::~Personnage(void)
{
  delete this->objet3D;
}

void Personnage::deplacer(float16 distance, float16 direction, bool8 entourage[8])
{
  const float EPSILON = 0.001f;

  if (EPSILON >= distance) return;

  // Bridage de la distance pour eviter la perte de collision (a calculer et verifier)
  float16 distanceMax = this->rayon - EPSILON;

  // Calcule des segments pour ce deplacement
  /*unsigned long nbGrandsSegments = (unsigned long) (distance / distanceMax);
  float distancePetitSegment = fmod(distance, distanceMax);

  // Pour chaque grand segment
  for(uint32 i = 0; i < nbGrandsSegments; i++)
  {
      // Deplacement d'un peu moin d'un grand segment
      this->deplacer(distanceMax - EPSILON, direction, entourage);
  }

  // Distance restante
  distance = distancePetitSegment;*/

  if (distance > distanceMax)
  {
    distance = distanceMax;
  }

  direction += this->angleHorizontal;

  // On evite que l'azimut du personnage approche un multiple de 90
  // Cela evite des soucis lors de la gestion des collisions
  const float MODULO = 90.0f;
  float reste = fmod(direction, MODULO);

  if (this->angleHorizontal >= 0.0f) // Angle positif ou nul
  {
    if (reste < EPSILON) // 0+
    {
      direction += EPSILON;
    }
    else if (reste > MODULO - EPSILON) // 90-
    {
      direction -= EPSILON;
    }
  }
  else // Angle negatif
  {
    if (reste > -EPSILON) // 0-
    {
      direction -= EPSILON;
    }
    else if (reste > -MODULO + EPSILON) // (-90)+
    {
      direction += EPSILON;
    }
  }

  // Calcule de la position cible du personnage
  float16 positionCibleY = this->positionY - distance * cos(direction * M_PI / 180.0);
  float16 positionCibleX = this->positionX - distance * sin(direction * M_PI / 180.0);
  //float16 positionCibleY = this->positionY - distance * sin(direction * M_PI / 180.0);
  //float16 positionCibleX = this->positionX - distance * cos(direction * M_PI / 180.0);

  // GESTION DES COLLISIONS AVEC MUR EST

  // S'il y a un mur a l'Est
  // et que la droite du personnage arrive dans le mur
  if( (1 == entourage[4]) && ceil(positionCibleX + this->rayon) != ceil(this->positionX) )
  {
    // On rectifie la translation en Y (3D)
    positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
  }

  // GESTION DES COLLISIONS AVEC MUR SUD

  // S'il y a un mur au Sud et que notre Sud arrive dans le mur
  if (1 == entourage[6] && ceil(positionCibleY + this->rayon) != ceil(this->positionY))
  {
    // On rectifie la translation en X (3D)
    positionCibleY -= (positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon);
  }

  // GESTION DES COLLISIONS AVEC MUR OUEST

  // S'il y a un mur a l'Ouest et que notre Ouest arrive dans le mur
  if (1 == entourage[3] && floor(positionCibleX - this->rayon) != floor(this->positionX))
  {
    // On rectifie la translation en Y (3D)
    positionCibleX -= (positionCibleX - this->rayon) - (sint32)(positionCibleX + this->rayon);
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD

  // S'il y a un mur au Nord et que notre Nord arrive dans le mur
  if (1 == entourage[1] && floor(positionCibleY - this->rayon) != floor(this->positionY))
  {
    // On rectifie la translation en X (3D)
    positionCibleY -= (positionCibleY - this->rayon) - (sint32)(positionCibleY + this->rayon);
  }


  // GESTION DES COLLISIONS AVEC MUR AU SUD-EST
  if (1 == entourage[7]
      && ceil(positionCibleY + this->rayon) != ceil(positionY)
      && ceil(positionCibleX + this->rayon) != ceil(positionX)
     )
  {
    if (positionCibleY > this->positionY && positionCibleX > this->positionX) // Approche
    {
      // Evite la division par zero
      if (((positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon)) == 0.0 || (positionCibleX - this->positionX) == 0.0)
      {
        positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
      }
      else
      {
        // Si ce sont les bord en Y qui ont �t� en contact lors de la collision
        if ( ((positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon)) / ((positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon)) > (positionCibleY - this->positionY) / (positionCibleX - this->positionX))
        {
          positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
        }
        else
        {
          positionCibleY -= (positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon);
        }
      }
    }
    else if (positionCibleY < this->positionY) // Eloigne en X
    {
      positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
    }
    else if (positionCibleX < this->positionX) // Eloigne en Y
    {
      positionCibleY -= (positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon);
      }
  }

  // GESTION DES COLLISIONS AVEC MUR AU SUD-OUEST
  if (1 == entourage[5]
      && ceil(positionCibleY + this->rayon) != ceil(positionY)
      && floor(positionCibleX - this->rayon) != floor(positionX)
     )
  {
    if (positionCibleY < this->positionY) // Eloigne en X
    {
      positionCibleX += (sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon);
    }
    else if (positionCibleX > this->positionX) // Eloigne en Y
    {
      positionCibleY -= (positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon);
    }
    else if (positionCibleY > this->positionY && positionCibleX < this->positionX) // Approche
    {
      if(((sint32)(positionCibleX - this->rayon + 1) - (positionCibleX - this->rayon)) == 0.0 || (this->positionX - positionCibleX) == 0.0)
      {
          positionCibleX += (sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( ((positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon)) / ((sint32)(positionCibleX - this->rayon + 1) - (positionCibleX - this->rayon)) > (positionCibleY - this->positionY) / (this->positionX - positionCibleX))
        {
          positionCibleX += (sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon);
        }
        else
        {
          positionCibleY -= (positionCibleY + this->rayon) - (sint32)(positionCibleY + this->rayon);
        }
      }
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD-OUEST
  if (1 == entourage[0]
      && floor(positionCibleY - this->rayon) != floor(positionY)
      && floor(positionCibleX - this->rayon) != floor(positionX)
     )
  {
    if (positionCibleY > this->positionY) // Eloigne en X
    {
      positionCibleX += (sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon);
    }
    else if (positionCibleX > this->positionX) // Eloigne en Y
    {
      positionCibleY += (sint32)((positionCibleY - this->rayon) + 1) - (positionCibleY - this->rayon);
    }
    else if (positionCibleY < this->positionY && positionCibleX < this->positionX) // Approche
    {
      // Evite la division par zero
      if (((sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon)) == 0.0 || (this->positionX - positionCibleX) == 0.0)
      {
        positionCibleX += (sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( ((sint32)((positionCibleY - this->rayon) + 1) - (positionCibleY - this->rayon)) / ((sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon)) > (this->positionY - positionCibleY) / (this->positionX - positionCibleX))
        {
          positionCibleX += (sint32)((positionCibleX - this->rayon) + 1) - (positionCibleX - this->rayon);
        }
        else
        {
          positionCibleY += (sint32)((positionCibleY - this->rayon) + 1) - (positionCibleY - this->rayon);
        }
      }
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD-EST
  if (1 == entourage[2] &&
      floor(positionCibleY - this->rayon) != floor(positionY) &&
      ceil(positionCibleX + this->rayon) != ceil(positionX))
  {
    if (positionCibleY > this->positionY) // Eloigne en X
    {
      positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
    }
    else if (positionCibleX < this->positionX) // Eloigne en Y
    {
      positionCibleY += (sint32)((positionCibleY - this->rayon) + 1) - (positionCibleY - this->rayon);
    }
    else if (positionCibleY < this->positionY && positionCibleX > this->positionX) // Approche
    {
      if ((((positionCibleX + this->rayon)) - (sint32)(positionCibleX + this->rayon)) == 0.0 || (this->positionX - positionCibleX) == 0.0)
      {
        positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( (((positionCibleY - this->rayon)) - (sint32)((positionCibleY - this->rayon) + 1)) / (((positionCibleX + this->rayon)) - (sint32)(positionCibleX + this->rayon)) < (this->positionY - positionCibleY) / (this->positionX - positionCibleX))
        {
          positionCibleX -= (positionCibleX + this->rayon) - (sint32)(positionCibleX + this->rayon);
        }
        else
        {
          positionCibleY += (sint32)((positionCibleY - this->rayon) + 1) - (positionCibleY - this->rayon);
        }
      }
    }
  }

  this->positionY = positionCibleY;
  this->positionX = positionCibleX;
}

/*
void Personnage::gauche(float16 distance, bool8 entourage[8])
{
  this->avancer(distance, 90.0, entourage);
}

void Personnage::droite(float16 distance, bool8 entourage[8])
{
  this->avancer(distance, -90.0, entourage);
}
*/

void Personnage::tournerHorizontalement(float16 angle)
{
  this->angleHorizontal += angle;

  while (this->angleHorizontal >= 180.0f) // Lorsqu'on depasse la limite (1/2 tour)
  {
    this->angleHorizontal -= 360.0f;
  }
  while (this->angleHorizontal < -180.0f) // Idem apres 1/2 tours vers la droite
  {
    this->angleHorizontal += 360.0f;
  }
}

void Personnage::tournerVerticalement(float16 angle)
{
  this->angleVertical += angle;

  if (45.0f < this->angleVertical)
  {
    this->angleVertical = 45.0f;
  }
  else if (-45.0f > this->angleVertical)
  {
    this->angleVertical = -45.0f;
  }
}

void Personnage::positionSurLaCarte(sint32* x, sint32* y)
{
  // Recupere la position du personnage sur la carte
  *x = (sint32)this->positionX;
  *y = (sint32)this->positionY;
}

void Personnage::regarder(void)
{
  #define RADIANS_PAR_DEGRES 0.0174532925199
  #define HAUTEUR_OEIL_PERSONNAGE 0.55

  gluLookAt(
    // Position de l'oeil
    this->positionY, this->positionX, HAUTEUR_OEIL_PERSONNAGE,

    // Point vise
    this->positionY - cos(-this->angleHorizontal * RADIANS_PAR_DEGRES),
    this->positionX + sin(-this->angleHorizontal * RADIANS_PAR_DEGRES),
    //this->positionX - cos(-angleHorizontal * RADIANS_PAR_DEGRES),
    //this->positionY + sin(-this->angleHorizontal * RADIANS_PAR_DEGRES),
    HAUTEUR_OEIL_PERSONNAGE + tan(this->angleVertical * RADIANS_PAR_DEGRES),

    // La verticale est en Z
    0,0,1);

  /* gluLookAt(
      // Position de la camera
      this->positionY + cos(-this->angleHorizontal * RADIANS_PAR_DEGRES),
      this->positionX - sin(-this->angleHorizontal * RADIANS_PAR_DEGRES),
      1,

      // Point vise
      this->positionY, this->positionX, 0,

      // La verticale est en Z
      0,0,1);*/
}

void Personnage::positionner(float16 x, float16 y)
{
  this->positionX = x;
  this->positionY = y;
}

void Personnage::orienter(float16 angle)
{
  this->angleHorizontal = angle;

  while (this->angleHorizontal >= 180.0f) // Lorsqu'on d�passe la limite (1/2 tour)
  {
    this->angleHorizontal -= 360.0f;
  }
  while (this->angleHorizontal < -180.0f) // Idem apr�s 1/2 tours vers la droite
  {
    this->angleHorizontal += 360.0f;
  }
}

float16 Personnage::lirePositionX()
{
  return this->positionX;
}

float16 Personnage::lirePositionY()
{
  return this->positionY;
}

float16 Personnage::lireAngle()
{
  return this->angleHorizontal;
}

void Personnage::ajouterPosition(sint32 heure, float16 x, float16 y, float16 angleHorizontal)
{
  this->historiqueDesPositions.ajouter(heure, x, y, angleHorizontal);
}

bool8 Personnage::positionnerDepuisLHistorique(sint32 heure)
{
  float16 positionX;
  float16 positionY;
  float16 angle;

  if (this->historiqueDesPositions.calculer(heure, &positionX, &positionY, &angle))
  {
    this->positionner(positionX, positionY);
    this->orienter(angle);

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*bool8 Personnage::heureDernierePositionConnue(sint32* heure)
{
    return this->historiqueDesPositions.heureDernierePositionConnue(heure);
}*/

sint32 Personnage::heureReapparition(void)
{
  return this->m_heureReapparition;
}

void Personnage::heureReapparition(sint32 heure)
{
  this->m_heureReapparition = heure;
}

Point Personnage::pointReapparition(void)
{
  return this->m_pointReapparition;
}

void Personnage::pointReapparition(Point point)
{
  this->m_pointReapparition.x = point.x;
  this->m_pointReapparition.y = point.y;
}

float16 Personnage::angleReapparition(void)
{
  return this->m_angleReapparition;
}

void Personnage::angleReapparition(float16 angle)
{
  this->m_angleReapparition = angle;
}

void Personnage::afficherHistorique(void)
{
  this->historiqueDesPositions.afficher();
}

uint32 Personnage::emis()
{
  return this->nbTirGagnant;
}

uint32 Personnage::recu()
{
  return this->nbTirRecu;
}

void Personnage::mettreAJourScore(uint32 emis, uint32 recu)
{
  this->nbTirGagnant = emis;
  this->nbTirRecu = recu;
}

void Personnage::pseudo(std::string pseudo)
{
  this->m_pseudo = pseudo;
}

std::string Personnage::pseudo(void)
{
  return this->m_pseudo;
}

void Personnage::dessiner()
{
  //this->objet3D->afficher();
  this->Objet3DStatique::dessiner();
}
