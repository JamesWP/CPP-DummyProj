//
//  network.h
//  CMS
//
//  Created by James Peach on 19/06/2016.
//  Copyright © 2016 James Peach. All rights reserved.
//

#ifndef network_h
#define network_h

/* The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <sstream>
#include <iostream>

class NetworkInterface
{
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  size_t n;
  bool _closed;
public:
  NetworkInterface(int port):portno{port},_closed{false}{}
  ~NetworkInterface(){
    close(newsockfd);
    close(sockfd);
  }
  void setup();
  void acceptConn();
  void sendStr(std::string);
  bool closed() const {return _closed;}
  std::string readStr();
};

#endif /* network_h */
