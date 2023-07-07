#include "ft_icmp.h"
#include "tools.h"

int i = 0;

void    icmpRequest(void) {

}

void printBits(unsigned int num)
{
   for(unsigned int bit=0;bit< 256; bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}

/* max 16bits */
uint16_t    checksum(uint16_t *hdr, size_t len) {
    uint16_t sum = 0;

    while (len > 1) {
        sum += *hdr++;
        len -= sizeof(uint16_t);
    }
    if (len > 0) {
        sum += *hdr;
    }
    return (~sum);//complement one' from sum
}

void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}

void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}

void    parseIp(struct iphdr *ip, char *buff) {
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

void    parseIcmp(struct icmphdr  *icmp, char *buff) {
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

void    displayRequest(struct iphdr *ip, struct icmphdr *icmp) {
    char str[1000];
    
    ft_memset(str, 0, 1001);
    (void)icmp;
    //if (icmp->type == 8 || !ip || !icmp)
      //  return ; //a voir
    const char *ntop = inet_ntop(AF_INET, &ip->saddr, str, INET_ADDRSTRLEN);
    
    printf("%lu bytes from %s: icmp_seq=%u ttl=%u time=0,579 ms\n",
        ip->tot_len - sizeof(*ip), ntop, 0, ip->ttl);
}

void    icmpResponse(struct msghdr *msg) {
    struct iovec *iov = msg->msg_iov;
    struct iphdr    ip;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;

    ft_memset(&ip, 0, sizeof(ip));
    ft_memset(&icmp, 0, sizeof(icmp));
    parseIp(&ip, buff);
    buff += 20;
    parseIcmp(&icmp, buff);
    printf("\n");
    printf("ihl: %u ver: %u tos: %u tot_len: %u id: %u\nf_off: %u ttl: %u protocol: %u\n check: %u saddr: %u daddr: %u\n"
        , ip.ihl, ip.version, ip.tos, ip.tot_len, ip.id, ip.frag_off, ip.ttl, ip.protocol, ip.check, ip.saddr, ip.daddr);
    printf("type: %u code: %u\nchecksum: %u id: %u, seq: %u\ngateway: %u\n", icmp.type, icmp.code, icmp.checksum, icmp.un.echo.id, icmp.un.echo.sequence, icmp.un.gateway);
    displayRequest(&ip, &icmp);
}

void    sigHandler(int sig) {
    printf("\na");
    if (sig != SIGALRM)
        return ;
    
    

    
}

void    sigHandlerInt(int sigNum) {
    if (sigNum != SIGINT)
        return ;
}

void    sigHandlerAlrm(int sigNum) {
    if (sigNum != SIGALRM)
        return ;
    struct icmphdr  icmp;
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvB;
    struct timeval tvA;
    char buff2[65507];
    char buff[65507];
    int result = -1;
    
    //while (1) {
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(i);
    ++i;
    printf("i:%d\n", i);
    icmp.checksum = 0;
    ft_memcpy(buff, &icmp, sizeof(icmp));
    icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp));
    ft_memcpy(buff, &icmp, sizeof(icmp));
    alarm(1);
    gettimeofday(&tvB, 0);
    result = sendto(fdSocket, buff,
        64, 0,
        listAddr->ai_addr, sizeof(*listAddr->ai_addr));
    if (result < 0) {
        dprintf(2, "%s\n", gai_strerror(result));
        exit(1);
    }
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff2, 0, 65507);
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    result = recvmsg(fdSocket, &msgResponse, 0);
    if (result < 0) {
        dprintf(2, "%s\n", gai_strerror(result));
        exit(1);
    }
    gettimeofday(&tvA, 0);
    printf("tvB sec: %ld tvA sec: %ld\n", tvB.tv_sec, tvA.tv_sec);
    printf("usec: %ld\n", tvA.tv_usec - tvB.tv_usec);
    icmpResponse(&msgResponse);
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
    struct icmphdr  icmp;
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvB;
    struct timeval tvA;
    char buff2[65507];
    char buff[65507];
    char str[1000];
    ft_memset(str, 0, 1001);
    int result = -1;

    signal(SIGALRM, sigHandlerAlrm);
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(i);
    ++i;
    printf("i:%d\n", i);
    icmp.checksum = 0;
    ft_memcpy(buff, &icmp, sizeof(icmp));
    icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp));
    ft_memcpy(buff, &icmp, sizeof(icmp));
    alarm(1);
    gettimeofday(&tvB, 0);
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    printf("PING %s (%s): %u data bytes\n", listAddr->ai_canonname,
        inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN), 0);
    result = sendto(fdSocket, buff,
        64, 0,
        listAddr->ai_addr, sizeof(*listAddr->ai_addr));
    if (result < 0) {
        dprintf(2, "%s\n", gai_strerror(result));
        exit(1);
    }
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff2, 0, 65507);
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    result = recvmsg(fdSocket, &msgResponse, MSG_WAITALL);
    if (result < 0) {
        dprintf(2, "%s\n", gai_strerror(result));
        exit(1);
    }
    gettimeofday(&tvA, 0);
    printf("tvB sec: %ld tvA sec: %ld\n", tvB.tv_sec, tvA.tv_sec);
    printf("usec: %ld\n", tvA.tv_usec - tvB.tv_usec);
    icmpResponse(&msgResponse);
    while (1);
}