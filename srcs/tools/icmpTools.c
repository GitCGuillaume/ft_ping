#include "tools.h"

void    exitInet(void) {
    if  (listAddr) {
        freeaddrinfo(listAddr);
    }
    if (fdSocket >= 0)
        close(fdSocket);
    exit(1);
}

/* max 16bits */
uint16_t    checksum(uint16_t *hdr, size_t len) {
    size_t sum = 0;
printf("LEN: %lu\n", len);
    while (len > 1) {
        printf("*hdr:%x\n", *hdr);
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

/* Get part from addr using mask bits */
void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}

/* Get part from addr using mask bits */
void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
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
    code /usr/include/netinet/ip_icmp.h
    NONE = no code
    https://datatracker.ietf.org/doc/html/rfc1122#page-38
    If an ICMP message of unknown type is received, it MUST be
         silently discarded.
*/
void getIcmpCode(struct icmphdr *icmp) {
    //list of icmp macro
    if (!icmp)
        return ;
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
    printf("ty: %u\n", icmp->code);
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