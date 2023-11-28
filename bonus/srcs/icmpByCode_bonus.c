#include "tools_bonus.h"
#include "ft_icmp_bonus.h"
#include "icmpCodeTools_bonus.h"

static void    destUnreach(uint8_t code) {
    const char *arr[16] = {
        "Destination Net Unreachable", "Destination Host Unreachable", "Destination Protocol Unreachable",
        "Destination Port Unreachable", "Fragmentation needed and DF set", "Source Route Failed",
        "Network Unknown", "Host Unknown", "Host Isolated",
        "Communication With Network Is Adminitratively Prohibited",
        "Communication With Host Is Adminitratively Prohibited",
        "Destination Network Unreachable At This TOS",
        "Destination Host Unreachable At This TOS",
        "Packet Filtered", "Precedence Violation", "Precedence Cutoff"
    };
    int i;

    for (i = 0; i < 16; ++i) {
        if (i == code)
            break ;
    }
    if (i != 16)
        printf("%s", arr[i]);
}

/* Deprecated */
static void    sourceQuench(uint8_t code) {
    if (code != 0)
        return ;
    printf("%s", "Source Quench");
}

static void    redirect(uint8_t code) {
    if (code == 0)
        printf("%s", "Redirect Network");
    else if (code == 1)
        printf("%s", "Redirect Host");
    else if (code == 2)
        printf("%s", "Type of Service and Network");
    else if (code == 3)
        printf("%s", "Type of Service and Host");
}

static void    timeExceed(uint8_t code) {
    const char  *arr[2] = {
        "Time to live exceeded",
        "Frag reassembly time exceeded"
    };
    int i;

    for (i = 0; i < 2; ++i) {
        if (i == code)
            break ;
    }
    if (i != 2)
        printf("%s", arr[i]);
}

static void    paramProb(uint8_t code) {
    if (code == 0)
        printf("%s", "Pointer indicate the error");
    else if (code == 1)
        printf("%s", "Required option is missing");
    else if (code == 2)
        printf("%s", "Bad length");
}

static void    displayTimeExceed(struct iphdr *ip, const char *ntop, ssize_t recv) {
    struct sockaddr_in  fqdn;
    char host[FQDN_MAX];
    char serv[FQDN_MAX];

    ft_memset(&fqdn, 0, sizeof(fqdn));
    ft_memset(host, 0, FQDN_MAX);
    ft_memset(serv, 0, FQDN_MAX);
    fqdn.sin_family = AF_INET;
    fqdn.sin_addr.s_addr = ip->saddr;
    const int getNameResult
        = getnameinfo((const struct sockaddr *)&fqdn, sizeof(fqdn),
            host, sizeof(host), serv, sizeof(serv), NI_NAMEREQD);
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
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    if (!icmp || !translate || !buff)
        return (FALSE);
    struct iphdr ipOriginal;
    struct icmphdr icmpOriginal;

    ft_memset(&ipOriginal, 0, sizeof(struct iphdr));
    ft_memset(&icmpOriginal, 0, sizeof(struct icmphdr));
    if (icmp->type != 12)
        buff += sizeof(struct icmphdr);
    parseIp(&ipOriginal, buff);
    buff += sizeof(struct iphdr);
    parseIcmp(&icmpOriginal, buff);
    if (isReplyOk(&ipOriginal, &icmpOriginal, translate, recv) == FALSE){
        return (FALSE);
    }
    displayTimeExceed(ip, ntop, recv);
    unsigned int types[19] = {
        NONE, NONE, NONE,
        ICMP_DEST_UNREACH, ICMP_SOURCE_QUENCH,
        ICMP_REDIRECT, NONE,NONE, NONE,
        NONE, NONE, ICMP_TIME_EXCEEDED,
        ICMP_PARAMETERPROB, ICMP_TIMESTAMP, ICMP_TIMESTAMPREPLY,
        ICMP_INFO_REQUEST, ICMP_INFO_REPLY, ICMP_ADDRESS,
        ICMP_ADDRESSREPLY
    };
    //list of function to call from macro number
    void    *functionArray[19] = {
        NULL, NULL, NULL,
        &destUnreach, &sourceQuench, &redirect,
        NULL, NULL, NULL, NULL, NULL,
        &timeExceed, &paramProb, NULL, NULL,
        NULL, NULL, NULL, NULL
    };
    void    (*functionCall)(uint8_t) = NULL;
    unsigned int i;

    for (i = 0; i < 20; ++i) {
        if (icmp->type == types[i]
            && types[i] != NONE) {
            functionCall = functionArray[i];
            break ;
        }
    }
    if (functionCall)
           functionCall(icmp->code);
    if (t_flags.v == TRUE) {
        headerDumpIp(&ipOriginal);
        headerDumpData(&icmpOriginal,
            ipOriginal.tot_len - sizeof(struct iphdr));
    }
    return (TRUE);
}