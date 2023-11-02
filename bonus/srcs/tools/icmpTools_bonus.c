#include "tools_bonus.h"

/* Get part from addr using mask bits */
void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump) {
    if (addr && buff)
        *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}

/* Get part from addr using mask bits */
void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump) {
    if (addr && buff)
        *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
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
    if (icmp->type == 0 || icmp->type == 8) {
        bitMask(&icmp->un.echo.id, 0xFF00, buff, 8, 0);
        bitMask(&icmp->un.echo.id, 0xFF, buff, 0, 1);
        buff += 2;
        bitMask(&icmp->un.echo.sequence, 0xFF00, buff, 8, 0);
        bitMask(&icmp->un.echo.sequence, 0xFF, buff, 0, 1);
    } else if (icmp->type == 5) {
        bigBitMask(&icmp->un.gateway, 0xFF, buff, 0, 0);
        bigBitMask(&icmp->un.gateway, 0xFF00, buff, 8, 1);
        bigBitMask(&icmp->un.gateway, 0xFF0000, buff, 16, 2);
        bigBitMask(&icmp->un.gateway, 0xFF000000, buff, 24, 3);
    }
}