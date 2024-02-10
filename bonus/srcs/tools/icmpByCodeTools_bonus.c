#include "tools_bonus.h"

/*
    https://datatracker.ietf.org/doc/html/rfc2474#section-3
*/
void    headerDumpIp(struct iphdr *ip, int special) {
    if (!ip)
        return ;
    /* Display header*/
    uint16_t    noNetwork
        = (ip->tot_len < ntohs(ip->tot_len) ? ip->tot_len : ntohs(ip->tot_len));
    char src[INET_ADDRSTRLEN];
    char dst[INET_ADDRSTRLEN];
    ft_memset(src, 0, sizeof(src));
    ft_memset(dst, 0, sizeof(dst));

    const char * saddr = inet_ntop(AF_INET, &ip->saddr, src, INET_ADDRSTRLEN);
    const char * daddr = inet_ntop(AF_INET, &ip->daddr, dst, INET_ADDRSTRLEN);
    printf("\n");
    if (special) {
        printf("IP Hdr Dump:\n %x%x%02hhx %04hx",
        ip->version, ip->ihl, ip->tos, ntohs(ip->tot_len));
        printf(" %04hx %04hx %02hhx%02hhx", ntohs(ip->id), ntohs(ip->frag_off), ip->ttl, ip->protocol);
        printf(" %04hx %04hx %04hx %04hx %04hx\n", ntohs(ip->check),
            ntohs(ip->saddr & 0x0000FFFF), ntohs(ip->saddr >> 16),
            ntohs(ip->daddr & 0x0000FFFF), ntohs(ip->daddr >> 16));
    }
    /* Display header verbose */
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
    printf("%2x %2x  %02hhx %04hx",
        ip->version, ip->ihl, ip->tos,
        noNetwork);
    uint8_t flag = ntohs(ip->frag_off) >> 13;
    printf(" %04hx %3hhx %04hx  %02hhx",
        ntohs(ip->id), flag, (uint16_t)(ntohs(ip->frag_off) & 0x1FFF)
        , ip->ttl);
    printf("  %02hhx %04hx", ip->protocol, ntohs(ip->check));
    printf(" %s  %s\n", saddr, daddr);
}

void    headerDumpData(struct icmphdr *icmp, uint16_t size) {
    if (!icmp)
        return ;
    printf("ICMP: type %hhu, code %hhu, size %hu, id 0x%04hx, seq 0x%04hx",
        icmp->type, icmp->code, size, ntohs(icmp->un.echo.id), ntohs(icmp->un.echo.sequence));
}

unsigned char isReplyOk(struct iphdr *ipOriginal, struct icmphdr *icmpOriginal,
    ssize_t recv) {
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
    recv -= sizeof(struct icmphdr);
    ping = &pingMemory[ntohs(icmpOriginal->un.echo.sequence)];
    if (!ping)
        return (FALSE);
    icmp = &ping->icmp;
    return (icmp->type == icmpOriginal->type && icmp->code == icmpOriginal->code
        && ntohs(icmp->checksum) == ntohs(icmpOriginal->checksum)
        && ntohs(icmp->un.echo.id) == ntohs(icmpOriginal->un.echo.id)
        && ntohs(icmp->un.echo.sequence) == ntohs(icmpOriginal->un.echo.sequence));
}