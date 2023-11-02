#ifndef ICMP_CODE_TOOLS_BONUS_H
# define ICMP_CODE_TOOLS_BONUS_H

void    headerDumpIp(struct iphdr *ip);
void    headerDumpData(struct icmphdr *icmp, uint16_t size);
unsigned char isReplyOk(struct iphdr *ipOriginal, struct icmphdr *icmpOriginal,
    struct sockaddr_in *translate, ssize_t recv);

#endif