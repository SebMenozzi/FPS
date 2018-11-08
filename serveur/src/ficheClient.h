#ifndef FICHECLIENT_H_INCLUDED
#define FICHECLIENT_H_INCLUDED

#include "types.h"

typedef struct
{
    std::string pseudo;
    std::string ip;
    uint16 port;
    sint32 heureDernierMessage;

} FicheClient;

#endif // FICHECLIENT_H_INCLUDED
