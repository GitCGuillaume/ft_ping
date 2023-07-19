#ifndef FT_ICMP_H
# define FT_ICMP_H

#include "netinet/ip_icmp.h"
#include <sys/time.h>
#include "tools.h"

#ifndef ECHO_REQUEST_B_SENT
# define ECHO_REQUEST_B_SENT 56
#endif
#ifndef BUFF2_SIZE
# define BUFF2_SIZE 56
#endif

uint16_t    checksum(uint16_t *hdr, size_t len);
void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump);
void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump);
void    runIcmp(/*struct addrinfo *client*/);

#endif