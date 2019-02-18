#include "historiqueDesPositions.h"
#include <iostream>
#include <stdio.h>

#define EXTRAPOLATION_MAX (200)

HistoriqueDesPositions::HistoriqueDesPositions()
{

}

HistoriqueDesPositions::~HistoriqueDesPositions()
{

}

void HistoriqueDesPositions::ajouter(sint32 heure, float16 x, float16 y, float16 z, float16 angleHorizontal, float16 angleVertical)
{
  // Si la position a ajouter est plus recente que la derniere position ajoutee
  std::vector<HistoriqueDesPositions::Position>::iterator position = positions.end();
  position--; // Derniere position
  if (0 == positions.size() || position->heure < heure)
  {
    // Creation de la position
    HistoriqueDesPositions::Position position;
    position.heure = heure;
    position.x = x;
    position.y = y;
    position.z = z;
    position.angleHorizontal = angleHorizontal;
    position.angleVertical = angleVertical;

    // Ajout de la position a la liste
    this->positions.push_back(position);
  }
}

bool8 HistoriqueDesPositions::calculer(sint32 heure, float16* x, float16* y, float16* angleHorizontal)
{
  uint32 dernierePosition = this->positions.size() - 1;

  if (this->positions.size() == 0) // La liste des positions est vide
  {
    // Impossible de calculer une position
    return FALSE;
  }
  else if (this->positions.size() == 1) // Une seule position
  {
    // Retourne la position connue quelque soit l'heure
    *x = this->positions[0].x;
    *y = this->positions[0].y;
    *angleHorizontal = this->positions[0].angleHorizontal;

    return TRUE;
  }
  else // Deux positions connues ou plus
  {
    // Avant la premiere position connue
    if (heure < this->positions[0].heure)
    {
      // Retourne la premiere position connue
      *x = this->positions[0].x;
      *y = this->positions[0].y;
      *angleHorizontal = this->positions[0].angleHorizontal;

      return TRUE;
    }
    // Position calculable a l'interrieur de l'historique
    else if (heure < this->positions[dernierePosition].heure)
    {
      uint32 i = this->positions.size() - 2; // Indice de l'avant derniere position

      while(TRUE) // Parcours l'historique en partant de la fin
      {
        if (this->positions[i].heure <= heure) // Detection de la position immediatement avant
        {
          // Calcule de la position interpolee
          *x = this->positions[i].x +
               (this->positions[i+1].x - this->positions[i].x) *
               (heure-this->positions[i].heure) /
               (this->positions[i+1].heure-this->positions[i].heure);

          *y = this->positions[i].y +
               (this->positions[i+1].y - this->positions[i].y) *
               (heure-this->positions[i].heure) /
               (this->positions[i+1].heure-this->positions[i].heure);

          // Calcule de l'angle interpolee, loin de +/-180 degres
          if (this->positions[i+1].angleHorizontal - this->positions[i].angleHorizontal > -180 && this->positions[i+1].angleHorizontal - this->positions[i].angleHorizontal < 180)
          {
            *angleHorizontal = this->positions[i].angleHorizontal +
                     (this->positions[i+1].angleHorizontal - this->positions[i].angleHorizontal) *
                     (heure-this->positions[i].heure) /
                     (this->positions[i+1].heure-this->positions[i].heure);
          }
          else if (this->positions[i+1].angleHorizontal - this->positions[i].angleHorizontal >= 180) // Au voisinage de +/-180 degres, vers la gauche
          {
            *angleHorizontal = this->positions[i].angleHorizontal +
                    ((this->positions[i+1].angleHorizontal - 360) - this->positions[i].angleHorizontal) *
                    (heure-this->positions[i].heure) /
                    (this->positions[i+1].heure-this->positions[i].heure);
          }
          else if (this->positions[i+1].angleHorizontal - this->positions[i].angleHorizontal <= -180) // Au voisinage de +/-180 degres, vers la droite
          {
            *angleHorizontal = this->positions[i].angleHorizontal +
                     ((this->positions[i+1].angleHorizontal + 360) - this->positions[i].angleHorizontal) *
                     (heure-this->positions[i].heure) /
                     (this->positions[i+1].heure-this->positions[i].heure);
          }

          // Bridage de l'angle horizontal dans l'intervale [-180;180[
          if (*angleHorizontal >= 180.0)
          {
            *angleHorizontal -= 360;
          }
          else if (*angleHorizontal < -180.0)
          {
            *angleHorizontal += 360;
          }
          return TRUE;
        }
        i--;
      }
    }
    // Apres la derniere position connue
    else
    {
      // Extrapolation de 200 ms maximum
      if ( heure >  (this->positions[dernierePosition].heure + EXTRAPOLATION_MAX) )
      {
        heure = this->positions[dernierePosition].heure + EXTRAPOLATION_MAX;
      }

      // Extrapolation de la position
      *x = this->positions[dernierePosition].x +
           ( (this->positions[dernierePosition].x - this->positions[dernierePosition-1].x) / (this->positions[dernierePosition].heure - this->positions[dernierePosition-1].heure) ) *
           (heure - this->positions[dernierePosition].heure);

      *y = this->positions[dernierePosition].y +
           ( (this->positions[dernierePosition].y - this->positions[dernierePosition-1].y) / (this->positions[dernierePosition].heure - this->positions[dernierePosition-1].heure) ) *
           (heure - this->positions[dernierePosition].heure);

      // Extrapolation de l'angle horizontal
      *angleHorizontal = this->positions[dernierePosition].angleHorizontal +
               ( (this->positions[dernierePosition].angleHorizontal - this->positions[dernierePosition-1].angleHorizontal) / (this->positions[dernierePosition].heure - this->positions[dernierePosition-1].heure) ) *
               (heure - this->positions[dernierePosition].heure);

      if (*angleHorizontal >= 180)
      {
        *angleHorizontal -= 360;
      }
      else if (*angleHorizontal < -180)
      {
        *angleHorizontal += 360;
      }
      return TRUE;
    }
  }
  return FALSE;
}

void HistoriqueDesPositions::afficher()
{
  std::cout << "Historique des positions :" << std::endl;

  // Parcours la liste des positions
  for (std::vector<HistoriqueDesPositions::Position>::iterator position = positions.begin(); position != positions.end();  )
  {
    std::cout << position->heure << "\t";
    std::cout << position->x << "\t";
    std::cout << position->y << "\t";
    std::cout << position->angleHorizontal << "\t";
    std::cout << position->angleVertical << std::endl;

    position++; // Passage a la position suivante
  }

  std::cout << std::endl;
}
