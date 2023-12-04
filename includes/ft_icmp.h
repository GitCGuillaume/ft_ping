#ifndef FT_ICMP_H
# define FT_ICMP_H

#include "netinet/ip_icmp.h"
#include <sys/time.h>
#include "tools.h"

#ifndef ICMP_ECHO
# define ICMP_ECHO 8
#endif
#ifndef ECHO_REQUEST_SIZE
# define ECHO_REQUEST_SIZE 64 /* -s The default is 56 */
#endif
#ifndef ECHO_REPLY_SIZE
# define ECHO_REPLY_SIZE 65536 /* which translates into 64 ICMP data bytes, taking the 8 bytes of ICMP header data into account.*/
#endif

void    runIcmp(void);
int getIcmpCode(struct iphdr *ip, struct icmphdr *icmp,
    char *buff, ssize_t recv, const char *ntop);
void    parseIp(struct iphdr *ip, char *buff);
void    parseIcmp(struct icmphdr  *icmp, char *buff);
//icmpReponse.c
void    icmpInitResponse(struct msghdr *msg, ssize_t recv,
    struct timeval *tvA);

#endif