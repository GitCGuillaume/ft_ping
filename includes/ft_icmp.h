#ifndef FT_ICMP_H
# define FT_ICMP_H

#include "netinet/ip_icmp.h"
#include <sys/time.h>
#include "tools.h"

#ifndef ECHO_REQUEST_B_SENT
# define ECHO_REQUEST_B_SENT 56
#endif

void    runIcmp(/*struct addrinfo *client*/);

extern struct s_flags t_flags;
extern struct  addrinfo *listAddr;

#endif