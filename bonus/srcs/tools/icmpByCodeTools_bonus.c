#include "tools_bonus.h"

/*
    https://datatracker.ietf.org/doc/html/rfc2474#section-3
*/
void    headerDumpIp(struct iphdr *ip) {
    if (!ip)
        return ;
    /* Display header*/
    uint16_t    oddsLen
        = (ntohs(ip->tot_len) < ip->tot_len ? ntohs(ip->tot_len) : ip->tot_len);
    char src[INET_ADDRSTRLEN];
    char dst[INET_ADDRSTRLEN];
    ft_memset(src, 0, sizeof(src));
    ft_memset(dst, 0, sizeof(dst));

    const char * saddr = inet_ntop(AF_INET, &ip->saddr, src, INET_ADDRSTRLEN);
    const char * daddr = inet_ntop(AF_INET, &ip->daddr, dst, INET_ADDRSTRLEN);
    printf("\nIP Hdr Dump:\n %u%u%hhx%hhx %04hx",
        ip->version, ip->ihl, (uint8_t)(ip->tos & 0xF0) >> 4,
        (uint8_t)(ip->tos & 0xF), ip->tot_len);
    printf(" %04hx %04hx %02hhu%02hhu", ip->id, ip->frag_off, ip->ttl, ip->protocol);
    printf(" %04hx %04hx %04hx %04hx %04hx\n", ip->check,
        convertEndianess(ip->saddr & 0x0000FFFF), convertEndianess(ip->saddr >> 16),
        convertEndianess(ip->daddr & 0x0000FFFF), convertEndianess(ip->daddr >> 16));
    /* Display header verbose */
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
    printf("%2u %2u  %hhx%hhx %04hx",
        ip->version, ip->ihl, (uint8_t)(ip->tos & 0xF0) >> 4,
        (uint8_t)(ip->tos & 0xF),
        oddsLen);
    uint8_t flag = ip->frag_off >> 13;
    printf(" %04hx %3hhu %04hx  %02hhu",
        ip->id, flag, (uint16_t)(ip->frag_off & 0x1FFF)
        , ip->ttl);
    printf("  %02hhu %04hx", ip->protocol, ip->check);
    printf(" %s  %s\n", saddr, daddr);
}

void    headerDumpData(struct icmphdr *icmp, uint16_t size) {
    if (!icmp)
        return ;
    printf("ICMP: type %hhu, code %hhu, size %hu, id 0x%04hx, seq 0x%04hx",
        icmp->type, icmp->code, size, icmp->un.echo.id, icmp->un.echo.sequence);
}

/*
    convertEndianess == need to convert from
    big endian to little endian (ping is from sendto)
*/
unsigned char isReplyOk(struct iphdr *ipOriginal, struct icmphdr *icmpOriginal,
    struct sockaddr_in *translate, ssize_t recv) {
    struct icmphdr  *icmp;
    struct s_ping_memory *ping;

    if (!ipOriginal)
        return (FALSE);
    //remove icmp size, now we are in the original payload/ip + datagrame
    recv -= sizeof(struct icmphdr);
    recv -= sizeof(struct iphdr);
    if (recv < 8)
        return (FALSE);
    if (ipOriginal->protocol != ICMP)
        return (FALSE);
    if (translate->sin_addr.s_addr != ipOriginal->daddr) {
        return (FALSE);
    }
    recv -= sizeof(struct icmphdr);
    if (recv < 0)
        return (FALSE);
    ping = &pingMemory[icmpOriginal->un.echo.sequence];
    if (!ping)
        return (FALSE);
    icmp = &ping->icmp;
    return (icmp->type == icmpOriginal->type && icmp->code == icmpOriginal->code
        && convertEndianess(icmp->checksum) == icmpOriginal->checksum
        && convertEndianess(icmp->un.echo.id) == icmpOriginal->un.echo.id
        && convertEndianess(icmp->un.echo.sequence) == icmpOriginal->un.echo.sequence);
}