#ifndef ICMP_CODE_TOOLS_H
# define ICMP_CODE_TOOLS_H

#include <stdio.h>
#include <stdint.h>
#include "netinet/ip_icmp.h"

#ifndef NONE
    #define NONE 20
#endif

void    headerDumpIp(struct iphdr *ip, int special);
void    headerDumpData(struct icmphdr *icmp, uint16_t size);
unsigned char isReplyOk(struct iphdr *ipOriginal, struct icmphdr *icmpOriginal,
    ssize_t recv);
void    runCode(struct icmphdr *icmp, struct iphdr *ipOriginal,
    struct icmphdr *icmpOriginal);

#endif