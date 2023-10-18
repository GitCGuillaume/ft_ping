#include "ft_icmp.h"
#include "tools.h"

/*
void printBits(unsigned int num)
{
   for(unsigned int bit=0;bit< sizeof(unsigned int*); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}*/
void printBits2(uint16_t num)
{
   for(uint16_t bit=0;bit< 16; bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}

/* Display response from target using IPv4 and Icmp structure */
void    displayResponse(struct iphdr *ip, struct icmphdr *icmp,
    struct s_ping_memory *ping, struct timeval *tvA, struct timeval *tvB,
    struct sockaddr_in *translate) {
    if (!ip || !icmp)
        exitInet();
    //uint8_t idSend = convertEndianess(ping->icmp.un.echo.id);
    //uint8_t seqSend = convertEndianess(ping->icmp.un.echo.sequence);
    //uint8_t idRequest = icmp->un.echo.id;
    //uint8_t seqRequest = icmp->un.echo.sequence;
    //Compare host ping Id/Request with client ping
    if (translate->sin_addr.s_addr != ip->saddr)
        return ;
    //if (idSend != idRequest/* || seqSend != seqRequest*/)
    //    return ;
    double milliSeconds = 0.000000;
    uint16_t icmpSequence = icmp->un.echo.sequence;// & 0x00FF;

    ++roundTripGlobal.number;
    if (tvB->tv_sec && tvB->tv_usec) {
        time_t seconds = tvA->tv_sec - tvB->tv_sec;
        suseconds_t microSeconds = tvA->tv_usec - tvB->tv_usec;
        milliSeconds = (seconds*1000.000000) + (microSeconds / 1000.000000);

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
        //printf("seq: %u\n", icmp.un.echo.sequence);
        ping = &pingMemory[icmp.un.echo.sequence];
        if (!ping)
            return ;   
        uint16_t initialId = convertEndianess(pingMemory[0].icmp.un.echo.id);
        uint16_t idRequest = icmp.un.echo.id;
        //printf("sans convert: %u\nidS: %u idR: %u\n", ping->icmp.un.echo.id, initialId, idRequest);
       // printf("id: %u idS: %u idR: %u\n", ping->icmp.un.echo.id,
         //   pingMemory[0].icmp.un.echo.id, idRequest);
        if (initialId != idRequest)
            return ;
    }
    //printf("qqqqq%s %s\n", inet_ntop(AF_INET, &ip.saddr, str3, INET_ADDRSTRLEN),
    //    inet_ntop(AF_INET, &ip.daddr, str4, INET_ADDRSTRLEN));
    //printf("id: %u seq: %u\n", icmp.un.echo.id, icmp.un.echo.sequence);
    //printf("Pid: %u Pseq: %u\n", convertEndianess(ping->icmp.un.echo.id),
    //    convertEndianess(ping->icmp.un.echo.sequence));
    //printf("ID: %u seq: %u\n", icmp.un.echo.id, icmp.un.echo.sequence);
    //printBits2(icmp.un.echo.id);
    //printf("PID: %u Pseq: %u\n", convertEndianess(ping->icmp.un.echo.id),
     //   convertEndianess(ping->icmp.un.echo.sequence));
     //   printf("\n");
    //const char *ntop = inet_ntop(AF_INET, &ip.saddr, str, INET_ADDRSTRLEN);
    //if (!ntop)
    //    exitInet();
    const char *ntop = inet_ntop(AF_INET, &ip.saddr, dest, INET_ADDRSTRLEN);
    if (!ntop)
        exitInet();
    //printf("allo: %s dest: %s\n", ntop, dest);
    if (resultChecksum != 0 && icmp.type != 3)
            printf("checksum mismatch from %s\n", ntop);
    if (icmp.type != 8) {// if (icmp.type == 0) {
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
        //printf("allo: %s\n", ntop);
        //printf("res: %d str: %s serv: %s\n", getNameResult, host, serv);
        if (getNameResult != 0)
            exitInet();
        printf("%lu bytes from %s (%s): ", recv, host, ntop);
    } else {
        printf("%lu bytes from %s: ", recv, ntop);
    }
    //printf("resultC: %x chk: %x\n", resultChecksum, icmp.checksum);
    //if (resultChecksum != 0) {
        //printf("resultChecksum: %hu\ncode: icmp.code: %hu type: %hu\n", resultChecksum, icmp.code, icmp.type);
        
    //    getIcmpCode(&ip, &icmp, translate, buff, recv);
    //} else {
    if (icmp.type == 0) {
        struct timeval *tvB = NULL;

        if (!ping)
            return ;
        buff += sizeof(struct icmphdr);
        //-8 mean remove icmp header size
        //uint16_t checksumOriginal = checksum((uint16_t *)&ping->tvB, sizeof(struct timeval));
        //uint16_t checksumTimevalBuffer = checksum((uint16_t *)buff, sizeof(struct timeval));
        //buff += 16;
        tvB = (struct timeval *)buff;
        buff += 16;
        //bitMask(&tvB.tv_sec, 0xFF00, buff, 8, 0);
        //bitMask(&tvB.tv_usec, 0xFF, buff, 0, 1);
        recv -= sizeof(struct timeval) + sizeof(struct icmphdr);
        /*if (checksumOriginal != checksumTimevalBuffer) {
            return ;
        }*/
        for (int i = 0; i < recv; i++) {
            if (buff[i] != i)
                return ;
        }
        displayResponse(&ip, &icmp, ping, tvA, tvB, translate);
    }
    else
        getIcmpCode(&ip, &icmp, translate, buff, recv);
    printf("\n");
    //}
    //ft_memset(&pingMemory[icmp.un.echo.sequence], 0, sizeof(struct s_ping_memory));
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
    //printf("result: %u\n", result);
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
    //ft_memset(&pingMemory[cpyI].icmp, 0, sizeof(struct icmphdr));
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    pingMemory[cpyI].icmp.type = ICMP_ECHO;
    pingMemory[cpyI].icmp.code = 0;
    pingMemory[cpyI].icmp.un.echo.id = htons(getpid());
    pingMemory[cpyI].icmp.un.echo.sequence = htons(roundTripGlobal.packetSend);
   // printf("del: %u %u\n", pingMemory[cpyI].icmp.un.echo.id, pingMemory[cpyI].icmp.un.echo.sequence);
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
void    runIcmp(struct s_flags *t_flags) {
    if (!listAddr)
        exitInet();
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    struct timeval tvB;
    char buff[ECHO_REQUEST_SIZE];
    char str[16];
    int result = -1;

    if (signal(SIGALRM, sigHandlerAlrm) == SIG_ERR)
        exitInet();
    //init vars part
    ft_memset(str, 0, 16);
    //ft_memset(&pingMemory[0].icmp, 0, sizeof(struct icmphdr));
    //ft_memset(&pingMemory[0].rv, 0, sizeof(struct icmphdr));
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    pingMemory[0].icmp.type = ICMP_ECHO;
    pingMemory[0].icmp.code = 0;
  //  printf("s: %lu\n", sizeof(get));
    pingMemory[0].icmp.un.echo.id = htons(getpid());
    pingMemory[0].icmp.un.echo.sequence = htons(roundTripGlobal.packetSend);
   // printf("del: %u %u\n", pingMemory[0].icmp.un.echo.id, pingMemory[0].icmp.un.echo.sequence);
    pingMemory[0].icmp.checksum = 0;
    //get time before substract
    const char *inetResult = inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN);
    if (!inetResult) {
        exitInet();
    }
    printf("PING %s (%s): %lu data bytes", listAddr->ai_canonname,
        inetResult, ECHO_REQUEST_SIZE - sizeof(struct icmphdr));
    //jump line or display -v id option part
    if (t_flags->v == FALSE)
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