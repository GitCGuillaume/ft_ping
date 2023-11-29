#include "ft_icmp.h"
#include "tools.h"

 static int    substractDelta(struct timeval *elapsedEndTime, struct timeval *elapsedStartTime) {
    if (gettimeofday(elapsedEndTime, 0) < 0) {
        exitInet();
    }
    long    tv_sec = elapsedEndTime->tv_sec - elapsedStartTime->tv_sec;
    long    tv_usec = elapsedEndTime->tv_usec - elapsedStartTime->tv_usec;
    long    seconds = 1 - tv_sec;
    long    milli = 0 - tv_usec;

    //adjust seconds for every 1M MicroSeconds
    printf("s:%ld m:%ld\n", seconds, milli);
    while (milli < 0) {
        seconds -= 1;
        milli += 1000000;
    }
    printf("sec:%ld\n", seconds);
    if ((seconds == 0 && milli == 0) || seconds < 0)
    {
        seconds = 0;
        milli=1;
    }
    //return (FALSE);
    /*if () {
        return (FALSE);
    }*/
    elapsedEndTime->tv_sec = seconds;
    elapsedEndTime->tv_usec = milli;
    socklen_t len = sizeof(*elapsedEndTime);
    //struct timeval a, b;
    //gettimeofday(&a, NULL);
    if (setsockopt(fdSocket, SOL_SOCKET, SO_RCVTIMEO, elapsedEndTime, len) != 0) {
        dprintf(2, "%s", "Couldn't set option RCVTIMEO socket.\n");
        exitInet();
    }
    return (TRUE);
}

/* Get request response */
static int    icmpGetResponse(/*struct timeval *elapsedStartTime*/) {
    char buff[ECHO_REPLY_SIZE];
    //struct timeval elapsedEndTime;
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvA;
    int result = -1;

   // fd_set rfds;FD_ZERO(&rfds);FD_SET(fdSocket, &rfds);

    //init response
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    ft_memset(buff, 0, ECHO_REPLY_SIZE);
    msg[0].iov_base = buff;
    msg[0].iov_len = sizeof(buff);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    int cpyErrno = errno;
    //now need to correct elapsed time
    //float a = 0;
    //int val = substractDelta(&elapsedEndTime, elapsedStartTime);
    //(void)val;
    result = recvmsg(fdSocket, &msgResponse, 0);
    if (result < 0
        && errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
        //alarm(0);
        dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
        exitInet();
    }
    //dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
    //printf("res:%d e:%d\n", result, errno);
    if (result == -1) {
      //  printf("ret\n");
        //if (gettimeofday(elapsedStartTime, 0) < 0) {
        //    exitInet();
        //}
        return (TRUE);
    }
    if (!end) {
        errno = cpyErrno;
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

/* send ping using signal alarm */
/*
void    sigHandlerAlrm(int sigNum) {
    if (sigNum != SIGALRM)
        return ;
    interrupt = TRUE;
    ++nb;
    if (end)
	    alarm(0);
    return ;
}
*/

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
            convertEndianess(pingMemory[0].icmp.un.echo.id),
            convertEndianess(pingMemory[0].icmp.un.echo.id));
}

void    sendPacket(int num) {
    if (num != SIGALRM)
        return ;
    if (end) {
        return ;
    }
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
        freeaddrinfo(listAddr);
        listAddr = NULL;
        dprintf(2, "ping: sending packet: %s\n", strerror(errno));
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
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
void    runIcmp() {
    if (!listAddr)
        exitInet();
    //struct timeval tvB;
    //struct timeval elapsedStartTime;
    char buff[ECHO_REQUEST_SIZE];
    //int result = -1;
    int cpyI;

    if (signal(SIGALRM, sendPacket) == SIG_ERR)
        exitInet();
    //init vars part
    //display ping header part
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
        /*ft_memset(buff, 0, ECHO_REQUEST_SIZE);
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
            freeaddrinfo(listAddr);
            listAddr = NULL;
            dprintf(2, "ping: sending packet: %s\n", strerror(errno));
            if (fdSocket >= 0)
                close(fdSocket);
            exit(1);
        }
        if (gettimeofday(&elapsedStartTime, 0) < 0) {
            exitInet();
        }*/
        //Call another ping
        //alarm(1);
        //int interrupt = FALSE;
//printf("send\n");
        //while (!end){// && !interrupt) {
            /*interrupt =*/ icmpGetResponse(/*&elapsedStartTime*/);
            //if (gettimeofday(&elapsedStartTime, 0) < 0) {
            //    exitInet();
            //}
        //}
    }
    if (end != TRUE) {
        dprintf(2, "ping: sending packet: %s\n", strerror(end));
        exitInet();
    }
    signalEnd();
}
