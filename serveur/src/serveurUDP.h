#ifndef SERVEURUDP_H_INCLUDED
#define SERVEURUDP_H_INCLUDED

#if defined(__WIN32__)
  #include <winsock2.h>
  typedef int SOCKLEN_T;
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/ip.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <fcntl.h>
  typedef sockaddr_in SOCKADDR_IN;
  typedef sockaddr SOCKADDR;
  typedef int SOCKET;
  typedef socklen_t SOCKLEN_T;
#endif
#include "types.h"
#include <string>

#define TAILLE_MAX_MESSAGE 65507

class ServeurUDP
{
  private:
    SOCKADDR_IN adresse_udp;
    SOCKET socketUdp;

  public:
    ServeurUDP(uint16 portEcoute);
    std::string recevoir(std::string* ipExpediteur, uint16* portExpediteur);
    void envoyer(std::string message, std::string ipDestinataire, uint16 portDestinataire);
};

#endif // SERVEURUDP_H_INCLUDED
