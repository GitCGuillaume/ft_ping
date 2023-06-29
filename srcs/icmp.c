#include "ft_icmp.h"
#include "tools.h"

void    icmpRequest(void) {

}

void    icmpResponse(void) {

}

/* max 16bits */
uint16_t    checksum(uint16_t *hdr, size_t len) {
    uint16_t sum = 0;

    while (len > 0) {
        sum += *hdr++;
        len -= sizeof(uint16_t);
    }
    return (~sum);
}

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/


void    runIcmp(struct addrinfo *client, int fdSocket) {
    struct msghdr msgResponse;
    struct icmphdr  icmp;
    struct addrinfo *mem = client;

    int result = -1;
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.checksum = checksum((uint16_t *)icmp, sizeof(icmp));
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(1);
    while (mem) {
//        printf("%d\n", mem->ai_flags);
        result = sendto(fdSocket, &icmp,
            sizeof(struct icmphdr), 0,
            (struct sockaddr *)mem, sizeof(struct sockaddr));
        if (result < 0) {
            dprintf(2, "%s\n", gai_strerror(result));
            exit(1);
        }
        while (1) {
            printf("res: %lu\n", recvmsg(fdSocket, &msgResponse, MSG_DONTWAIT));
            usleep(100);
        }
        break ;
        mem = mem->ai_next;
    }
}