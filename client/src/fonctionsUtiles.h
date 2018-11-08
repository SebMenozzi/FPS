#ifndef FONCTIONSUTILES_H_INCLUDED
#define FONCTIONSUTILES_H_INCLUDED

#include <string>
#include "types.h"

std::string decapsuler(std::string* message);
sint32 stringEnSint32(std::string chaine);
uint32 stringEnUint32(std::string chaine);
float16 stringEnFloat16(std::string chaine);
std::string sint32EnString(sint32 valeur);

#endif // FONCTIONSUTILES_H_INCLUDED
