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
#define ICMP 1
#define STDERR 2
#define FQDN_MAX 255

struct s_flags {
    char v;
    char interrogation;
};

/*
    dup == when packet is read for the first time, set it to TRUE
    if dup is already on TRUE, then it's a duplicate packet
*/
struct s_ping_memory {
    struct icmphdr  icmp;
    struct timeval tvB;
    char    dup;
};
/*[0]=min [1]=max*/
struct s_round_trip {
    double  rtt[2];
    double  sum;
    double  squareSum;
    size_t  number;
    uint32_t    packetReceive;
    uint32_t    packetDuplicate;
    uint32_t    packetSend;
};

void    exitInet(void);

extern int fdSocket;
extern struct  addrinfo *listAddr;
/*Store pings for recvMsg*/
extern struct s_ping_memory    pingMemory[65536];
extern struct s_round_trip  roundTripGlobal;
#endif