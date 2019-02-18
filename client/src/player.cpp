#include "player.h"
#include <math.h>

#ifndef INT_MIN
  #define INT_MIN ((int)0x80000000)
#endif

Player::Player(float16 x, float16 y, float16 z, float16 horizontal_angle, float16 vertical_angle, float16 radius, std::string file) : Objet3DStatique(file)
{
  this->x = x;
  this->y = y;
  this->z = z;
  this->horizontal_angle = horizontal_angle; // En degrès, sens trigo vue de +Z vers -Z (du dessus) [-3600.0 .. +3600.0[
  this->vertical_angle = vertical_angle; // En degrès ; 0 = horizon ; > 0 = vers le ciel ; < 0 = vers les sol [-45 .. +45]
  this->radius = radius;
  this->reapparition_time = INT_MIN;
  this->winning_shots_count = 0;
  this->received_shots_count = 0;
  this->username = "";

  //this->objet3D = new Objet3D(file);
}

Player::~Player(void)
{
  //delete this->objet3D;
}

void Player::move(float16 distance, float16 direction, bool8 entourage[8], float16 heightmap_height)
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

  direction += this->horizontal_angle;

  // On evite que l'azimut du Player approche un multiple de 90
  // Cela evite des soucis lors de la gestion des collisions
  const float MODULO = 90.0f;
  float reste = fmod(direction, MODULO);

  if (this->horizontal_angle >= 0.0f) {  // Angle positif ou nul
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
  float16 target_x = this->x - distance * sin(direction * M_PI / 180.0);
  float16 target_y = this->y - distance * cos(direction * M_PI / 180.0);

  this->x = target_x;
  this->y = target_y;
  this->z = heightmap_height;

  /*

  // GESTION DES COLLISIONS AVEC MUR EST

  // S'il y a un mur a l'Est
  // et que la droite du Player arrive dans le mur
  if( (1 == entourage[4]) && ceil(target_x + this->radius) != ceil(this->x) )
  {
    // On rectifie la translation en Y (3D)
    target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
  }

  // GESTION DES COLLISIONS AVEC MUR SUD

  // S'il y a un mur au Sud et que notre Sud arrive dans le mur
  if (1 == entourage[6] && ceil(target_y + this->radius) != ceil(this->y))
  {
    // On rectifie la translation en X (3D)
    target_y -= (target_y + this->radius) - (sint32)(target_y + this->radius);
  }

  // GESTION DES COLLISIONS AVEC MUR OUEST

  // S'il y a un mur a l'Ouest et que notre Ouest arrive dans le mur
  if (1 == entourage[3] && floor(target_x - this->radius) != floor(this->x))
  {
    // On rectifie la translation en Y (3D)
    target_x -= (target_x - this->radius) - (sint32)(target_x + this->radius);
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD

  // S'il y a un mur au Nord et que notre Nord arrive dans le mur
  if (1 == entourage[1] && floor(target_y - this->radius) != floor(this->y))
  {
    // On rectifie la translation en X (3D)
    target_y -= (target_y - this->radius) - (sint32)(target_y + this->radius);
  }


  // GESTION DES COLLISIONS AVEC MUR AU SUD-EST
  if (1 == entourage[7] && ceil(target_y + this->radius) != ceil(y) && ceil(target_x + this->radius) != ceil(x))
  {
    if (target_y > this->y && target_x > this->x) // Approche
    {
      // Evite la division par zero
      if (((target_x + this->radius) - (sint32)(target_x + this->radius)) == 0.0 || (target_x - this->x) == 0.0)
      {
        target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
      }
      else
      {
        // Si ce sont les bord en Y qui ont �t� en contact lors de la collision
        if ( ((target_y + this->radius) - (sint32)(target_y + this->radius)) / ((target_x + this->radius) - (sint32)(target_x + this->radius)) > (target_y - this->y) / (target_x - this->x))
        {
          target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
        }
        else
        {
          target_y -= (target_y + this->radius) - (sint32)(target_y + this->radius);
        }
      }
    }
    else if (target_y < this->y) // Eloigne en X
    {
      target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
    }
    else if (target_x < this->x) // Eloigne en Y
    {
      target_y -= (target_y + this->radius) - (sint32)(target_y + this->radius);
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU SUD-OUEST
  if (1 == entourage[5] && ceil(target_y + this->radius) != ceil(y) && floor(target_x - this->radius) != floor(x))
  {
    if (target_y < this->y) // Eloigne en X
    {
      target_x += (sint32)((target_x - this->radius) + 1) - (target_x - this->radius);
    }
    else if (target_x > this->x) // Eloigne en Y
    {
      target_y -= (target_y + this->radius) - (sint32)(target_y + this->radius);
    }
    else if (target_y > this->y && target_x < this->x) // Approche
    {
      if(((sint32)(target_x - this->radius + 1) - (target_x - this->radius)) == 0.0 || (this->x - target_x) == 0.0)
      {
          target_x += (sint32)((target_x - this->radius) + 1) - (target_x - this->radius);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( ((target_y + this->radius) - (sint32)(target_y + this->radius)) / ((sint32)(target_x - this->radius + 1) - (target_x - this->radius)) > (target_y - this->y) / (this->x - target_x))
        {
          target_x += (sint32)((target_x - this->radius) + 1) - (target_x - this->radius);
        }
        else
        {
          target_y -= (target_y + this->radius) - (sint32)(target_y + this->radius);
        }
      }
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD-OUEST
  if (1 == entourage[0] && floor(target_y - this->radius) != floor(y) && floor(target_x - this->radius) != floor(x))
  {
    if (target_y > this->y) // Eloigne en X
    {
      target_x += (sint32)((target_x - this->radius) + 1) - (target_x - this->radius);
    }
    else if (target_x > this->x) // Eloigne en Y
    {
      target_y += (sint32)((target_y - this->radius) + 1) - (target_y - this->radius);
    }
    else if (target_y < this->y && target_x < this->x) // Approche
    {
      // Evite la division par zero
      if (((sint32)((target_x - this->radius) + 1) - (target_x - this->radius)) == 0.0 || (this->x - target_x) == 0.0)
      {
        target_x += (sint32)((target_x - this->radius) + 1) - (target_x - this->radius);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( ((sint32)((target_y - this->radius) + 1) - (target_y - this->radius)) / ((sint32)((target_x - this->radius) + 1) - (target_x - this->radius)) > (this->y - target_y) / (this->x - target_x))
        {
          target_x += (sint32)((target_x - this->radius) + 1) - (target_x - this->radius);
        }
        else
        {
          target_y += (sint32)((target_y - this->radius) + 1) - (target_y - this->radius);
        }
      }
    }
  }

  // GESTION DES COLLISIONS AVEC MUR AU NORD-EST
  if (1 == entourage[2] && floor(target_y - this->radius) != floor(y) && ceil(target_x + this->radius) != ceil(x))
  {
    if (target_y > this->y) // Eloigne en X
    {
      target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
    }
    else if (target_x < this->x) // Eloigne en Y
    {
      target_y += (sint32)((target_y - this->radius) + 1) - (target_y - this->radius);
    }
    else if (target_y < this->y && target_x > this->x) // Approche
    {
      if ((((target_x + this->radius)) - (sint32)(target_x + this->radius)) == 0.0 || (this->x - target_x) == 0.0)
      {
        target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
      }
      else
      {
        // Si ce sont les bords en Y qui ont �t� en contact lors de la collision
        if ( (((target_y - this->radius)) - (sint32)((target_y - this->radius) + 1)) / (((target_x + this->radius)) - (sint32)(target_x + this->radius)) < (this->y - target_y) / (this->x - target_x))
        {
          target_x -= (target_x + this->radius) - (sint32)(target_x + this->radius);
        }
        else
        {
          target_y += (sint32)((target_y - this->radius) + 1) - (target_y - this->radius);
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

void Player::horizontal_turn(float16 angle)
{
  this->horizontal_angle += angle;

  while (this->horizontal_angle >= 180.0f) { // Lorsqu'on depasse la limite (1/2 tour)
    this->horizontal_angle -= 360.0f;
  }
  while (this->horizontal_angle < -180.0f) { // Idem apres 1/2 tours vers la droite
    this->horizontal_angle += 360.0f;
  }
}

void Player::vertical_turn(float16 angle)
{
  this->vertical_angle += angle;

  if (45.0f < this->vertical_angle) {
    this->vertical_angle = 45.0f;
  } else if (-45.0f > this->vertical_angle) {
    this->vertical_angle = -45.0f;
  }
}

void Player::go_up()
{
  this->z += 0.05;
}

void Player::go_down()
{
  this->z -= 0.05;
}

void Player::get_position(float16* x, float16* y, float16* z)
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

  glRotatef(this->vertical_angle, -1, 0, 0); // Hausse / baisse la tête

  gluLookAt(
    // Eye position
    this->y,
    this->x,
    this->z + HEIGHT_LOOK_PLAYER,

    // target
    this->y - cos(-this->horizontal_angle * RADIANS_PAR_DEGRES),
    this->x + sin(-this->horizontal_angle * RADIANS_PAR_DEGRES),
    //HEIGHT_LOOK_PLAYER + tan(this->vertical_angle * RADIANS_PAR_DEGRES),
    this->z + HEIGHT_LOOK_PLAYER,

    // La verticale est en Z
    0,0,1);
}

void Player::set_position(float16 x, float16 y, float16 z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}

void Player::set_direction(float16 horizontal_angle, float16 vertical_angle)
{
  this->horizontal_angle = horizontal_angle;

  while (this->horizontal_angle >= 180.0f) { // Lorsqu'on depasse la limite (1/2 tour)
    this->horizontal_angle -= 360.0f;
  }
  while (this->horizontal_angle < -180.0f) { // Idem apres 1/2 tours vers la droite
    this->horizontal_angle += 360.0f;
  }

  this->vertical_angle = vertical_angle;

  if (45.0f < this->vertical_angle) {
    this->vertical_angle = 45.0f;
  } else if (-45.0f > this->vertical_angle) {
    this->vertical_angle = -45.0f;
  }
}

float16 Player::get_x()
{
  return this->x;
}

float16 Player::get_y()
{
  return this->y;
}

float16 Player::get_z()
{
  return this->z;
}

float16 Player::get_horizontal_angle()
{
  return this->horizontal_angle;
}

float16 Player::get_vertical_angle()
{
  return this->vertical_angle;
}

/*
void Player::add_position(sint32 time, float16 x, float16 y, float16 z, float16 horizontal_angle, float16 vertical_angle)
{
  this->positions.ajouter(time, x, y, z, horizontal_angle, vertical_angle);
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

sint32 Player::get_reapparition_time(void)
{
  return this->reapparition_time;
}

void Player::set_reapparition_time(sint32 time)
{
  this->reapparition_time = time;
}

Point3Float Player::get_reapparition_point(void)
{
  return this->reapparition_point;
}

void Player::set_reapparition_point(Point3Float point)
{
  this->reapparition_point.x = point.x;
  this->reapparition_point.y = point.y;
  this->reapparition_point.y = point.z;
}

float16 Player::get_reapparition_angle(void)
{
  return this->reapparition_angle;
}

void Player::set_reapparition_angle(float16 angle)
{
  this->reapparition_angle = angle;
}

/*
void Player::afficherHistorique(void)
{
  this->positions.display();
}
*/

uint32 Player::get_winning_shots()
{
  return this->winning_shots_count;
}

uint32 Player::get_received_shots()
{
  return this->received_shots_count;
}

void Player::update_score(uint32 emitted, uint32 received)
{
  this->winning_shots_count = emitted;
  this->received_shots_count = received;
}

void Player::set_username(std::string username)
{
  this->username = username;
}

std::string Player::get_username(void)
{
  return this->username;
}

void Player::draw()
{
  //this->objet3D->afficher();
  this->Objet3DStatique::dessiner();
}
