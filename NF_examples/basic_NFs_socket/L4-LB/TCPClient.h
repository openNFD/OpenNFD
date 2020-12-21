#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <vector>
#include "basic_classes.h"
using namespace std;

class TCPClient
{
  private:
    std::string address;
    int port;
    struct sockaddr_in server;

  public:
    int sock;
    TCPClient();
    int setup(string address, int port);
    int setup(IP ip, int port);
    bool Send(string data);
    string receive(int size = 4096);
    string read();
};
