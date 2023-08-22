#ifndef TOOLS_H
# define TOOLS_H

#include <sys/types.h>
#include <sys/socket.h>
#include "netinet/ip_icmp.h"
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include "libft.h"

#define TRUE 1
#define FALSE 0
#define NONE 20

struct s_flags {
    char v;
    char interrogation;
};

struct s_ping_memory {
    struct timeval *tvA;
    struct timeval *tvB;
    struct sockaddr_in *translate;
    struct icmphdr  *icmp;
};

void    exitInet(void);

extern int fdSocket;
extern struct  addrinfo *listAddr;
extern struct s_ping_memory    *pingMemory[65536];

#endif