#include "ft_icmp.h"
#include "tools.h"

/* Display response from target using IPv4 and Icmp structure */
void    displayResponse(struct iphdr *ip, struct icmphdr *icmp,
    struct s_ping_memory *ping, struct timeval *tvA, struct timeval *tvB,
    struct sockaddr_in *translate) {
    if (!ip || !icmp || !tvA || !tvB)
        exitInet();
    //Compare host ping Id/Request with client ping
    if (translate->sin_addr.s_addr != ip->saddr)
        return ;
    double milliSeconds = 0.000000;
    uint16_t icmpSequence = icmp->un.echo.sequence;// & 0x00FF;

    ++roundTripGlobal.number;
    if (tvB->tv_sec && tvB->tv_usec) {
        time_t seconds = tvA->tv_sec - tvB->tv_sec;
        suseconds_t microSeconds = tvA->tv_usec - tvB->tv_usec;
        milliSeconds = (seconds * 1000.000000) + (microSeconds / 1000.000000);

        roundTripGlobal.sum += milliSeconds;
        roundTripGlobal.squareSum += (milliSeconds * milliSeconds);
        if (milliSeconds < roundTripGlobal.rtt[0] || roundTripGlobal.rtt[0] == 0)
            roundTripGlobal.rtt[0] = milliSeconds;
        if (roundTripGlobal.rtt[1] < milliSeconds)
            roundTripGlobal.rtt[1] = milliSeconds;
    }
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

/*
    Start of reading ping response from targetted client
    Source : https://en.wikipedia.org/wiki/Checksum
     To validate a message,
        the receiver adds all the words in the same manner,
        including the checksum;
        if the result is not a word full of zeros, an error must have occurred
*/
void    icmpResponse(struct msghdr *msg, ssize_t recv,
    struct timeval *tvA) {
    if (!msg)
        exitInet();
    struct iovec *iov = msg->msg_iov;
    struct iphdr    ip;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    uint16_t resultChecksum;
    struct s_ping_memory *ping = 0;
    char dest[INET_ADDRSTRLEN];
    
    ft_memset(dest, 0, INET_ADDRSTRLEN);
    ft_memset(&ip, 0, sizeof(ip));
    ft_memset(&icmp, 0, sizeof(icmp));
    /* Get IPv4 from buffer  */
    parseIp(&ip, buff);
    buff += 20;
    /* Get Icmp from buffer */
    resultChecksum = checksum((uint16_t *)buff, sizeof(icmp) + sizeof(struct timeval) + 40);
    parseIcmp(&icmp, buff);
    if (icmp.type > 18 || icmp.type == 8)
        return ;
    if (icmp.type == 0 && icmp.code == 0) {
        ping = &pingMemory[icmp.un.echo.sequence];
        if (!ping)
            return ;
        uint16_t initialId = convertEndianess(pingMemory[0].icmp.un.echo.id);
        uint16_t idRequest = icmp.un.echo.id;
        if (initialId != idRequest)
            return ;
    }
    const char *ntop = inet_ntop(AF_INET, &ip.saddr, dest, INET_ADDRSTRLEN);
    if (!ntop)
        exitInet();
    if (resultChecksum != 0 && icmp.type != 3)
            printf("checksum mismatch from %s\n", ntop);
    if (icmp.type != 0) {// if (icmp.type == 0) {
        struct sockaddr_in  fqdn;
        char host[FQDN_MAX];
        char serv[FQDN_MAX];

        ft_memset(&fqdn, 0, sizeof(fqdn));
        ft_memset(host, 0, FQDN_MAX);
        ft_memset(serv, 0, FQDN_MAX);
        fqdn.sin_family = AF_INET;
        fqdn.sin_addr.s_addr = ip.saddr;
        const int getNameResult
            = getnameinfo((const struct sockaddr *)&fqdn, sizeof(fqdn),
                host, sizeof(host), serv, sizeof(serv), NI_NAMEREQD);
        if (getNameResult != 0)
            printf("%lu bytes from %s: ", recv, ntop);
        else
            printf("%lu bytes from %s (%s): ", recv, host, ntop);
    } else {
        printf("%lu bytes from %s: ", recv, ntop);
    }
    if (icmp.type == 0) {
        struct timeval *tvB = NULL;

        if (!ping)
            return ;
        buff += sizeof(struct icmphdr);
        tvB = (struct timeval *)buff;
        buff += 16;
        recv -= sizeof(struct timeval) + sizeof(struct icmphdr);
        for (int i = 0; i < recv; i++) {
            if (buff[i] != i)
                return ;
        }
        displayResponse(&ip, &icmp, ping, tvA, tvB, translate);
    }
    else{
        getIcmpCode(&icmp, translate, buff, recv);
    }
    printf("\n");
}

/* Get request response */
void    icmpRequest() {
    char buff2[ECHO_REPLY_SIZE];
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvA;
    int result = -1;

    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff2, 0, ECHO_REPLY_SIZE);
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    result = recvmsg(fdSocket, &msgResponse, 0);
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    if (gettimeofday(&tvA, 0) < 0)
        exitInet();
    //result - sizeof(struct iphdr) = whole response minus ip header (84-20)=64 
    //echo reply is 64 like a standard reply from the ping inetutils2.0
    icmpResponse(&msgResponse, result - sizeof(struct iphdr), &tvA);
}

/* send ping using signal alarm */
void    sigHandlerAlrm(int sigNum) {
    int cpyI = roundTripGlobal.packetSend;

    if (sigNum != SIGALRM)
        return ;
    if (!listAddr)
        exitInet();
    struct timeval tvB;
    char buff[ECHO_REQUEST_SIZE];
    int result = -1;
    
    //init part
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    pingMemory[cpyI].icmp.type = ICMP_ECHO;
    pingMemory[cpyI].icmp.code = 0;
    pingMemory[cpyI].icmp.un.echo.id = htons(getpid());
    pingMemory[cpyI].icmp.un.echo.sequence = htons(roundTripGlobal.packetSend);
    pingMemory[cpyI].icmp.checksum = 0;
    if (gettimeofday(&tvB, 0) < 0) {
        exitInet();
    }
    ft_memcpy(buff, &pingMemory[cpyI].icmp, sizeof(pingMemory[cpyI].icmp));
    ft_memcpy(buff + sizeof(pingMemory[cpyI].icmp), &tvB, sizeof(tvB));
    uint8_t j = sizeof(pingMemory[cpyI].icmp) + sizeof(tvB);
    const uint8_t max = j + 40;
    char value = 0;
    for (; j < max; ++j)
        buff[j] = value++;
    pingMemory[cpyI].icmp.checksum
        = checksum((uint16_t *)buff, sizeof(pingMemory[cpyI].icmp) + sizeof(tvB) + 40);
    ft_memcpy(buff, &pingMemory[cpyI].icmp, sizeof(pingMemory[cpyI].icmp));
    roundTripGlobal.packetSend++;
    result = sendto(fdSocket, buff,
        ECHO_REQUEST_SIZE, 0,
        listAddr->ai_addr, sizeof(*listAddr->ai_addr));
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    //Call another ping
    alarm(1);
}

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/

/* (ipv4 max)65535 - (sizeof ip)20 - (sizeof icmp)8 */
void    runIcmp() {
    if (!listAddr)
        exitInet();
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    struct timeval tvB;
    char buff[ECHO_REQUEST_SIZE];
    char str[INET_ADDRSTRLEN];
    int result = -1;

    if (signal(SIGALRM, sigHandlerAlrm) == SIG_ERR)
        exitInet();
    //init vars part
    ft_memset(str, 0, INET_ADDRSTRLEN);
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    pingMemory[0].icmp.type = ICMP_ECHO;
    pingMemory[0].icmp.code = 0;
    pingMemory[0].icmp.un.echo.id = htons(getpid());
    pingMemory[0].icmp.un.echo.sequence = htons(roundTripGlobal.packetSend);
    pingMemory[0].icmp.checksum = 0;
    //get time before substract
    const char *inetResult = inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN);
    if (!inetResult) {
        exitInet();
    }
    printf("PING %s (%s): %lu data bytes", listAddr->ai_canonname,
        inetResult, ECHO_REQUEST_SIZE - sizeof(struct icmphdr));
    //jump line or display -v id option part
    if (t_flags.v == FALSE)
        printf("\n");
    else
        printf(", id 0x%04x = %u\n",
            convertEndianess(pingMemory[0].icmp.un.echo.id),
            convertEndianess(pingMemory[0].icmp.un.echo.id));
    if (gettimeofday(&tvB, 0) < 0) {
        exitInet();
    }
    ft_memcpy(buff, &pingMemory[0].icmp, sizeof(pingMemory[0].icmp));
    ft_memcpy(buff + sizeof(pingMemory[0].icmp), &tvB, sizeof(tvB));
    uint8_t j = sizeof(pingMemory[0].icmp) + sizeof(tvB);
    const uint8_t max = j + 40;
    char value = 0;
    for (; j < max; ++j)
        buff[j] = value++;
    pingMemory[0].icmp.checksum
        = checksum((uint16_t *)buff, sizeof(pingMemory[0].icmp) + sizeof(tvB) + 40);
    ft_memcpy(buff, &pingMemory[0].icmp, sizeof(pingMemory[0].icmp));
    roundTripGlobal.packetSend++;
    result = sendto(fdSocket, buff,
        ECHO_REQUEST_SIZE, 0,
        listAddr->ai_addr, sizeof(*listAddr->ai_addr));
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    //Call another ping
    alarm(1);
    while (1) {
        icmpRequest();
        usleep(50);
    }
}