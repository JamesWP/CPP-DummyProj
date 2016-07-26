// code adapted from:
//https://www.gnu.org/software/libc/manual/html_node/Server-Example.html
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define PORT    5555
#define MAXMSG  512

// makes a socket listener on port
// will re use recently used port if required
int
make_socket (uint16_t port)
{
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }
  int enable = 1;

  // the following will allow the socket to be bound again to a previoudly
  // closed port without having to wait for it to become available again
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl (INADDR_ANY);
  if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
    {
      perror ("bind");
      exit (EXIT_FAILURE);
    }

  return sock;
}

int
read_from_client (int filedes)
{
  char buffer[MAXMSG+1];
  int nbytes;

  nbytes = read (filedes, buffer, MAXMSG);
  if (nbytes < 0)
    {
      /* Read error. */
      perror ("read");
      exit (EXIT_FAILURE);
    }
  else if (nbytes == 0)
    /* End-of-file. */
    return -1;
  else
    {
      // remove last \n if one exists and place terminator after string
      if(buffer[nbytes-1] == '\n') buffer[nbytes-1] = '\0';
      else buffer[nbytes] = '\0';
      /* Data read. */
      fprintf (stderr, "Server: got message: `%s'\n", buffer);
      return 0;
    }
}

int
main (void)
{
  int sock; // the server socket
  // active is the container for the sockets, at each loop it is coped into 
  // read_fd_set before itteration to prevent update while looping
  fd_set active_fd_set, read_fd_set;
  int i;
  struct sockaddr_in clientname;
  socklen_t size;

  /* Create the socket and set it up to accept connections. */
  sock = make_socket (PORT);
  if (listen (sock, 1) < 0)
    {
      perror ("listen");
      exit (EXIT_FAILURE);
    }

  /* Initialize the set of active sockets. */
  FD_ZERO (&active_fd_set);
  FD_SET (sock, &active_fd_set);

  while (1)
  {
    /* Block until input arrives on one or more active sockets. */
    // read_fd_set will be modified to contain only the sockets that require
    // servicing
    read_fd_set = active_fd_set;
    if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
    {
      perror ("select");
      exit (EXIT_FAILURE);
    }

    /* Service all the sockets with input pending. */
    for (i = 0; i < FD_SETSIZE; ++i)
    {
      if (!FD_ISSET (i, &read_fd_set)) continue;
      // socket i requires attention
      if(i == sock)            // socket is origonal listening socket and has a new connection
      {
        /* Connection request on original socket. */
        int new;
        size = sizeof (clientname);
        new = accept (sock,
                      (struct sockaddr *) &clientname,
                      &size);
        if (new < 0)
          {
            perror ("accept");
            exit (EXIT_FAILURE);
          }
        char str[INET_ADDRSTRLEN];
        inet_ntop( AF_INET, &clientname.sin_addr, str, INET_ADDRSTRLEN );
        fprintf (stderr,
                 "Server: connect from host %s, port %u.\n",
                 str, ntohs (clientname.sin_port));
        FD_SET (new, &active_fd_set);
      }else{
        // socket is a client with new data
        /* Data arriving on an already-connected socket. */
        if (read_from_client (i) < 0){
            close (i);
            FD_CLR (i, &active_fd_set);
          }
      }
    }
  }
}
