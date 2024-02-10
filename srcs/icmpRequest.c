#include "ft_icmp.h"
#include "tools.h"
#include "flags.h"

static sig_atomic_t errSignal = FALSE;

/* Get request response */
static int    icmpGetResponse(void) {
    char buff[ECHO_REPLY_SIZE] = {0};
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvA;
    int result = -1;

    //init response
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    msg[0].iov_base = buff;
    msg[0].iov_len = sizeof(buff);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    int cpyErrno = errno;
    result = recvmsg(fdSocket, &msgResponse, 0);
    if (result < 0
        && errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
        alarm(0);
        dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
        exitInet();
    }
    if (result == -1) {
        return (TRUE);
    }
    errno = cpyErrno;
    if (!end) {
        if (gettimeofday(&tvA, 0) < 0)
            exitInet();
        //result - sizeof(struct iphdr) = whole response minus ip header (84-20)=64 
        //echo reply is 64 like a standard reply from the ping inetutils2.0
        icmpInitResponse(&msgResponse, result - sizeof(struct iphdr), &tvA);
    }
    return (FALSE);
}

static void    initPing(struct s_ping_memory *ping, int cpyI) {
    if (!ping)
        exitInet();

    ping->icmp.type = ICMP_ECHO;
    ping->icmp.code = 0;
    ping->icmp.un.echo.id = htons(getpid());
    ping->icmp.un.echo.sequence = htons(cpyI);
}

static void    fillBuffer(char *buff, struct s_ping_memory *ping,
    const struct timeval *tvB) {
    uint8_t j = sizeof(ping->icmp) + sizeof(*tvB);
    const uint8_t max = j + 40;
    char value = 0;

    ft_memcpy(buff, &ping->icmp, sizeof(ping->icmp));
    ft_memcpy(buff + sizeof(ping->icmp), tvB, sizeof(*tvB));
    for (; j < max; ++j)
        buff[j] = value++;
    ping->icmp.checksum
        = checksum((uint16_t *)buff, sizeof(ping->icmp) + sizeof(*tvB) + 40);
    ft_memcpy(buff, &ping->icmp, sizeof(ping->icmp));
}

static void    displayPingHeader() {
    struct sockaddr_in *translate = (struct sockaddr_in *)listAddr->ai_addr;
    char str[INET_ADDRSTRLEN];
    ft_memset(&str, 0, INET_ADDRSTRLEN);
    const char *inetResult = inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN);

    if (!inetResult) {
        exitInet();
    }
    printf("PING %s (%s): %lu data bytes", listAddr->ai_canonname,
        inetResult, ECHO_REQUEST_SIZE - sizeof(struct icmphdr));
    //jump line or display -v id option part
    if (t_flags.v == FALSE)
        printf("\n");
    else
        printf(", id 0x%04x = %u\n",
            ntohs(pingMemory[0].icmp.un.echo.id),
            ntohs(pingMemory[0].icmp.un.echo.id));
}

static void    sendPacket(int num) {
    if (num != SIGALRM)
        return ;
    alarm(1);
    struct timeval tvB;
    char buff[ECHO_REQUEST_SIZE];
    int result = -1;
    int cpyI;

    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    cpyI = roundTripGlobal.packetSend % 65536;
    if (cpyI == 0)
        ft_memset(pingMemory, 0, sizeof(pingMemory));
    initPing(&pingMemory[cpyI], cpyI);
    //get timestamp for ping payload
    if (gettimeofday(&tvB, 0) < 0) {
        exitInet();
    }
    fillBuffer(buff, &pingMemory[cpyI], &tvB);
    //inc nb packets and send
    roundTripGlobal.packetSend++;
    result = sendto(fdSocket, buff,
        ECHO_REQUEST_SIZE, 0,
        listAddr->ai_addr, listAddr->ai_addrlen);
    if (result < 0) {
        alarm(0);
        errSignal = TRUE;
        sigHandlerInt(SIGINT);
    }
}

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/
/* (ipv4 max)65535 - (sizeof ip)20 - (sizeof icmp)8 */
void    runIcmp(void) {
    if (!listAddr)
        exitInet();
    char buff[ECHO_REQUEST_SIZE];
    int cpyI;

    if (signal(SIGALRM, sendPacket) == SIG_ERR)
        exitInet();
    //init vars part
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    cpyI = roundTripGlobal.packetSend % 65536;
    if (cpyI == 0)
        ft_memset(pingMemory, 0, sizeof(pingMemory));
    initPing(&pingMemory[cpyI], cpyI);
    //display ping header
    displayPingHeader();
    //end display ping header
    sendPacket(SIGALRM);
    while (!end) {
        icmpGetResponse();
    }
    if (errSignal) {
        dprintf(2, "ping: sending packet: %s\n", strerror(errno));
        exitInet();
    }
}
