#include "udpClient.h"
#include <cstdio>
#include <cstring>

UDPClient::UDPClient()
{
  static bool8 networkInitialized = FALSE;

  // if the network hasn't been initialized
  if (networkInitialized == FALSE)
  {
    #ifdef __WIN32__
      // Initialization of the network
      WSADATA WSAData;
      WSAStartup(MAKEWORD(2,0), &WSAData);
    #endif

    networkInitialized = TRUE;
  }

  printf("Creation of UDP Client...\n");

  // Socket
  this->udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // Non-blocking socket
  #ifdef __WIN32__
    uint32 arg = 1;
    ioctlsocket (this->udpSocket, FIONBIO, &arg);
  #else
      fcntl (this->udpSocket, F_SETFL, fcntl (this->udpSocket, F_GETFL) | O_NONBLOCK);
  #endif

  printf("UDP CLient has been successfully created!\n");
}

void UDPClient::connect(std::string ip, uint16 port)
{
  // Address
  this->serverAddress.sin_family = AF_INET;
  this->serverAddress.sin_port = htons (port);
  memset(&this->serverAddress.sin_zero, 0, 8);

  hostent* serverInfo = gethostbyname(ip.c_str());
  if(serverInfo != NULL) {
    this->serverAddress.sin_addr.s_addr = ((in_addr*) serverInfo->h_addr)->s_addr;

    if (this->serverAddress.sin_addr.s_addr == 0) {
      this->serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
    }
  }
  else {
    // Error in resolving the server name
    this->serverAddress.sin_addr.s_addr = 0;
  }
}

std::string UDPClient::receive()
{
  SOCKADDR_IN senderAddress;
  SOCKLEN_T sizeSenderAddress;
  char message[MAX_SIZE_MESSAGE] = { 0 };

  // Reception of a possible message
  sizeSenderAddress = sizeof(senderAddress);
  if (recvfrom(this->udpSocket, message, MAX_SIZE_MESSAGE, 0, (struct sockaddr*) &senderAddress, &sizeSenderAddress) > 0) {
    return message;
  }

  return "";
}

void UDPClient::send(std::string message)
{
  // if we are connected
  if(this->serverAddress.sin_addr.s_addr != 0) {
    // Sending
    if(sendto(this->udpSocket, message.c_str(), message.length() + 1, 0, (sockaddr*) &this->serverAddress, sizeof(this->serverAddress)) == -1) {
      printf("An error happened while sending the message!\n");
    }
  }
}
