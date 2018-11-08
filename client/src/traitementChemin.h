#ifndef FONCTIONS_UTILES_H_INCLUDED
#define FONCTIONS_UTILES_H_INCLUDED

#include <string>

void deparasiterFin(std::string* chaineADeparasiter, std::string caracteresParasitants);
void deparasiterDebut(std::string* chaineADeparasiter, std::string caracteresParasitants);
void deparasiterDebutEtFin(std::string* chaineADeparasiter, std::string caracteresParasitants);
std::string cheminFinal(std::string cheminDeBase, std::string cheminRelatif);

#endif // FONCTIONS_UTILES_H_INCLUDED
