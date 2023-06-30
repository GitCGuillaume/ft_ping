#include "ft_icmp.h"
#include "tools.h"

void    icmpRequest(void) {

}

void    icmpResponse(void) {

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

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/

/* (ipv4 max)65535 - (sizeof ip)20 - (sizeof icmp)8 */
void    runIcmp(struct addrinfo *client, int fdSocket) {
   // struct msghdr msgResponse;
    char buff[65507];
    struct icmphdr  icmp;
    struct addrinfo *mem = client;
    ft_memset(buff, 0, 65507);
    int result = -1;
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    icmp.un.echo.sequence = htons(1);
    icmp.checksum = 0;
    //checksum((uint16_t *)&icmp, 32);
    //exit(1);
    ft_memcpy(buff, &icmp, sizeof(icmp));
    icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp) + 1);
    ft_memcpy(buff, &icmp, sizeof(icmp));
    printf("struct ip: %ld\n", sizeof(struct ip));
    printf("%ld\n", sizeof(icmp));
    while (mem) {
//        printf("%d\n", mem->ai_flags);
        result = sendto(fdSocket, buff,
            sizeof(struct icmphdr) + 1, MSG_CONFIRM,
            mem->ai_addr, sizeof(*mem->ai_addr));
        printf("result: %d\n", result);
        if (result < 0) {
            dprintf(2, "%s\n", gai_strerror(result));
            exit(1);
        }
        /*while (1) {
            printf("res: %lu\n", recvmsg(fdSocket, &msgResponse, MSG_DONTWAIT));
            usleep(100);
        }*/
        //break ;
        mem = mem->ai_next;
    }
}