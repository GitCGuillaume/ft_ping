#ifndef FT_ICMP_H
# define FT_ICMP_H

#include "tools.h"
#include "netinet/ip_icmp.h"

void    runIcmp(struct addrinfo *client, int fdSocket);

#endif