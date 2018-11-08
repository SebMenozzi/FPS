#ifndef CLIENTUDP_H_INCLUDED
#define CLIENTUDP_H_INCLUDED

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

class ClientUDP
{
  private:
    SOCKADDR_IN adresseServeur;
    int socketUdp;

  public:
    ClientUDP();
    void connect(std::string ipServeur, uint16 portServeur);
    std::string recevoir();
    void envoyer(std::string message);
};

#endif // CLIENTUDP_H_INCLUDED
