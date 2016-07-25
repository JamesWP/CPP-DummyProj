//
//  networkread.hpp
//  cms
//
//  Created by James Peach on 25/07/2016.
//  Copyright © 2016 James Peach. All rights reserved.
//

#ifndef networkread_hpp
#define networkread_hpp

#include <string>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

//reads from the socket and calls the callback for each line
//http://stackoverflow.com/questions/6090594/c-recv-read-until-newline-occurs
// Callback should be a function that takes a std::string
// this function will return false when the socket fd is now closed or if an error occured

namespace networkthread{
  template<int BufferSize,typename Callback>
  bool processInput(Callback c, const int fd, char *inbuf, size_t &inbuf_used) {
    size_t buf_remain = BufferSize - inbuf_used;
    if (buf_remain == 0) {
      fprintf(stderr, "Line exceeded buffer length!\n");
      abort();
    }

    // received up to remainin buffer length, blocking ( don't waste time looping)
    ssize_t rv = recv(fd, (void*)&inbuf[inbuf_used], buf_remain, 0);
    if (rv == 0) {
      return false;
    }
    if (rv < 0 && errno == EAGAIN) {
      /* no data for now, call back when the socket is readable */
      return true;
    }
    if (rv < 0) {
      perror("Connection error");
      return false;
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
    return true;
  }
}
#endif /* networkread_hpp */
