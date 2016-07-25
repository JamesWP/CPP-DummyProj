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

  //reads from the socket and calls the callback for each line
  //http://stackoverflow.com/questions/6090594/c-recv-read-until-newline-occurs
  template<class Callback>
  void processInput(Callback c) {
    int fd = newsockfd;
    size_t buf_remain = sizeof(inbuf) - inbuf_used;
    if (buf_remain == 0) {
      fprintf(stderr, "Line exceeded buffer length!\n");
      abort();
    }

    // received up to remainin buffer length, blocking ( don't waste time looping)
    ssize_t rv = recv(fd, (void*)&inbuf[inbuf_used], buf_remain, 0);
    if (rv == 0) {
      // connection has been closed by the client
      _closed = true;
    }
    if (rv < 0 && errno == EAGAIN) {
      /* no data for now, call back when the socket is readable */
      return;
    }
    if (rv < 0) {
      perror("Connection error");
      abort();
    }
    inbuf_used += rv;

    /* Scan for newlines in the line buffer; we're careful here to deal with embedded \0s
     * an evil server may send, as well as only processing lines that are complete.
     */
    char *line_start = inbuf;
    char *line_end;
    while ( (line_end = (char*)memchr((void*)line_start, '\n', inbuf_used - (line_start - inbuf))))
    {
      *line_end = 0;
      std::string str(line_start);
      c(str);
      line_start = line_end + 1;
    }
    /* Shift buffer down so the unprocessed data is at the start */
    inbuf_used -= (line_start - inbuf);
    memmove(inbuf, line_start, inbuf_used);
  } 
};

#endif /* network_h */
