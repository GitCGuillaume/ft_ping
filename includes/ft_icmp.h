#ifndef FT_ICMP_H
# define FT_ICMP_H

#include "netinet/ip_icmp.h"
#include <sys/time.h>
#include "tools.h"

#ifndef ECHO_REQUEST_SIZE
# define ECHO_REQUEST_SIZE 64 /* -s The default is 56 */
#endif
#ifndef ECHO_REPLY_SIZE
# define ECHO_REPLY_SIZE 84 /* which translates into 64 ICMP data bytes, taking the 8 bytes of ICMP header data into account.*/
#endif

uint16_t    checksum(uint16_t *hdr, size_t len);
void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump);
void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump);
uint16_t    convertEndianess(uint16_t echoVal);
void    runIcmp();
void    getIcmpCode(struct iphdr *ip, struct icmphdr *icmp,
    struct sockaddr_in *translate, char *buff, ssize_t recv);
void    parseIp(struct iphdr *ip, char *buff);
void    parseIcmp(struct icmphdr  *icmp, char *buff);

#endif