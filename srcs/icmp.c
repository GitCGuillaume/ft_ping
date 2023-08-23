#include "ft_icmp.h"
#include "tools.h"

int i = 0;

void printBits(unsigned int num)
{
   for(unsigned int bit=0;bit< sizeof(unsigned int*); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}
void printBits2(uint16_t num)
{
   for(uint16_t bit=0;bit< 16; bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}


/* Parse using mask */
void    parseIp(struct iphdr *ip, char *buff) {
    if (!ip || !buff)
        exitInet();
    ip->ihl = *buff & 0xF;
    *buff = *buff >> 4;//take half
    ip->version = *buff;
    ++buff;
    ip->tos = *buff;
    ++buff;
    bitMask(&ip->tot_len, 0xFF00, buff, 8, 0);
    bitMask(&ip->tot_len, 0xFF, buff, 0, 1);
    buff += 2;
    bitMask(&ip->id, 0xFF00, buff, 8, 0);
    bitMask(&ip->id, 0xFF, buff, 0, 1);
    buff += 2;
    bitMask(&ip->frag_off, 0xFF00, buff, 8, 0);
    bitMask(&ip->frag_off, 0xFF, buff, 0, 1);
    buff += 2;
    ip->ttl = *buff;
    ++buff;
    ip->protocol = *buff;
    ++buff;
    bitMask(&ip->check, 0xFF00, buff, 8, 0);
    bitMask(&ip->check, 0xFF, buff, 0, 1);
    buff += 2;
    //big endianness for ipv4 inet_ntop htop
    bigBitMask(&ip->saddr, 0xFF, buff, 0, 0);
    bigBitMask(&ip->saddr, 0xFF00, buff, 8, 1);
    bigBitMask(&ip->saddr, 0xFF0000, buff, 16, 2);
    bigBitMask(&ip->saddr, 0xFF000000, buff, 24, 3);
    buff += 4;
    /*bigBitMask(&ip->daddr, 0xFF, buff, 24, 0);
    bigBitMask(&ip->daddr, 0xFF00, buff, 16, 1);
    bigBitMask(&ip->daddr, 0xFF0000, buff, 8, 2);
    bigBitMask(&ip->daddr, 0xFF000000, buff, 0, 3);*/
    bigBitMask(&ip->daddr, 0xFF, buff, 0, 0);
    bigBitMask(&ip->daddr, 0xFF00, buff, 8, 1);
    bigBitMask(&ip->daddr, 0xFF0000, buff, 16, 2);
    bigBitMask(&ip->daddr, 0xFF000000, buff, 24, 3);
}

/* Parse using mask */
void    parseIcmp(struct icmphdr  *icmp, char *buff) {
    if (!icmp || !buff)
        exitInet();
    icmp->type = *buff;
    ++buff;
    icmp->code = *buff;
    ++buff;
    bitMask(&icmp->checksum, 0xFF00, buff, 8, 0);
    bitMask(&icmp->checksum, 0xFF, buff, 0, 1);
    buff += 2;
    bitMask(&icmp->un.echo.id, 0xFF00, buff, 8, 0);
    bitMask(&icmp->un.echo.id, 0xFF, buff, 0, 1);
    buff += 2;
    bitMask(&icmp->un.echo.sequence, 0xFF00, buff, 8, 0);
    bitMask(&icmp->un.echo.sequence, 0xFF, buff, 0, 1);
    buff += 2;
}

/* Display response from target using IPv4 and Icmp structure */
void    displayResponse(struct iphdr *ip, struct icmphdr *icmp,
    struct s_ping_memory *ping, struct timeval *tvA) {
    if (!ip || !icmp)
        exitInet();
    printf("aaa: %lu bbb:%lu\n", tvA->tv_usec, ping->tvB.tv_usec);
    //char str[16];
    time_t seconds = tvA->tv_sec - ping->tvB.tv_sec;
    suseconds_t microSeconds = tvA->tv_usec - ping->tvB.tv_usec;
    float milliSeconds = (seconds*1000.0000) + (microSeconds / 1000.0000);
    uint16_t icmpSequence = icmp->un.echo.sequence;// & 0x00FF;
    
    //ft_memset(str, 0, 16);
    //const char *ntop = inet_ntop(AF_INET, &ip->saddr, str, INET_ADDRSTRLEN);
    //if (!ntop)
    //    exitInet();
    printf("icmp_seq=%u ttl=%u time=%f ms\n",
        icmpSequence, ip->ttl, milliSeconds);
}

/*
    Start of reading ping response from targetted client
    Source : https://en.wikipedia.org/wiki/Checksum
     To validate a message,
        the receiver adds all the words in the same manner,
        including the checksum;
        if the result is not a word full of zeros, an error must have occurred
*/
void    icmpResponse(struct msghdr *msg, ssize_t recv) {
    if (!msg)
        exitInet();
    struct timeval tvA;
    struct iovec *iov = msg->msg_iov;
    struct iphdr    ip;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    uint16_t resultChecksum;
    struct s_ping_memory *ping = 0;
    char str[16];

    ft_memset(str, 0, 16);
    ft_memset(&ip, 0, sizeof(ip));
    ft_memset(&icmp, 0, sizeof(icmp));
    /* Get IPv4 from buffer  */
    parseIp(&ip, buff);
    buff += 20;
    /* Get Icmp from buffer */
    resultChecksum = checksum((uint16_t *)buff, sizeof(icmp) + sizeof(struct timeval) + 40);
    parseIcmp(&icmp, buff);
    if (icmp.type == 8)
        return ;
    printf("main addr: %u\n", translate->sin_addr.s_addr);
    printf("saddr: %u daddr:%u\n", ip.saddr, ip.daddr);
    ping = &pingMemory[icmp.un.echo.sequence];
    if (!ping)
        return ;
    //printf("ID: %u seq: %u\n", icmp.un.echo.id, icmp.un.echo.sequence);
    //printBits2(icmp.un.echo.id);
    //printf("PID: %u Pseq: %u\n", convertEndianess(ping->icmp.un.echo.id),
     //   convertEndianess(ping->icmp.un.echo.sequence));
     //   printf("\n");
    const char *ntop = inet_ntop(AF_INET, &ip.saddr, str, INET_ADDRSTRLEN);
    if (!ntop)
        exitInet();
    printf("%lu bytes from %s: ",
        recv, ntop);
    //printf("resultC: %x chk: %x\n", resultChecksum, icmp.checksum);
    if (gettimeofday(&tvA, 0) < 0)
        exitInet();
    //printf("icmp.type: %u", icmp.type);
    if (resultChecksum != 0) {
        printf("resultChecksum: %hu\ncode: icmp.code: %hu type: %hu\n", resultChecksum, icmp.code, icmp.type);
        printf("id: %u seq: %u\n", icmp.un.echo.id, icmp.un.echo.sequence);
        getIcmpCode(&icmp);
    } else {
        if (icmp.type == 0 && icmp.code == 0)
            displayResponse(&ip, &icmp, ping, &tvA);
        else
            getIcmpCode(&icmp);
    }
    ft_memset(&pingMemory[icmp.un.echo.sequence], 0, sizeof(struct s_ping_memory));
}

/* Ger request response */
void    icmpRequest() {
    char buff2[ECHO_REPLY_SIZE];
    struct msghdr msgResponse;
    struct iovec msg[1];
    int result = -1;

    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff2, 0, ECHO_REPLY_SIZE);
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    result = recvmsg(fdSocket, &msgResponse, 0);
    printf("result: %u\n", result);
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    icmpResponse(&msgResponse, result);
}

/* send ping using signal alarm */
void    sigHandlerAlrm(int sigNum) {
    int cpyI = i;

    if (sigNum != SIGALRM)
        return ;
    if (!listAddr)
        exitInet();
    char buff[ECHO_REQUEST_SIZE];
    int result = -1;
    
    //init part
    //ft_memset(&pingMemory[cpyI].icmp, 0, sizeof(struct icmphdr));
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    pingMemory[cpyI].icmp.type = ICMP_ECHO;
    pingMemory[cpyI].icmp.code = 0;
    pingMemory[cpyI].icmp.un.echo.id = getpid();
    pingMemory[cpyI].icmp.un.echo.sequence = htons(i);
    ++i;
    pingMemory[cpyI].icmp.checksum = 0;
    if (gettimeofday(&pingMemory[cpyI].tvB, 0) < 0) {
        exitInet();
    }
    ft_memcpy(buff, &pingMemory[cpyI].icmp, sizeof(pingMemory[cpyI].icmp));
    ft_memcpy(buff + sizeof(pingMemory[cpyI].icmp), &pingMemory[cpyI].tvB, sizeof(pingMemory[cpyI].tvB));
    uint8_t j = sizeof(pingMemory[cpyI].icmp) + sizeof(pingMemory[cpyI].tvB);
    const uint8_t max = j + 40;
    char value = 0;
    for (; j < max; ++j)
        buff[j] = value++;
    pingMemory[cpyI].icmp.checksum
        = checksum((uint16_t *)buff, sizeof(pingMemory[cpyI].icmp) + sizeof(pingMemory[cpyI].tvB) + 40);
    ft_memcpy(buff, &pingMemory[cpyI].icmp, sizeof(pingMemory[cpyI].icmp));
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
    pingMemory[0].icmp.un.echo.id = getpid();
    pingMemory[0].icmp.un.echo.sequence = htons(i);
    ++i;
    pingMemory[0].icmp.checksum = 0;
    //get time before substract
    if (gettimeofday(&pingMemory[0].tvB, 0) < 0) {
        exitInet();
    }
    printf("aaa: %lu\n", pingMemory[0].tvB.tv_usec);
    ft_memcpy(buff, &pingMemory[0].icmp, sizeof(pingMemory[0].icmp));
    ft_memcpy(buff + sizeof(pingMemory[0].icmp), &pingMemory[0].tvB, sizeof(pingMemory[0].tvB));
    uint8_t j = sizeof(pingMemory[0].icmp) + sizeof(pingMemory[0].tvB);
    const uint8_t max = j + 40;
    char value = 0;
    for (; j < max; ++j)
        buff[j] = value++;
    pingMemory[0].icmp.checksum
        = checksum((uint16_t *)buff, sizeof(pingMemory[0].icmp) + sizeof(pingMemory[0].tvB) + 40);
    ft_memcpy(buff, &pingMemory[0].icmp, sizeof(pingMemory[0].icmp));
    //call another ping
    const char *inetResult = inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN);
    if (!inetResult) {
        exitInet();
    }
    printf("PING %s (%s): %u data bytes\n", listAddr->ai_canonname,
        inetResult, ECHO_REQUEST_SIZE);
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
    alarm(1);
    while (1) {
        icmpRequest();
        usleep(50);
    }
}