#ifndef TOOLS_BONUS_H
# define TOOLS_BONUS_H

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include "libft.h"

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif
#ifndef MAXDNAME
    #define MAXDNAME 1025
#endif
#define STOP 2
#define RELOOP 2
#define NONE 20
#define ICMP 1
#define STDERR 2
#define EPSILON 1.e-10

/*
    dup == when packet is read for the first time, set it to TRUE
    if dup is already on TRUE, then it's a duplicate packet
*/
struct s_ping_memory {
    struct icmphdr  icmp;
    char    dup;
};
/*[0]=min [1]=max*/
struct s_round_trip {
    double  rtt[2];
    double  sum;
    double  squareSum;
    size_t  number;
    size_t  packetReceive;
    size_t  packetDuplicate;
    size_t  packetSend;
};

void    sigHandlerInt(int sigNum);
void    signalEnd(void);
void    exitInet(void);
void    timerFlagExit(struct timeval *tvB, struct timeval cpyGlobal);
uint16_t    checksum(uint16_t *hdr, size_t len);
void    switchFlags(char *argv[], int pos, int i, int len);
int    similarFlags(int same[8], const char *memory);

extern int fdSocket;
extern struct  addrinfo *listAddr;
/*Store pings for recvMsg*/
extern struct s_ping_memory    pingMemory[65536];
extern struct s_round_trip  roundTripGlobal;
extern volatile sig_atomic_t   end;
#endif