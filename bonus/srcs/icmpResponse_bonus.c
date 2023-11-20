#include "ft_icmp_bonus.h"
#include "tools_bonus.h"

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

static struct timeval *icmpReponse(struct iphdr *ip, struct icmphdr *icmp,
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
            return (NULL);
        //check if payload has some issues
        buff += sizeof(struct icmphdr);
        tvB = (struct timeval *)buff;
        buff += 16;
        recv -= sizeof(struct timeval) + sizeof(struct icmphdr);
        for (int i = 0; i < recv; i++) {
            if (buff[i] != i)
                return (NULL);
        }
        displayResponse(ip, icmp, ping, tvA, tvB);
    }
    printf("\n");
    return (tvB);
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
    struct iphdr    *ip = NULL;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;
    struct s_ping_memory *ping = 0;

    //ft_memset(&ip, 0, sizeof(ip));
    //ft_memset(&icmp, 0, sizeof(icmp));
    /* Get IPv4 from buffer  */
    //parseIp(&ip, buff);
    ip = (struct iphdr *)buff;
    buff += 20;
    /* Get Icmp from buffer */
    parseIcmp(&icmp, buff);
    if (icmp.type > 18 || icmp.type == 8)
        return ;//(RELOOP);
    if (icmp.type == 0 && icmp.code == 0) {
        ping = &pingMemory[icmp.un.echo.sequence];
        if (!ping)
            return ;//(RELOOP);
        const uint16_t initialId = convertEndianess(pingMemory[0].icmp.un.echo.id);
        const uint16_t idRequest = icmp.un.echo.id;
        if (initialId != idRequest)
            return ;//(RELOOP);
    }
    struct timeval *tvB = icmpReponse(ip, &icmp, recv,
        tvA, buff);
    if (!tvB)
        return ;//(RELOOP);
    if (icmp.type != 8) {// && !t_flags.preload) {
        //while (!end && !interrupt) {}
            //usleep(1);
        //interrupt = FALSE;
        //printf("pre:%d\n", t_flags.preload);
        --t_flags.preload;
        if (t_flags.preload != -1) {
            return ;//(RELOOP);
        }
        t_flags.preload = 0;
        
        struct timeval timeout;
        //cut interval sec / microseconds
        long it_sec = (long)t_flags.interval;
        long it_usec = (t_flags.interval - (long)t_flags.interval) * 1000000.0f;

        gettimeofday(&timeout, NULL);
        printf("sec:%ld it_usec:%ld timeout_sec: %ld timeout_usec: %ld\n", it_sec, it_usec, timeout.tv_sec - tvB->tv_sec, timeout.tv_usec - tvB->tv_usec);
        //exit(1);
        timeout.tv_sec = it_sec  - (timeout.tv_sec - tvB->tv_sec);
        timeout.tv_usec = it_usec - (timeout.tv_usec - tvB->tv_usec);
        printf("sec: %ld usec: %ld\n", timeout.tv_sec, timeout.tv_usec);
        if (setsockopt(fdSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0) {
            dprintf(2, "%s", "Couldn't set option RCVTIMEO socket.\n");
            exitInet();
        }
    exit(1);
        /*ssize_t time = ((timeout.tv_sec - tvB->tv_sec) * 1000000)
            + ((timeout.tv_usec - tvB->tv_usec));
        while (!end && !interrupt && time < t_flags.time) {
            gettimeofday(&timeout, NULL);
            time = ((timeout.tv_sec - tvB->tv_sec) * 1000000)
                + ((timeout.tv_usec - tvB->tv_usec));
        }*/
    }// else {
    //    t_flags.preload--;
    //}
    return ;//(TRUE);
    //printf("p:%d\n", t_flags.preload);
        //usleep(1000000);
}
/*
struct timeval new;
float test = t_flags.interval - *milliSeconds;
printf("interval: %f test:%f\n", t_flags.interval, test);
//float it_sec = (long)test;
float it_usec = test - (long)test;// * t_flags.dividend;

//   exit(1);
new.tv_usec = (long)(it_usec * 1000000.0f);//(it_usec * 1000000.0f) % 1000000;//(long)(it_usec * CONV_SEC_TO_MICR) / t_flags.dividend;//t_flags.interval - (int)t_flags.interval;
new.tv_sec = (long)test;
printf("\ntv_sec: %ld\ntv_usec: %ld\nsec: %f, msec: %f", new.tv_sec, new.tv_usec, test, it_usec);
//exit(1);
//new.tv_sec = (long)it_sec;
//new.tv_usec = (long)(it_usec * 1000000.0f) % 1000000;//(long)(it_usec * CONV_SEC_TO_MICR) / t_flags.dividend;//(int)(t_flags.interval - (int)t_flags.interval);
//if (!new.tv_sec
//    && !new.tv_usec) {
//    new.it_interval.tv_sec = 1;
//    new.tv_sec = 1;
// }
if (setsockopt(fdSocket, SOL_SOCKET, SO_RCVTIMEO, &new, sizeof(new)) != 0) {
    dprintf(2, "%s", "Couldn't set option RCVTIMEO socket.\n");
    exitInet();
}
*/