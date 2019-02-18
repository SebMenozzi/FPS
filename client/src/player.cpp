#include "player.h"
#include <math.h>

#ifndef INT_MIN
  #define INT_MIN ((int)0x80000000)
#endif

Player::Player(float16 x, float16 y, float16 z, float16 horizontalAngle, float16 verticalAngle, float16 radius, std::string file) : Objet3DStatique(file)
{
  this->x = x;
  this->y = y;
  this->z = z;
  this->horizontalAngle = horizontalAngle; // En degrès, sens trigo vue de +Z vers -Z (du dessus) [-3600.0 .. +3600.0[
  this->verticalAngle = verticalAngle; // En degrès ; 0 = horizon ; > 0 = vers le ciel ; < 0 = vers les sol [-45 .. +45]
  this->radius = radius;
  this->reapparitionTime = INT_MIN;
  this->winningShotsCount = 0;
  this->receivedShotsCount = 0;
  this->username = "";

  //this->objet3D = new Objet3D(file);
}

Player::~Player(void)
{
  //delete this->objet3D;
}

void Player::move(float16 distance, float16 direction, bool8 entourage[8], float16 heightmapHeight)
{
  const float EPSILON = 0.001f;

  //if (EPSILON >= distance) return;

  // Bridage de la distance pour eviter la perte de collision (a calculer et verifier)
  //float16 distanceMax = this->rayon - EPSILON;

  // Calcule des segments pour ce deplacement
  /*unsigned long nbGrandsSegments = (unsigned long) (distance / distanceMax);
  float distancePetitSegment = fmod(distance, distanceMax);
  // Pour chaque grand segment
  for(uint32 i = 0; i < nbGrandsSegments; i++)
  {
      // Deplacement d'un peu moins d'un grand segment
      this->deplacer(distanceMax - EPSILON, direction, entourage);
  }
  // Distance restante
  distance = distancePetitSegment;*/

  /*
  if (distance > distanceMax)
  {
    distance = distanceMax;
  }
  */

  direction += this->horizontalAngle;

  // On evite que l'azimut du Player approche un multiple de 90
  // Cela evite des soucis lors de la gestion des collisions
  const float MODULO = 90.0f;
  float reste = fmod(direction, MODULO);

  if (this->horizontalAngle >= 0.0f) {  // Angle positif ou nul
    if (reste < EPSILON) { // 0+
      direction += EPSILON;
    } else if (reste > MODULO - EPSILON) { // 90-
      direction -= EPSILON;
    }
  }
  else { // Angle negatif
    if (reste > -EPSILON) { // 0-
      direction -= EPSILON;
    } else if (reste > -MODULO + EPSILON) { // (-90)+
      direction += EPSILON;
    }
  }

  // Calcule de la position cible du Player
  float16 targetX = this->x - distance * sin(direction * M_PI / 180.0);
  float16 targetY = this->y - distance * cos(direction * M_PI / 180.0);

  this->x = targetX;
  this->y = targetY;
  this->z = heightmapHeight;

  /*

  // GESTION DES COLLISIONS AVEC MUR EST

  // S'il y a un mur a l'Est
  // et que la droite du Player arrive dans le mur
  if( (1 == entourage[4]) && ceil(targetX + this->radius) != ceil(this->x) )
  {
    // On rectifie la translation en Y (3D)
    targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
  }

  // GESTION DES COLLISIONS AVEC MUR SUD

  // S'il y a un mur au Sud et que notre Sud arrive dans le mur
  if (1 == entourage[6] && ceil(targetY + this->radius) != ceil(this->y))
  {
    // On rectifie la translation en X (3D)
    targetY -= (targetY + this->radius) - (sint32)(targetY + this->radius);
  }

  // GESTION DES COLLISIONS AVEC MUR OUEST

  // S'il y a un mur a l'Ouest et que notre Ouest arrive dans le mur
  if (1 == entourage[3] && floor(targetX - this->radius) != floor(this->x))
  {
    // On rectifie la translation en Y (3D)
    targetX -= (targetX - this->radius) - (sint32)(targetX + this->radius);
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD

  // S'il y a un mur au Nord et que notre Nord arrive dans le mur
  if (1 == entourage[1] && floor(targetY - this->radius) != floor(this->y))
  {
    // On rectifie la translation en X (3D)
    targetY -= (targetY - this->radius) - (sint32)(targetY + this->radius);
  }


  // GESTION DES COLLISIONS AVEC MUR AU SUD-EST
  if (1 == entourage[7] && ceil(targetY + this->radius) != ceil(y) && ceil(targetX + this->radius) != ceil(x))
  {
    if (targetY > this->y && targetX > this->x) // Approche
    {
      // Evite la division par zero
      if (((targetX + this->radius) - (sint32)(targetX + this->radius)) == 0.0 || (targetX - this->x) == 0.0)
      {
        targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
      }
      else
      {
        // Si ce sont les bord en Y qui ont �t� en contact lors de la collision
        if ( ((targetY + this->radius) - (sint32)(targetY + this->radius)) / ((targetX + this->radius) - (sint32)(targetX + this->radius)) > (targetY - this->y) / (targetX - this->x))
        {
          targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
        }
        else
        {
          targetY -= (targetY + this->radius) - (sint32)(targetY + this->radius);
        }
      }
    }
    else if (targetY < this->y) // Eloigne en X
    {
      targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
    }
    else if (targetX < this->x) // Eloigne en Y
    {
      targetY -= (targetY + this->radius) - (sint32)(targetY + this->radius);
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU SUD-OUEST
  if (1 == entourage[5] && ceil(targetY + this->radius) != ceil(y) && floor(targetX - this->radius) != floor(x))
  {
    if (targetY < this->y) // Eloigne en X
    {
      targetX += (sint32)((targetX - this->radius) + 1) - (targetX - this->radius);
    }
    else if (targetX > this->x) // Eloigne en Y
    {
      targetY -= (targetY + this->radius) - (sint32)(targetY + this->radius);
    }
    else if (targetY > this->y && targetX < this->x) // Approche
    {
      if(((sint32)(targetX - this->radius + 1) - (targetX - this->radius)) == 0.0 || (this->x - targetX) == 0.0)
      {
          targetX += (sint32)((targetX - this->radius) + 1) - (targetX - this->radius);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( ((targetY + this->radius) - (sint32)(targetY + this->radius)) / ((sint32)(targetX - this->radius + 1) - (targetX - this->radius)) > (targetY - this->y) / (this->x - targetX))
        {
          targetX += (sint32)((targetX - this->radius) + 1) - (targetX - this->radius);
        }
        else
        {
          targetY -= (targetY + this->radius) - (sint32)(targetY + this->radius);
        }
      }
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD-OUEST
  if (1 == entourage[0] && floor(targetY - this->radius) != floor(y) && floor(targetX - this->radius) != floor(x))
  {
    if (targetY > this->y) // Eloigne en X
    {
      targetX += (sint32)((targetX - this->radius) + 1) - (targetX - this->radius);
    }
    else if (targetX > this->x) // Eloigne en Y
    {
      targetY += (sint32)((targetY - this->radius) + 1) - (targetY - this->radius);
    }
    else if (targetY < this->y && targetX < this->x) // Approche
    {
      // Evite la division par zero
      if (((sint32)((targetX - this->radius) + 1) - (targetX - this->radius)) == 0.0 || (this->x - targetX) == 0.0)
      {
        targetX += (sint32)((targetX - this->radius) + 1) - (targetX - this->radius);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( ((sint32)((targetY - this->radius) + 1) - (targetY - this->radius)) / ((sint32)((targetX - this->radius) + 1) - (targetX - this->radius)) > (this->y - targetY) / (this->x - targetX))
        {
          targetX += (sint32)((targetX - this->radius) + 1) - (targetX - this->radius);
        }
        else
        {
          targetY += (sint32)((targetY - this->radius) + 1) - (targetY - this->radius);
        }
      }
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD-EST
  if (1 == entourage[2] && floor(targetY - this->radius) != floor(y) && ceil(targetX + this->radius) != ceil(x))
  {
    if (targetY > this->y) // Eloigne en X
    {
      targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
    }
    else if (targetX < this->x) // Eloigne en Y
    {
      targetY += (sint32)((targetY - this->radius) + 1) - (targetY - this->radius);
    }
    else if (targetY < this->y && targetX > this->x) // Approche
    {
      if ((((targetX + this->radius)) - (sint32)(targetX + this->radius)) == 0.0 || (this->x - targetX) == 0.0)
      {
        targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( (((targetY - this->radius)) - (sint32)((targetY - this->radius) + 1)) / (((targetX + this->radius)) - (sint32)(targetX + this->radius)) < (this->y - targetY) / (this->x - targetX))
        {
          targetX -= (targetX + this->radius) - (sint32)(targetX + this->radius);
        }
        else
        {
          targetY += (sint32)((targetY - this->radius) + 1) - (targetY - this->radius);
        }
      }
    }
  }
  */
}

/*
void Player::gauche(float16 distance, bool8 entourage[8])
{
  this->avancer(distance, 90.0, entourage);
}

void Player::droite(float16 distance, bool8 entourage[8])
{
  this->avancer(distance, -90.0, entourage);
}
*/

void Player::horizontalTurn(float16 angle)
{
  this->horizontalAngle += angle;

  while (this->horizontalAngle >= 180.0f) { // Lorsqu'on depasse la limite (1/2 tour)
    this->horizontalAngle -= 360.0f;
  }
  while (this->horizontalAngle < -180.0f) { // Idem apres 1/2 tours vers la droite
    this->horizontalAngle += 360.0f;
  }
}

void Player::verticalTurn(float16 angle)
{
  this->verticalAngle += angle;

  if (45.0f < this->verticalAngle) {
    this->verticalAngle = 45.0f;
  } else if (-45.0f > this->verticalAngle) {
    this->verticalAngle = -45.0f;
  }
}

void Player::goUp()
{
  this->z += 0.05;
}

void Player::goDown()
{
  this->z -= 0.05;
}

void Player::getPosition(float16* x, float16* y, float16* z)
{
  *x = this->x;
  *y = this->y;
  *z = this->y;
}

void Player::look(void)
{
  #define RADIANS_PAR_DEGRES 0.0174532925199
  #define HEIGHT_LOOK_PLAYER 0.55

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glRotatef(this->verticalAngle, -1, 0, 0); // Hausse / baisse la tête

  gluLookAt(
    // Eye position
    this->y,
    this->x,
    this->z + HEIGHT_LOOK_PLAYER,

    // target
    this->y - cos(-this->horizontalAngle * RADIANS_PAR_DEGRES),
    this->x + sin(-this->horizontalAngle * RADIANS_PAR_DEGRES),
    //HEIGHT_LOOK_PLAYER + tan(this->verticalAngle * RADIANS_PAR_DEGRES),
    this->z + HEIGHT_LOOK_PLAYER,

    // La verticale est en Z
    0,0,1);
}

void Player::setPosition(float16 x, float16 y, float16 z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}

void Player::setDirection(float16 horizontalAngle, float16 verticalAngle)
{
  this->horizontalAngle = horizontalAngle;

  while (this->horizontalAngle >= 180.0f) { // Lorsqu'on depasse la limite (1/2 tour)
    this->horizontalAngle -= 360.0f;
  }
  while (this->horizontalAngle < -180.0f) { // Idem apres 1/2 tours vers la droite
    this->horizontalAngle += 360.0f;
  }

  this->verticalAngle = verticalAngle;

  if (45.0f < this->verticalAngle) {
    this->verticalAngle = 45.0f;
  } else if (-45.0f > this->verticalAngle) {
    this->verticalAngle = -45.0f;
  }
}

float16 Player::getX()
{
  return this->x;
}

float16 Player::getY()
{
  return this->y;
}

float16 Player::getZ()
{
  return this->z;
}

float16 Player::getHorizontalAngle()
{
  return this->horizontalAngle;
}

float16 Player::getVerticalAngle()
{
  return this->verticalAngle;
}

/*
void Player::add_position(sint32 time, float16 x, float16 y, float16 z, float16 horizontalAngle, float16 verticalAngle)
{
  this->positions.ajouter(time, x, y, z, horizontalAngle, verticalAngle);
}
*/

/*
bool8 Player::positionnerDepuisLHistorique(sint32 heure)
{
  float16 x;
  float16 y;
  float16 angle;

  if (this->historiqueDesPositions.calculer(heure, &x, &y, &angle))
  {
    // TODO: calculer avec la composante Z
    this->positionner(x, y, 0);
    this->orienter(angle, 0);

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
*/

/*bool8 Player::heureDernierePositionConnue(sint32* heure)
{
    return this->historiqueDesPositions.heureDernierePositionConnue(heure);
}*/

sint32 Player::getReapparitionTime(void)
{
  return this->reapparitionTime;
}

void Player::setReapparitionTime(sint32 time)
{
  this->reapparitionTime = time;
}

Point3Float Player::getReapparitionPoint(void)
{
  return this->reapparitionPoint;
}

void Player::setReapparitionPoint(Point3Float point)
{
  this->reapparitionPoint.x = point.x;
  this->reapparitionPoint.y = point.y;
  this->reapparitionPoint.y = point.z;
}

float16 Player::getReapparitionAngle(void)
{
  return this->reapparitionAngle;
}

void Player::setReapparitionAngle(float16 angle)
{
  this->reapparitionAngle = angle;
}

/*
void Player::afficherHistorique(void)
{
  this->positions.display();
}
*/

uint32 Player::getWinningShots()
{
  return this->winningShotsCount;
}

uint32 Player::getReceivedShots()
{
  return this->receivedShotsCount;
}

void Player::updateScore(uint32 emitted, uint32 received)
{
  this->winningShotsCount = emitted;
  this->receivedShotsCount = received;
}

void Player::setUsername(std::string username)
{
  this->username = username;
}

std::string Player::getUsername(void)
{
  return this->username;
}

void Player::draw()
{
  //this->objet3D->afficher();
  this->Objet3DStatique::dessiner();
}
