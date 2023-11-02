#ifndef FT_ICMP_BONUS_H
# define FT_ICMP_BONUS_H

#include "netinet/ip_icmp.h"
#include <sys/time.h>
#include "tools_bonus.h"

#ifndef ICMP_ECHO
# define ICMP_ECHO 8
#endif
#ifndef ECHO_REQUEST_SIZE
# define ECHO_REQUEST_SIZE 64 /* -s The default is 56 */
#endif
#ifndef ECHO_REPLY_SIZE
# define ECHO_REPLY_SIZE 84 /* which translates into 64 ICMP data bytes, taking the 8 bytes of ICMP header data into account.*/
#endif

void    runIcmp();
void getIcmpCode(struct icmphdr *icmp, char *buff, ssize_t recv);
void    parseIp(struct iphdr *ip, char *buff);
void    parseIcmp(struct icmphdr  *icmp, char *buff);
//icmpReponse.c
void    icmpInitResponse(struct msghdr *msg, ssize_t recv,
    struct timeval *tvA);

#endif