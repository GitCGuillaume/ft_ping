#include "ft_icmp.h"
#include "tools.h"

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
    bigBitMask(&ip->saddr, 0xFF000000, buff, 24, 0);
    bigBitMask(&ip->saddr, 0xFF0000, buff, 16, 1);
    bigBitMask(&ip->saddr, 0xFF00, buff, 8, 2);
    bigBitMask(&ip->saddr, 0xFF, buff, 0, 3);
    buff += 4;
    bigBitMask(&ip->daddr, 0xFF000000, buff, 24, 0);
    bigBitMask(&ip->daddr, 0xFF0000, buff, 16, 1);
    bigBitMask(&ip->daddr, 0xFF00, buff, 8, 2);
    bigBitMask(&ip->daddr, 0xFF, buff, 0, 3);
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
}

void    sigHandler(int sig) {
    printf("\na");
    if (sig != SIGALRM)
        return ;
    struct icmphdr  icmp;
    char buff[65507];
    int result = -1;
    static int i = 0;

    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(i);
    ++i;
    icmp.checksum = 0;
    ft_memcpy(buff, &icmp, sizeof(icmp));
    icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp));
    ft_memcpy(buff, &icmp, sizeof(icmp));   
    result = sendto(fdSocket, buff,
        64, 0,
        mem->ai_addr, sizeof(*mem->ai_addr));
    if (result < 0) {
        dprintf(2, "%s\n", gai_strerror(result));
        exit(1);
    }
}

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/

/* (ipv4 max)65535 - (sizeof ip)20 - (sizeof icmp)8 */
void    runIcmp(struct addrinfo *client) {
    struct msghdr msgResponse;
    char buff2[65507];
    
    struct iovec msg[1];

    signal(sigHandler, SIGALRM);
    while (1) {
        alarm(1);
        ft_memset(&msgResponse, 0, sizeof(struct msghdr));
        ft_memset(buff2, 0, 65507);
        msg[0].iov_base = buff2;
        msg[0].iov_len = sizeof(buff2);
        msgResponse.msg_name = 0;
        msgResponse.msg_namelen = 0;
        msgResponse.msg_iov = msg;
        msgResponse.msg_iovlen = 1;
        msgResponse.msg_control = 0;
        msgResponse.msg_controllen = 0;
        result = recvmsg(fdSocket, &msgResponse, MSG_WAITALL);
        if (result < 0) {
            dprintf(2, "%s\n", gai_strerror(result));
            exit(1);
        }
        icmpResponse(&msgResponse);
        usleep(1000000);
    }
}