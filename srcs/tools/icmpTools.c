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

    while (len > 1) {
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

void    bitMask(uint16_t *addr, uint16_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}

void    bigBitMask(uint32_t *addr, uint32_t mask, char *buff, int nb, int jump) {
    *addr = (*addr & ~mask) | ((*(buff + jump) << nb) & mask);
}