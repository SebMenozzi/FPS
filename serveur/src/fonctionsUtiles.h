#ifndef FONCTIONSUTILES_H_INCLUDED
#define FONCTIONSUTILES_H_INCLUDED

#include <string>
#include "types.h"

std::string decapsuler(std::string* message);
sint32 stringEnSint32(std::string chaine);
float16 stringEnFloat16(std::string chaine);
std::string sint32EnString(sint32 valeur);
std::string uint32EnString(uint32 valeur);
int creerDossier(std::string chemin);
int dossierExiste(std::string chemin);

#endif // FONCTIONSUTILES_H_INCLUDED
