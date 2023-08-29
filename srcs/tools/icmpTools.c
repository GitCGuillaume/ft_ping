#include "tools.h"

void    exitInet(void) {
    if  (listAddr) {
        freeaddrinfo(listAddr);
    }
    if (fdSocket >= 0)
        close(fdSocket);
    exit(1);
}

/* Get part from addr using mask bits */
void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}

/* Get part from addr using mask bits */
void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump) {
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

/* max 16bits */
uint16_t    checksum(uint16_t *hdr, size_t len) {
    size_t sum = 0;

    while (len > 1) {
        //printf("*hdr:%x\n", *hdr);
        sum += *hdr++;
        len -= sizeof(uint16_t);
    }
    /* if len is odd */
    if (len > 0) {
        sum += *hdr;
    }
    //if sum superior than 16 bits,
    //get 16's least significants bits + 16's most significant bits
    if (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (~sum); //complement one' from sum
}

/*  Convert big endian to little endian
    0 0 1 1 1 1 1 1 >> 8 && << 8 1 0 1 1 0 0 0 1
    0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1
    OR
    1 0 1 1 0 0 0 1 0 0 0 0 0 0 0 0
    =
    0 1 0 0 1 0 0 1 0 0 1 0 0 0 0 0
*/

uint16_t    convertEndianess(uint16_t echoVal) {
    return (echoVal >> 8 | echoVal << 8);
}

void    destUnreach(uint8_t code) {
    const char *arr[16] = {
        "Destination Net Unreachable", "Destination Host Unreachable", "Destination Protocol Unreachable",
        "Destination Port Unreachable", "Fragmentation needed and DF set", "Source Route Failed",
        "Network Unknown", "Host Unknown", "Host Isolated",
        "Communication With Network Is Adminitratively Prohibited",
        "Communication With Host Is Adminitratively Prohibited",
        "Destination Network Unreachable At This TOS",
        "Destination Host Unreachable At This TOS",
        "Packet Filtered", "Precedence Violation", "Precedence Cutoff"
    };
    int i;

    for (i = 0; i < 16; ++i) {
        if (i == code)
            break ;
    }
    if (i != 16)
        printf("%s\n", arr[i]);
}

/* Deprecated */
void    sourceQuench(uint8_t code) {
    printf("%s\n", "Source Quench");
}

void    redirect(uint8_t code) {
    if (code == 0)
        printf("%s\n", "Redirect Network");
    else if (code == 1)
        printf("%s\n", "Redirect Host");
    else if (code == 2)
        printf("%s\n", "Type of Service and Network");
    else if (code == 3)
        printf("%s\n", "Type of Service and Host");
}

void    timeExceed(uint8_t code) {
    const char  *arr[2] = {
        "Time to live exceeded",
        "Frag reassembly time exceeded"
    };
    int i;

    for (i = 0; i < 2; ++i) {
        if (i == code)
            break ;
    }
    if (i != 2)
        printf("%s\n", arr[i]);
}

void    paramProb(uint8_t code) {
    if (code == 0)
        printf("%s\n", "Pointer indicate the error");
    else if (code == 1)
        printf("%s\n", "Required option is missing");
    else if (code == 2)
        printf("%s\n", "Bad length");
}

void    timeStamp(uint8_t code) {

}

void    timeStampReply(uint8_t code) {

}

void    infoRequest(uint8_t code) {

}

void    infoReply(uint8_t code) {

}

void    address(uint8_t code) {

}

void    addressReply(uint8_t code) {
    
}


/*
    convertEndianess == need to convert from
    big endian to little endian (ping is from sendto)
*/
unsigned char isReplyOk(struct iphdr *ip,
    struct sockaddr_in *translate, char *buff, ssize_t recv) {
    struct iphdr    ipOriginal;
    struct icmphdr  icmpOriginal;
    struct icmphdr  *icmp;
    struct s_ping_memory *ping;

    //remove icmp size, now we are in the original payload/ip + datagrame
    recv -= sizeof(struct icmphdr);
    recv -= sizeof(struct iphdr);
    if (recv < 8)
        return (FALSE);
    printf("recv: %ld\n", recv);
    parseIp(&ipOriginal, buff);
    buff += sizeof(struct iphdr);
    if (ipOriginal.protocol != ICMP)
        return (FALSE);
    if (translate->sin_addr.s_addr != ipOriginal.daddr) {
        return (FALSE);
    }
    recv -= sizeof(struct icmphdr);
    if (recv < 0)
        return (FALSE);
    parseIcmp(&icmpOriginal, buff);
    ping = &pingMemory[icmpOriginal.un.echo.sequence];
    if (!ping)
        return (FALSE);
    
    icmp = &ping->icmp;
    /*printf("type: %u %u\ncode: %u %u\nchk: %u %u\nid: %u %u\nseq: %u %u",
        icmp->type, icmpOriginal.type,
        icmp->code, icmpOriginal.code,
        convertEndianess(icmp->checksum), icmpOriginal.checksum,
        convertEndianess(icmp->un.echo.id), icmpOriginal.un.echo.id,
        convertEndianess(icmp->un.echo.sequence), icmpOriginal.un.echo.sequence);
    */
    return (icmp->type == icmpOriginal.type && icmp->code == icmpOriginal.code
        && convertEndianess(icmp->checksum) == icmpOriginal.checksum
        && convertEndianess(icmp->un.echo.id) == icmpOriginal.un.echo.id
        && convertEndianess(icmp->un.echo.sequence) == icmpOriginal.un.echo.sequence);
}

/*
    code /usr/include/netinet/ip_icmp.h
    NONE = no code
    https://datatracker.ietf.org/doc/html/rfc1122#page-38
    If an ICMP message of unknown type is received, it MUST be
         silently discarded.
*/
void getIcmpCode(struct iphdr *ip, struct icmphdr *icmp,
    struct sockaddr_in *translate, char *buff, ssize_t recv) {
    //list of icmp macro
    if (!icmp)
        return ;
    char str[16];
    char str2[16];

    if (icmp->type != 12)
        buff += sizeof(struct icmphdr);
    if (isReplyOk(ip, translate, buff, recv) == FALSE)
        return ;
    ft_memset(str, 0, 16);
    ft_memset(str2, 0, 16);
    //struct iphdr  *originalIp = (struct iphdr *)buff;
    //printf("zzzz%s %s\n", inet_ntop(AF_INET, &originalIp->saddr, str, INET_ADDRSTRLEN),
    //    inet_ntop(AF_INET, &originalIp->daddr, str2, INET_ADDRSTRLEN));
    unsigned int types[19] = {
        NONE, NONE, NONE,
        ICMP_DEST_UNREACH, ICMP_SOURCE_QUENCH,
        ICMP_REDIRECT, NONE,NONE, NONE,
        NONE, NONE, ICMP_TIME_EXCEEDED,
        ICMP_PARAMETERPROB, ICMP_TIMESTAMP, ICMP_TIMESTAMPREPLY,
        ICMP_INFO_REQUEST, ICMP_INFO_REPLY, ICMP_ADDRESS,
        ICMP_ADDRESSREPLY
    };
    //list of function to call from macro number
    void    *functionArray[19] = {
        (void*)0, (void*)0, (void*)0,
        &destUnreach, &sourceQuench, &redirect,
        (void*)0, (void*)0, (void*)0, (void*)0, (void*)0,
        &timeExceed, &paramProb, &timeStamp, &timeStampReply,
        &infoRequest, &infoReply, &address, &addressReply
    };
    void    (*functionCall)(uint8_t) = NULL;
    unsigned int i;

    printf("ty: %u\n", icmp->type);
    printf("code: %u\n", icmp->code);
    for (i = 0; i < 20; ++i) {
        if (icmp->type == types[i]
            && types[i] != NONE) {
            functionCall = functionArray[i];
            break ;
        }
    }
    if (functionCall)
           functionCall(icmp->code);
}