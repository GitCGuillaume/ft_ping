#ifndef TOOLS_BONUS_H
# define TOOLS_BONUS_H

#include <errno.h>
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
    char    v;
    char    interrogation;
    uint32_t    tos;
    uint32_t    ttl;
    uint32_t    w;
    uint32_t    preload;
};

/*
    dup == when packet is read for the first time, set it to TRUE
    if dup is already on TRUE, then it's a duplicate packet
*/
struct s_ping_memory {
    struct icmphdr  icmp;
    //struct timeval tvB;
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
    //uint32_t    packetReq;
};

void    sigHandlerInt(int sigNum);
void    exitInet(void);
void    exitTimer(int code);
void    timerFlagExit(struct timeval *tvB, struct timeval cpyGlobal);
void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump);
void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump);
uint16_t    convertEndianess(uint16_t echoVal);
uint16_t    checksum(uint16_t *hdr, size_t len);
uint16_t    convertEndianess(uint16_t echoVal);

extern int fdSocket;
extern struct s_flags t_flags;
extern struct  addrinfo *listAddr;
/*Store pings for recvMsg*/
extern struct s_ping_memory    pingMemory[65536];
extern struct s_round_trip  roundTripGlobal;
#endif