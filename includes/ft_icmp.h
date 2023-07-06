#ifndef FT_ICMP_H
# define FT_ICMP_H

#include "tools.h"
#include "netinet/ip_icmp.h"

void    runIcmp(struct addrinfo *client);

extern struct s_flags t_flags;

#endif