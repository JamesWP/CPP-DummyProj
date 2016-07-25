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

#include "networkread.hpp"

#define BUFFER_SIZE 1024

/**
 * provides access to network input output
 * wraps native c socket api with a c++ class
 */
class NetworkInterface
{
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  size_t n;
  bool _closed;
  // input buffer
  char inbuf[BUFFER_SIZE];
  size_t inbuf_used = 0;

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

  template<typename Callback>
  void processInput(Callback c) {
    _closed = !networkthread::processInput<BUFFER_SIZE>(c, newsockfd, inbuf, inbuf_used);
  }
  
};

#endif /* network_h */
