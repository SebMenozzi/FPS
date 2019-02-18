#ifndef UDPCLIENT_H_INCLUDED
#define UDPCLIENT_H_INCLUDED

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

#define MAX_SIZE_MESSAGE 65507

class UDPClient
{
  private:
    SOCKADDR_IN serverAddress;
    int udpSocket;

  public:
    UDPClient();
    void connect(std::string ip, uint16 port);
    std::string receive();
    void send(std::string message);
};

#endif // UDPCLIENT_H_INCLUDED
