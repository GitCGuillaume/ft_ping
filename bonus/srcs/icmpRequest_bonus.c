#include "ft_icmp_bonus.h"
#include "tools_bonus.h"

static struct timeval gTimer;



/* Get request response */
static void    icmpGetResponse() {
    char buff2[ECHO_REPLY_SIZE];
    struct msghdr msgResponse;
    struct iovec msg[1];
    struct timeval tvA;
    int result = -1;

    /*struct timeval test;
    gettimeofday(&test, NULL);
    double  milliSeconds;
    time_t seconds = test.tv_sec - gTimer.tv_sec;
    suseconds_t microSeconds = test.tv_usec - gTimer.tv_usec;
    milliSeconds = (seconds * 1000.000000) + (microSeconds / 1000.000000);
    printf("time: %f %ld %ld\n", milliSeconds, test.tv_usec, gTimer.tv_usec);
    */
    //int resultAlrm = FALSE;
    //init response
    ft_memset(&msgResponse, 0, sizeof(struct msghdr));
    //ft_memset(buff2, 0, ECHO_REPLY_SIZE);
    msg[0].iov_base = buff2;
    msg[0].iov_len = sizeof(buff2);
    msgResponse.msg_iov = msg;
    msgResponse.msg_iovlen = 1;
    int cpyErrno = errno;
    //result = recvmsg(fdSocket, &msgResponse, MSG_DONTWAIT);
    result = recvmsg(fdSocket, &msgResponse, 0);
    if (result < 0
        && errno != EWOULDBLOCK && errno != EAGAIN) {
        alarm(0);
        dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
        exitInet();
    }
    //printf("res:%d\n", result);
    //printf("int:%d\n", interrupt);
    //exit(1);
    while (!end && result == -1 && !interrupt) {
        result = recvmsg(fdSocket, &msgResponse, 0);
        if (result < 0
            && errno != EWOULDBLOCK && errno != EAGAIN) {
            alarm(0);
            dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
            exitInet();
        }
        //printf("res:%d errno:%d\n", result, errno);
        //if (t_flags.flagI == FALSE) {
            //usleep(1);
            //errno = cpyErrno;if (sigNum != SIGALRM)
       // return ;
        //}
    }
    if (end || (result == -1 && interrupt == TRUE)) {
        t_flags.preload = 0;
        return ;//(FALSE);
    }
    //printf("result:%d\n", result);
    errno = cpyErrno;
    //printf("result: %d\n", resultAlrm);
    //if (resultAlrm == -1)
    //    return (FALSE);
    //printf("result: %d\n", result);
    if (gettimeofday(&tvA, 0) < 0)
        exitInet();
    //result - sizeof(struct iphdr) = whole response minus ip header (84-20)=64 
    //echo reply is 64 like a standard reply from the ping inetutils2.0
    icmpInitResponse(&msgResponse, result - sizeof(struct iphdr), &tvA);
    //printf("sleep:%d\n", usleep(1000000));
    //return (TRUE);
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
static void    sigHandlerAlrm(int sigNum) {
    if (sigNum != SIGALRM)
        return ;
    interrupt = TRUE;
    ++nb;
    return ;
    int cpyI = roundTripGlobal.packetSend % 65536;

    
    if (!listAddr)
        exitInet();
    struct timeval tvB;
    char buff[ECHO_REQUEST_SIZE];
    int result = -1;

    //init part
    if (cpyI == 65535)
        ft_memset(pingMemory, 0, sizeof(pingMemory));
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    //if (USHRT_MAX < roundTripGlobal.packetReq)
    //    roundTripGlobal.packetReq = 0;
    initPing(&pingMemory[cpyI], cpyI);
    roundTripGlobal.packetSend++;
    //roundTripGlobal.packetReq++;
    if (gettimeofday(&tvB, 0) < 0) {
        exitInet();
    }
    if (t_flags.w)
        timerFlagExit(&tvB, gTimer);
    fillBuffer(buff, &pingMemory[cpyI], &tvB);
    //printf("send\n");
    //printf("c:%d\n", cpyI);   
   // if (!t_flags.preload) {
   //     printf("qsdqsd\n");
       /* printf("prealod\n");
        printf("c:%d\n", cpyI);
        struct itimerval new, old;
        new.it_interval.tv_sec = 1;
        new.it_interval.tv_usec = 0;
        new.it_value.tv_sec = 1;
        new.it_value.tv_usec = 0;*/
            //new.it_value.tv_sec, new.it_value.tv_usec;
       // printf("ret:%d\n", setitimer(ITIMER_REAL, &new, &old));
        //alarm(1);
    //} else {
    //}
    //if (0 < t_flags.preload)
    //    t_flags.preload--;
    //if (0 < t_flags.preload)
    //    t_flags.preload--;
    result = sendto(fdSocket, buff,
    ECHO_REQUEST_SIZE, 0,
    listAddr->ai_addr, listAddr->ai_addrlen);
    //pr.4intf("sent\n");
    if (result < 0) {
        //alarm(0);
        //printf("falaid2: %d\n", 0);
        //if (listAddr)
        //    freeaddrinfo(listAddr);
        //listAddr = NULL;
        //printf("send\n");
        //printf("fd: %d\nerrno:%d\n", fdSocket, errno);
        end = errno;
        //dprintf(2, "ping: sending packet: %s\n", strerror(errno));
        //if (fdSocket >= 0)
        //    close(fdSocket);
        //exit(1);
    }
    //if (t_flags.preload)
      //  t_flags.preload--;
    //Call another ping
    //if (!t_flags.preload)
    //if (t_flags.interval == -1)
    //    alarm(1);
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
            convertEndianess(pingMemory[0].icmp.un.echo.id),
            convertEndianess(pingMemory[0].icmp.un.echo.id));
}
/*
void    launchPreload(void) {
    if (t_flags.preload) {
        for (uint32_t i = 0; i < t_flags.preload; ++i) {
            sigHandlerAlrm(SIGALRM);
            icmpGetResponse();
        }
        t_flags.preload = 0;
    }
}*/

/*
    type + code important for internet control message protocol
    https://www.ibm.com/docs/fr/qsip/7.5?topic=applications-icmp-type-code-ids
    https://erg.abdn.ac.uk/users/gorry/course/inet-pages/icmp-code.html
    tldr : you need type, then you have access to a list of code
    checksum is a error detection method
*/
/* (ipv4 max)65535 - (sizeof ip)20 - (sizeof icmp)8 */
void    runIcmp() {
    //char preloadExist = FALSE;
    if (!listAddr)
        exitInet();
    struct timeval tvB;
    char buff[ECHO_REQUEST_SIZE];
    int result = -1;
    //int resultRecv = FALSE;
    int cpyI;

    if (signal(SIGALRM, sigHandlerAlrm) == SIG_ERR)
        exitInet();
    //get timestamp for ping payload
    struct itimerval new, old;
    float it_usec = t_flags.interval - (long)t_flags.interval;// * t_flags.dividend;
    float it_sec = t_flags.interval - (long)t_flags.interval;
    
    new.it_interval.tv_sec = (long)it_sec;
    new.it_interval.tv_usec = (long)(it_usec * 1000000.0f);//(long)(it_usec * CONV_SEC_TO_MICR) / t_flags.dividend;//t_flags.interval - (int)t_flags.interval;
    new.it_value.tv_sec = (long)it_sec;
    new.it_value.tv_usec = (long)(it_usec * 1000000.0f);//(long)(it_usec * CONV_SEC_TO_MICR) / t_flags.dividend;//(int)(t_flags.interval - (int)t_flags.interval);
    if (!new.it_interval.tv_sec
        && !new.it_interval.tv_usec) {
        new.it_interval.tv_sec = 1;
        new.it_value.tv_sec = 1;
    }

    double  milliSeconds;
    time_t seconds = new.it_value.tv_sec;
    suseconds_t microSeconds = new.it_value.tv_usec;
    milliSeconds = (seconds) + (microSeconds / 1000000.0f);
    printf("time: %f %ld %ld sec: %ld it_usec: %ld it:%f interval: %ld div:%d\n",
        milliSeconds, seconds, microSeconds, (long)it_sec, (long)(it_usec * 1000000.0f), t_flags.interval, (long)t_flags.interval, t_flags.dividend);
    printf("%ld %ld", new.it_interval.tv_sec, new.it_interval.tv_usec);
    //exit(1);
  
    //t_flags.time = ((new.it_interval.tv_sec) * 1000000)
    //    + (new.it_interval.tv_usec);
    //printf("usec: %ld\n", new.it_interval.tv_usec);
    //exit(1);
    //    new.it_value.tv_sec, new.it_value.tv_usec;
    if (setitimer(ITIMER_REAL, &new, &old) != 0) {
        alarm(0);
        //if (listAddr)
        //    freeaddrinfo(listAddr);
        dprintf(2, "ping: timer interval: %s\n", strerror(errno));
        exitInet();
        //if (fdSocket >= 0)
        //    close(fdSocket);
        //exit(1);
    }
    //if (t_flags.w) {
        if (gettimeofday(&gTimer, 0) < 0) {
            exitInet();
        }
    //}
    //display ping header
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    cpyI = roundTripGlobal.packetSend % 65536;
    initPing(&pingMemory[cpyI], 0);
    displayPingHeader();
    
    while (!end) {
    //if (interrupt == TRUE) {
        for (uint32_t preloadI = 0; preloadI <= t_flags.preload; ++preloadI) {
//            if (end)
  //              break ;
            //if (USHRT_MAX < roundTripGlobal.packetReq)
            //    roundTripGlobal.packetReq = 0;
            cpyI = roundTripGlobal.packetSend % 65536;
            if (cpyI == 0)
                ft_memset(pingMemory, 0, sizeof(pingMemory));
            initPing(&pingMemory[cpyI], cpyI);
            if (gettimeofday(&tvB, 0) < 0) {
                exitInet();
            }
            if (t_flags.w)
                timerFlagExit(&tvB, gTimer);
            fillBuffer(buff, &pingMemory[cpyI], &tvB);
            //inc nb packets and send
            roundTripGlobal.packetSend++;
            //roundTripGlobal.packetReq++;
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
            //icmpGetResponse();
        }
        interrupt = FALSE;
    //}
    //Call another ping
    //alarm(1);
    /*if (0 < t_flags.preload) {
        struct itimerval new, old;
        new.it_interval.tv_sec = 0;
        new.it_interval.tv_usec = 1;
        new.it_value.tv_sec = 0;
        new.it_value.tv_usec = 1;
        preloadExist = TRUE;
        //    new.it_value.tv_sec, new.it_value.tv_usec;
        printf("ret:%d\n", setitimer(ITIMER_REAL, &new, &old));
    } else {
    */    //printf("????????\n");
    /*struct itimerval new, old;
    new.it_interval.tv_sec = 1;
    new.it_interval.tv_usec = 0;
    new.it_value.tv_sec = 1;
    new.it_value.tv_usec = 0;
    //    new.it_value.tv_sec, new.it_value.tv_usec;
    printf("ret:%d\n", setitimer(ITIMER_REAL, &new, &old));
    */
    //if (t_flags.interval < 0.0f) {
    //    alarm(1);
    //} else {
        
    //}
        /*if (preloadExist == TRUE
            && t_flags.preload < roundTripGlobal.packetSend) {
            struct itimerval new, old;
            new.it_interval.tv_sec = 1;
            new.it_interval.tv_usec = 0;
            new.it_value.tv_sec = 1;
            new.it_value.tv_usec = 0;
            preloadExist = FALSE;
            printf("end\n");
            printf("ret:%d\n", setitimer(ITIMER_REAL, &new, &old));
        }*/
        //printf("plop\n");
        while (!end && !interrupt) {
            icmpGetResponse();
        }
    }
    if (end != TRUE) {
        alarm(0);
        printf("this is err\n");
        //signal(SIGALRM, SIG_DFL);
        dprintf(2, "ping: sending packet: %s\n", strerror(end));
        exitInet();
    }
    signalEnd();
}