#ifndef TIR_H_INCLUDED
#define TIR_H_INCLUDED

#include "types.h"
#include <string>

typedef struct
{
    // Identifiant du tireur
    std::string ip;
    uint16 port;

    sint32 heure; // Heure du tir
} Tir;

#endif // TIR_H_INCLUDED
