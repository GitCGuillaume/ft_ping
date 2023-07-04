#include "ft_icmp.h"
#include "tools.h"

void    icmpRequest(void) {

}

void    bitwiseMask(void *dst, void *src, size_t nb) {
    uint8_t mask = 0;

    mask = *(uint8_t *)src << nb;
    *(uint8_t *)dst = mask >> nb;
    return ;
}

void    icmpResponse(struct msghdr *msg, ssize_t recvMsgRes) {
    struct iovec *iov = msg->msg_iov;
    struct iphdr    ip;
    struct icmphdr  icmp;
    char *buff = iov->iov_base;
    /*printf("res: %ld\n", recvMsgRes);
    for (int i = 0; i < 84; ++i) {
        printf("%02x ", buff[i]);
    }
    ipHeader.ihl = buff[0];
    ipHeader.version = buff[1];
    printf("\nsizeof ipH: %lu\n", sizeof(ipHeader));
    printf("\nihl: %u\nver: %u\n", ipHeader.ihl, ipHeader.version);*/
    ft_memset(&ip, 0, sizeof(ip));
    ft_memset(&icmp, 0, sizeof(icmp));
    bitwiseMask(&ip, buff, sizeof(uint8_t));//tests ihl
    bitwiseMask(&ip, buff + sizeof(uint8_t), sizeof(uint8_t));//tests ihl
    //bitwiseMask(&ip + sizeof(uint16_t), buff + sizeof(uint16_t), sizeof(uint16_t));//tests ihl
    //ft_memcpy(&ip, buff, 1);
    //ft_memcpy(&icmp, buff + 20, sizeof(icmp));
    printf("ihl: %u ver: %u tos: %u tot_len: %u id: %u\nf_off: %u ttl: %u protocol: %u\n check: %u saddr: %u daddr: %u\n"
        , ip.ihl, ip.version, ip.tos, ip.tot_len, ip.id, ip.frag_off, ip.ttl, ip.protocol, ip.check, ip.saddr, ip.daddr);
    printf("type: %u code: %u\nchecksum: %u id: %u, seq: %u\n", icmp.type, icmp.code, icmp.checksum, icmp.un.echo.id, icmp.un.echo.sequence);
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
    struct msghdr msgResponse;
    char buff[65507];
    char buff2[65507];
    struct icmphdr  icmp;
    struct addrinfo *mem = client;
    struct iovec msg[1];
    ssize_t recvMsgRes = -1;
    //struct cmsghdr cmsg;
    ft_memset(buff, 0, 65507);
    ft_memset(buff2, 0, 65507);
    //ft_memset(&msg, 0, sizeof(struct iovec));
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    int result = -1;
    ft_memset(&icmp, 0, sizeof(struct icmphdr));
    icmp.type = ICMP_ECHO;
    icmp.code = 0;
    icmp.un.echo.id = getpid();
    int i = 1;
    while (1) {
        //while (mem) {
            icmp.un.echo.sequence = htons(i);
            ++i;
            icmp.checksum = 0;
            ft_memcpy(buff, &icmp, sizeof(icmp));
            icmp.checksum = checksum((uint16_t *)buff, sizeof(icmp));
            ft_memcpy(buff, &icmp, sizeof(icmp));
            result = sendto(fdSocket, buff,
                64, MSG_CONFIRM,
                mem->ai_addr, sizeof(*mem->ai_addr));
            printf("result: %d\n", result);
            if (result < 0) {
                dprintf(2, "%s\n", gai_strerror(result));
                exit(1);
            }
            printf("waiting...\n");
            
            ft_memset(&msgResponse, 0, sizeof(struct msghdr));
            msgResponse.msg_name = 0;
            msgResponse.msg_namelen = 0;
            msgResponse.msg_iov = msg;
            msgResponse.msg_iovlen = 1;
            msgResponse.msg_control = 0;
            msgResponse.msg_controllen = 0;
            printf("msgname: %p\nmsg_len: %u\nmsg_iov: %p\n iovlen:%lu\n msg_control: %p\n msg_controllen: %lu\n fl: %d\n",
                msgResponse.msg_name,
                msgResponse.msg_namelen,
                msgResponse.msg_iov, msgResponse.msg_iovlen,
                msgResponse.msg_control, msgResponse.msg_controllen,
                msgResponse.msg_flags);
            printf("res: %lu\n", (recvMsgRes = recvmsg(fdSocket, &msgResponse, MSG_WAITALL)));
            
            icmpResponse(&msgResponse, recvMsgRes);
            printf("ok\n");
            printf("msgname: %p\nmsg_len: %u\nmsg_iov: %p\n iovlen:%lu\n msg_control: %p\n msg_controllen: %lu\n fl: %d\n",
                msgResponse.msg_name,
                msgResponse.msg_namelen,
                msgResponse.msg_iov, msgResponse.msg_iovlen,
                msgResponse.msg_control, msgResponse.msg_controllen,
                msgResponse.msg_flags);
            exit(1);
            //exit(1);
            //mem = mem->ai_next;
            //break;
       // }
        usleep(1000000);
    }
}