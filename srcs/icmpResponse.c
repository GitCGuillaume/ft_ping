#include "ft_icmp.h"
#include "tools.h"

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

/*to calculate ctrl+c values*/
static void    countRoundTrip(double *milliSeconds, struct timeval *tvA,
    struct timeval *tvB) {
    ++roundTripGlobal.number;
    if (tvB->tv_sec && tvB->tv_usec) {
        time_t seconds = tvA->tv_sec - tvB->tv_sec;
        suseconds_t microSeconds = tvA->tv_usec - tvB->tv_usec;
        *milliSeconds = (seconds * 1000.000000) + (microSeconds / 1000.000000);

        roundTripGlobal.sum += *milliSeconds;
        roundTripGlobal.squareSum += (*milliSeconds * *milliSeconds);
        if (*milliSeconds < roundTripGlobal.rtt[0] || roundTripGlobal.rtt[0] == 0)
            roundTripGlobal.rtt[0] = *milliSeconds;
        if (roundTripGlobal.rtt[1] < *milliSeconds)
            roundTripGlobal.rtt[1] = *milliSeconds;
    }
}

/* Display response from target using IPv4 and Icmp structure */
static void    displayResponse(struct iphdr *ip, struct icmphdr *icmp,
    struct s_ping_memory *ping, struct timeval *tvA,
    struct timeval *tvB) {
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;

    if (!ip || !icmp || !tvA || !tvB)
        exitInet();
    //Compare host ping Id/Request with client ping
    if (translate->sin_addr.s_addr != ip->saddr)
        return ;
    double milliSeconds = 0.000000;
    uint16_t icmpSequence = icmp->un.echo.sequence;
    countRoundTrip(&milliSeconds, tvA, tvB);
    printf("icmp_seq=%u ttl=%u",
        icmpSequence, ip->ttl);
    if (tvB->tv_sec && tvB->tv_usec)
        printf(" time=%.3f ms", milliSeconds);
    if (ping->dup == TRUE) {
        printf("%s", " (DUP!)");
        roundTripGlobal.packetDuplicate += 1;
    } else {
        roundTripGlobal.packetReceive += 1;
    }
    ping->dup = TRUE;
}

static void    icmpReponse(struct iphdr *ip, struct icmphdr *icmp,
    ssize_t recv, struct timeval *tvA,
    char *buff) {
    struct timeval *tvB = NULL;
    struct s_ping_memory *ping = 0;
    char dest[INET_ADDRSTRLEN];
    uint16_t resultChecksum;

    ft_memset(dest, 0, INET_ADDRSTRLEN);
    const char *ntop = inet_ntop(AF_INET, &ip->saddr, dest, INET_ADDRSTRLEN);
    if (!ntop)
        exitInet();
    if (icmp->type != 0) {
        displayTimeExceed(ip, ntop, recv);
        getIcmpCode(icmp, buff, recv);
    } else {
        //check checksum
        resultChecksum = checksum((uint16_t *)buff, sizeof(*icmp) + sizeof(struct timeval) + 40);
        if (resultChecksum != 0)
            printf("checksum mismatch from %s\n", ntop);
        printf("%lu bytes from %s: ", recv, ntop);
        ping = &pingMemory[icmp->un.echo.sequence];
        if (!ping)
            return ;
        //check if payload has some issues
        buff += sizeof(struct icmphdr);
        tvB = (struct timeval *)buff;
        buff += 16;
        recv -= sizeof(struct timeval) + sizeof(struct icmphdr);
        for (int i = 0; i < recv; i++) {
            if (buff[i] != i)
                return ;
        }
        displayResponse(ip, icmp, ping, tvA, tvB);
    }
    printf("\n");
}

/*
    Start of reading ping response from targetted client
    Source : https://en.wikipedia.org/wiki/Checksum
     To validate a message,
        the receiver adds all the words in the same manner,
        including the checksum;
        if the result is not a word full of zeros, an error must have occurred
*/
void    icmpInitResponse(struct msghdr *msg, ssize_t recv,
    struct timeval *tvA) {
    if (!msg)
        exitInet();
    struct iovec *iov = msg->msg_iov;
    struct iphdr    ip;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;
    struct s_ping_memory *ping = 0;

    ft_memset(&ip, 0, sizeof(ip));
    ft_memset(&icmp, 0, sizeof(icmp));
    /* Get IPv4 from buffer  */
    parseIp(&ip, buff);
    buff += 20;
    /* Get Icmp from buffer */
    parseIcmp(&icmp, buff);
    if (icmp.type > 18 || icmp.type == 8)
        return ;
    if (icmp.type == 0 && icmp.code == 0) {
        ping = &pingMemory[icmp.un.echo.sequence];
        if (!ping)
            return ;
        const uint16_t initialId = convertEndianess(pingMemory[0].icmp.un.echo.id);
        const uint16_t idRequest = icmp.un.echo.id;
        if (initialId != idRequest)
            return ;
    }
    icmpReponse(&ip, &icmp, recv,
        tvA, buff);
}