#include "ft_icmp.h"
#include "tools.h"

void    icmpRequest(void) {

}

void    icmpResponse(void) {

}

void    runIcmp(struct addrinfo *client, int fdSocket) {
    char icmpEcho[255];
    struct addrinfo *mem = client;
    int headerIcmp = 0;
    int result = -1;

    ft_memset(icmpEcho, 0, 256);
    while (mem) {
        printf("%d\n", mem->ai_flags);
        result = sendto(fdSocket, &headerIcmp,
            sizeof(headerIcmp), 0,
            (struct sockaddr *)mem, sizeof(*mem));
        if (result < 0) {
            dprintf(2, "%s\n", gai_strerror(result));
            exit(1);
        }else  printf("ok");
        sleep(1);
        printf("res: %lu\n", recvmsg(fdSocket, (struct msghdr *)icmpEcho, MSG_DONTWAIT));
        printf("%s\n", icmpEcho);
        mem = mem->ai_next;
    }
}