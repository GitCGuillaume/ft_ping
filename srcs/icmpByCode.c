#include "tools.h"
#include "ft_icmp.h"
#include "icmpCodeTools.h"

static void    displayFqdn(struct iphdr *ip, const char *ntop, ssize_t recv) {
    struct sockaddr_in  fqdn;
    char host[MAXDNAME];

    ft_memset(&fqdn, 0, sizeof(fqdn));
    ft_memset(host, 0, MAXDNAME);
    fqdn.sin_family = AF_INET;
    fqdn.sin_addr.s_addr = ip->saddr;
    const int getNameResult
        = getnameinfo((const struct sockaddr *)&fqdn, sizeof(fqdn),
            host, sizeof(host), NULL, 0, NI_NAMEREQD);
    if (getNameResult != 0)
        printf("%lu bytes from %s: ", recv, ntop);
    else
        printf("%lu bytes from %s (%s): ", recv, host, ntop);
}

/*
    code /usr/include/netinet/ip_icmp.h
    NONE = no code
    https://datatracker.ietf.org/doc/html/rfc1122#page-38
    If an ICMP message of unknown type is received, it MUST be
         silently discarded.
*/
int getIcmpCode(struct iphdr *ip, struct icmphdr *icmp,
    char *buff, ssize_t recv, const char *ntop) {
    if (!icmp || !buff)
        return (FALSE);
    struct iphdr *ipOriginal = NULL;
    struct icmphdr *icmpOriginal = NULL;
    size_t payloadSize = recv - (sizeof(*icmp) + sizeof(struct timeval));
    uint16_t resultChecksum;

    //check checksum
    resultChecksum = checksum((uint16_t *)buff, sizeof(*icmp) + sizeof(struct timeval) + payloadSize);
    buff += sizeof(struct icmphdr);
    ipOriginal = (struct iphdr *)buff;
    buff += sizeof(struct iphdr);
    icmpOriginal = (struct icmphdr *)buff;
    if (isReplyOk(ipOriginal, icmpOriginal, recv) == FALSE){
        return (FALSE);
    }
    if (icmp->type == ICMP_DEST_UNREACH || icmp->type == ICMP_SOURCE_QUENCH
        || icmp->type == ICMP_REDIRECT || icmp->type == ICMP_TIME_EXCEEDED
        || icmp->type == ICMP_PARAMETERPROB) {
        if (resultChecksum != 0) {
            printf("checksum mismatch from %s\n", ntop);
        }
        displayFqdn(ip, ntop, recv);
    }
    runCode(icmp,
        ipOriginal, icmpOriginal);
    return (TRUE);
}