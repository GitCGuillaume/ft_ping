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
   for(uint16_t bit=0;bit< sizeof(uint16_t *); bit++)
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
    bigBitMask(&ip->daddr, 0xFF, buff, 24, 0);
    bigBitMask(&ip->daddr, 0xFF00, buff, 16, 1);
    bigBitMask(&ip->daddr, 0xFF0000, buff, 8, 2);
    bigBitMask(&ip->daddr, 0xFF000000, buff, 0, 3);
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
    printf("res icmp c: %u\n", icmp->checksum);
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
    struct timeval *tvB, struct timeval *tvA) {
    if (!ip || !icmp)
        exitInet();
    //char str[16];
    time_t seconds = tvA->tv_sec - tvB->tv_sec;
    suseconds_t microSeconds = tvA->tv_usec - tvB->tv_usec;
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
void    icmpResponse(struct msghdr *msg, struct timeval *tvB,
    struct timeval *tvA, ssize_t recv) {
    if (!msg || !tvB || !tvA)
        exitInet();
    struct iovec *iov = msg->msg_iov;
    struct iphdr    ip;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;
    char buffChecksum[BUFF2_SIZE];
    uint16_t resultChecksum;
    char str[16];

    ft_memset(str, 0, 16);
    ft_memset(&ip, 0, sizeof(ip));
    ft_memset(&icmp, 0, sizeof(icmp));
    ft_memset(buffChecksum, 0, BUFF2_SIZE);
    /* Get IPv4 from buffer  */
    parseIp(&ip, buff);
    buff += 20;
    /* Get Icmp from buffer */
    parseIcmp(&icmp, buff);
    buff += 8;
    printf("CHE: %x\n", icmp.checksum);

    ft_memcpy(buffChecksum, &icmp, sizeof(icmp));
    ft_memcpy(buffChecksum + sizeof(icmp), buff, sizeof(*tvB));
    //for(int i = 0; i < ECHO_REQUEST_B_SENT; ++i)
    //    printf("i: %x\n", buff[i]);
    resultChecksum = checksum((uint16_t *)buffChecksum, sizeof(icmp));
    //ft_memcpy(buffChecksum, &icmp, sizeof(icmp));
    //ft_memcpy(buffChecksum + sizeof(icmp), tvB, sizeof(*tvB));

    //ft_memcpy(buffChecksum, &icmp, sizeof(icmp));
    //ft_memcpy(buffChecksum + 8, tvB, sizeof(*tvB));
    //check source in commentary
    //printf("dsqdsqqsd:%u dd:%u %u\n", icmp.type, icmp.code, icmp.checksum);
    //resultChecksum = checksum((uint16_t *)buffChecksum, sizeof(icmp) + sizeof(*tvB));
    const char *ntop = inet_ntop(AF_INET, &ip.saddr, str, INET_ADDRSTRLEN);
    if (!ntop)
        exitInet();
    printf("%lu bytes from %s: ",
        recv, ntop);
    printf("resultC: %x\n", resultChecksum);
    if (resultChecksum != 0) {
        printf("resultChecksum: %hu\ncode: icmp.code: %hu type: %hu\n", resultChecksum, icmp.code, icmp.type);
        printf("id: %u seq: %u\n", icmp.un.echo.id, icmp.un.echo.sequence);
        getIcmpCode(&icmp);
    } else {
        displayResponse(&ip, &icmp, tvB, tvA);
    }
}

/* Ger request response */
void    icmpRequest(struct timeval *tvB, struct msghdr *msgResponse) {
    struct timeval tvA;
    int result = -1;

    result = recvmsg(fdSocket, msgResponse, 0);
    printf("RES: %u\n", result);
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    if (gettimeofday(&tvA, 0) < 0)
        exitInet();
    icmpResponse(msgResponse, tvB, &tvA, result);
}

/* send ping using signal alarm */
void    sigHandlerAlrm(int sigNum) {
    if (sigNum != SIGALRM)
        return ;
    if (!listAddr)
        exitInet();
    struct icmphdr  icmp;
    struct timeval tvB;
    struct msghdr msgResponse;
    struct iovec msg[1];
    char buff2[BUFF2_SIZE];
    char buff[ECHO_REQUEST_B_SENT];
    int result = -1;
    
    //init part
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff, 0, ECHO_REQUEST_B_SENT);
    ft_memset(buff2, 0, BUFF2_SIZE);
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(i);
    ++i;
    icmp.checksum = 0;
    ft_memcpy(buff, &icmp, sizeof(icmp));
    icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp));
    ft_memcpy(buff, &icmp, sizeof(icmp));
    //init vars part
    alarm(1);
    //get time before substract
    if (gettimeofday(&tvB, 0) < 0) {
        exitInet();
    }
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    result = sendto(fdSocket, buff,
        ECHO_REQUEST_B_SENT, 0,
        listAddr->ai_addr, sizeof(*listAddr->ai_addr));
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    icmpRequest(&tvB, &msgResponse);
}

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/

/* (ipv4 max)65535 - (sizeof ip)20 - (sizeof icmp)8 */
void    runIcmp(/*struct addrinfo *client*/) {
    if (!listAddr)
        exitInet();
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    struct icmphdr  icmp;
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvB;
    
    char buff[ECHO_REQUEST_B_SENT];
    char buff2[BUFF2_SIZE];
    char str[16];
    int result = -1;

    if (signal(SIGALRM, sigHandlerAlrm) == SIG_ERR)
        exitInet();
    //init vars part
    ft_memset(str, 0, 16);
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff, 0, ECHO_REQUEST_B_SENT);
    ft_memset(buff2, 0, BUFF2_SIZE);
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(i);
    ++i;
    icmp.checksum = 0;
    //get time before substract
    if (gettimeofday(&tvB, 0) < 0) {
        exitInet();
    }
    ft_memcpy(buff, &icmp, sizeof(icmp));
    ft_memcpy(buff + sizeof(icmp), &tvB, sizeof(tvB));
    //for(int i = 0; i < ECHO_REQUEST_B_SENT; ++i)
    //    printf("i: %x\n", buff[i]);
    icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp)+sizeof(tvB));
    ft_memcpy(buff, &icmp, sizeof(icmp));
    //ft_memcpy(buff + sizeof(icmp), &tvB, sizeof(tvB));
    //call another ping
    alarm(1);
    const char *inetResult = inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN);
    if (!inetResult) {
        exitInet();
    }
    printf("sdqs: %lu\n", sizeof(struct icmphdr));
    printf("un: %lu\n", sizeof(icmp.un));
    printf("sizeof: %lu\n", sizeof(struct iphdr));
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    printf("PING %s (%s): %u data bytes\n", listAddr->ai_canonname,
        inetResult, ECHO_REQUEST_B_SENT);
    result = sendto(fdSocket, buff,
        ECHO_REQUEST_B_SENT, 0,
        listAddr->ai_addr, sizeof(*listAddr->ai_addr));
    if (result < 0) {
        freeaddrinfo(listAddr);
        dprintf(2, "%s\n", gai_strerror(result));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    icmpRequest(&tvB, &msgResponse);
    while (1);
}