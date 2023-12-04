flags_bonus.h {
    struct s_flags {
    char    v;
    char    interrogation;
    uint32_t    tos;
    uint32_t    ttl;
    uint32_t    w;
    ssize_t    preload;
    double    interval;
    char    pattern[40];
};
}

pto {
    double    bigCallParseInterval(char *argv[], int i, int j) {
        double value = 0;

        findArgumentEq(argv, i, j);
        if (!argv[i][j]) {
            value = parseArgumentI(argv[i], argv[i + 1], &argv[i + 1]);
            argv[i + 1] = NULL;
        }
        else {
            value = parseArgumentI(argv[i], argv[i], &argv[i]);
            argv[i] = NULL;
        }
        return (value);
    }

    double    callParseInterval(char *argv[], int i) {
        double value = 0.0;

        if (findArgument(argv, i) == TRUE) {
            value = parseArgumentI(argv[i], argv[i], &argv[i]);
            argv[i] = NULL;
        }
        else {
            value = parseArgumentI(argv[i], argv[i + 1], &argv[i + 1]);
            argv[i + 1] = NULL;
        }
        return (value);
    }

}

parsingTollsTwo {
    /* convert integer and decimal parts into a result, and test it's value */
    static double convertIntoDouble(uint64_t integerPart, uint64_t decimalPart,
        size_t dividend, const char *original) {
        double   result = 0.0f;
        size_t  convert = 0;

        result = (double)integerPart + ((double)decimalPart / dividend);
        convert = result * 1000000;
        //printf("co:%ld\n", convert);
        //exit(1);
        if (convert < 200000) {
            dprintf(2, "ping: option value too small: %s\n", original);
            exit(1);
        }
        return (result);
    }

    double    parseArgumentI(const char *cmd,
        const char *original, char **str) {
        uint32_t    i = 0;
        uint64_t    integerPart = 0;
        uint64_t    decimalPart = 0;
        size_t    dividend = 1;

        requireArgument(cmd, original);
        findFirstDigit(str, &i);
        /*if (!str[0][0]) {
            return (0.0f);
        }*/
        if (str[0][i] == ',') {
            (*str) += i;
        }
        if (*str[0] && *str[0] != ',') {
            //get integer part into it's variable
            while (*str[0] && *str[0] != ',') {
                if (ft_isdigit(*str[0])) {
                    integerPart = integerPart * 10 + *str[0] - '0';
                } else {
                    dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                        original, str[0]);
                    dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                    exit(1);
                }
                (*str)++;
            }
        }
        //get decimal part into it's variable
        while (*str[0]) {
            if (*str[0] == ',')
                (*str)++;
            if (!*str[0])
                break ;
            if (ft_isdigit(*str[0])) {
                dividend *= 10;
                decimalPart = decimalPart * 10 + *str[0] - '0';
            } else {
                dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                    original, str[0]);
                dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                exit(1);
            }
            (*str)++;
        }
        return (convertIntoDouble(integerPart, decimalPart, dividend, original));
    }
}

toolsTwoBonus {
    void    switchFlags(char *argv[], int pos, int i, int len) {
    switch (pos) {
        case 0:
            t_flags.v = TRUE;
            break ;
        case 1:
            t_flags.w = bigCallParseArgument(argv, i, len, 2147483647);
            break ;
        case 2:
            t_flags.tos = bigCallParseArgument(argv, i, len, 255);
            break ;
        case 3:
            t_flags.ttl = bigCallParseArgument(argv, i, len, 255);
            break ;
        case 4:
            t_flags.preload = bigCallParsePreload(argv, i, len, 2147483647);
            break ;
        case 5:
            t_flags.interval = bigCallParseInterval(argv, i, len);
            break ;
        case 6:
            bigCallParsePattern(argv, i, len);
            break ;
        case 7:
            t_flags.interrogation = TRUE;
            flagInterrogation();
            exit(0);
            break ;
        case 8:
            flagUsage();
            exit(0);
            break ;
    }
}
}

static int openSocket() {
    if (!listAddr)
        exit(EXIT_FAILURE);
    struct addrinfo *mem = listAddr;
    ssize_t    ttl = t_flags.ttl;
    int     fd = -1;

    struct timeval new;
    float it_usec = t_flags.interval - (long)t_flags.interval;
    float it_sec = (long)t_flags.interval;

    new.tv_sec = (long)it_sec;
    new.tv_usec = (long)(it_usec * 1000000.0f) % 1000000;
    if (ttl == 0) {
        dprintf(2, "ping: option value too small: %ld\n", ttl);
        freeaddrinfo(listAddr);
        exit(1);
    } else if (ttl < 0 || ttl > 255) {
        dprintf(2, "ping: option value too big: %ld\n", ttl);
        freeaddrinfo(listAddr);
        exit(1);
    }
    while (mem)
    {
        fd = socket(mem->ai_family, mem->ai_socktype, mem->ai_protocol);
        if (fd < 0) {
            dprintf(2, "%s", "Couldn't open socket.\n");
            freeaddrinfo(listAddr);
            exit(EXIT_FAILURE);
        }
        else if (fd >= 0){
            if (setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
                dprintf(2, "%s", "Couldn't set option TTL socket.\n");
                if (fdSocket >= 0)
                    close(fdSocket);
                exit(EXIT_FAILURE);
            }
            if (t_flags.tos != 0) {
                if (setsockopt(fd, IPPROTO_IP, IP_TOS, &t_flags.tos, sizeof(t_flags.tos)) != 0) {  
                    dprintf(2, "%s", "Couldn't set option TTL socket.\n");
                    if (fdSocket >= 0)
                        close(fdSocket);
                    exit(EXIT_FAILURE);
                }
            }
            if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &new, sizeof(new)) != 0) {
                dprintf(2, "%s", "Couldn't set option RCVTIMEO socket.\n");
                if (fdSocket >= 0)
                    close(fdSocket);
                exit(EXIT_FAILURE);
            }
            break ;
        }
        mem = mem->ai_next;
    }
    return (fd);
}

int    similarFlags(int same[8], const char *memory) {
    //display ambiguous
    const char  *memFlag[9] = {
        "--verbose", "--timeout", "--tos", "--ttl",
        "--preload", "--interval", "--pattern",
        "--help", "--usage"
    };
    //if > 1 error
    int similar = 0;
    //save function to launch into pos
    int pos = 0;

    for (int i = 0; i < 9; i++) {
        if (same[i] == TRUE) {
            ++similar;
            pos = i;
        }
    }
    if (similar > 1) {
        printf("ping: option \'%s\' is ambiguous; possibilities:", memory);
        for (int i = 0; i < 9; i++) {
            if (same[i] == TRUE) {
                printf(" \'%s\'", memFlag[i]);
            }
        }
        printf("\nTry 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
    return (pos);
}

static  void    searchBigOption(char *argv[], int argc) {
    int nullable;
    char    *memory = NULL;
    int len;
    int same[9];

    ft_memset(&same, 0, 9 * sizeof(int));
    for (int i = 1; i < argc; ++i) {
        if (argv[i] && argv[i][0] == '-' && argv[i][1] == '-') {
            nullable = FALSE;
            len = ftStrlenWiEqual(argv[i]);
            if (len <= 9 && !ft_strncmp(argv[i], "--verbose", len)) {
                same[0] = TRUE;
                nullable = TRUE;
            }
            if(len <= 9 && !ft_strncmp(argv[i], "--timeout", len))
                same[1] = TRUE;
            if(len <= 5 && !ft_strncmp(argv[i], "--tos", len))
                same[2] = TRUE;
            if(len <= 5 && !ft_strncmp(argv[i], "--ttl", len))
                same[3] = TRUE;
            if (len <= 9 && !ft_strncmp(argv[i], "--preload", len))
                same[4] = TRUE;
            if (len <= 10 && !ft_strncmp(argv[i], "--interval", len))
                same[5] = TRUE;
            if (len <= 9 && !ft_strncmp(argv[i], "--pattern", len))
                same[6] = TRUE;
            if (len <= 6 && !ft_strncmp(argv[i], "--help", len)) {
                same[7] = TRUE;
            }
            if (len <= 7 && !ft_strncmp(argv[i], "--usage", len)) {
                same[8] = 8;
            }
            if (!memory)
                memory = argv[i];
            int pos = similarFlags(same, memory);
            switchFlags(argv, pos, i, len);
            ft_memset(&same, 0, 9 * sizeof(int));
            if (nullable) {
                argv[i] = NULL;
                nullable = FALSE;
            }
        }
    }
}

static void    searchFlags(char *argv[], int argc) {
    int nullable;
    int j;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] && argv[i][0] == '-' && argv[i][1] != '-') {
            nullable = FALSE;
            for (j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'v':
                        t_flags.v = TRUE;
                        nullable = TRUE;
                        continue ;
                    case '?':
                        t_flags.interrogation = TRUE;
                        flagInterrogation();
                        exit(0);
                        break ;
                    case 'w':
                        t_flags.w = callParseArgument(argv, i, 2147483647);
                        break ;
                    case 'T':
                        t_flags.tos = callParseArgument(argv, i, 255);
                        break ;
                    case 'l':
                        t_flags.preload = callParsePreload(argv, i, 2147483647);
                        break ;
                    case 'i':
                        t_flags.interval = callParseInterval(argv, i);
                        break ;
                    case 'p':
                        callParsePattern(argv, i);
                        break ;
                    default:
                        dprintf(2, "%s%c%c\n", "ping: invalid option -- \'", argv[i][j], '\'');
                        dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                        exit(64);
                }
            }
            if (nullable) {
                argv[i] = NULL;
                nullable = FALSE;
            }
        }
    }
}

int main() {t_flags.interval = 1.0f;}

static int    substractDelta(struct timeval elapsedEndTime, struct timeval *elapsedStartTime) {
    if (gettimeofday(&elapsedEndTime, 0) < 0) {
        exitInet();
    }
    float   it_usec = t_flags.interval - (long)t_flags.interval;
    long    it_sec = (long)t_flags.interval;
    long    tv_sec = elapsedEndTime.tv_sec - elapsedStartTime->tv_sec;
    long    tv_usec = (elapsedEndTime.tv_usec - elapsedStartTime->tv_usec);
    long    convertUsec = (long)(it_usec * 1000000);
    long    seconds = it_sec - tv_sec;
    long    milli = convertUsec - tv_usec ;

    //adjust seconds for every 1M MicroSeconds
    while (milli < 0) {
        seconds -= 1;
        milli += 1000000;
    }
    if ((seconds == 0 && milli == 0) || seconds < 0)
        return (FALSE);
    //milli = 1;
    elapsedEndTime.tv_sec = seconds;
    elapsedEndTime.tv_usec = milli;
    if (setsockopt(fdSocket, SOL_SOCKET, SO_RCVTIMEO, &elapsedEndTime, sizeof(elapsedEndTime)) != 0) {
        dprintf(2, "%s", "Couldn't set option RCVTIMEO socket.\n");
        exitInet();
    }
    return (TRUE);
}

static int    icmpGetResponse(struct timeval *elapsedStartTime) {
    char buff[ECHO_REPLY_SIZE];
    (void)elapsedStartTime;
   // struct timeval elapsedEndTime;
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
    //now need to correct elapsed time
    /*if (!substractDelta(elapsedEndTime, elapsedStartTime)) {
        result = recvmsg(fdSocket, &msgResponse, MSG_DONTWAIT);
        if (result < 0
            && errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
            //alarm(0);
            dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
            exitInet();
        }
    } else {
    */
        result = recvmsg(fdSocket, &msgResponse, 0);
        if (result < 0
            && errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
            //alarm(0);
            dprintf(2, "ping: receiving packet: %s\n", strerror(errno));
            exitInet();
        }
    //}
    if (end || result == -1){
        t_flags.preload = 0;
        return (TRUE);
    }
    errno = cpyErrno;
    if (gettimeofday(&tvA, 0) < 0)
        exitInet();
    //result - sizeof(struct iphdr) = whole response minus ip header (84-20)=64
    //echo reply is 64 like a standard reply from the ping inetutils2.0
    icmpInitResponse(&msgResponse, result - sizeof(struct iphdr), &tvA);
    return (FALSE);
}

void    runIcmp() {
    if (!listAddr)
        exitInet();
    struct timeval gTimer;
    struct timeval tvB;
    struct timeval elapsedStartTime;
    char buff[ECHO_REQUEST_SIZE];
    int result = -1;
    int cpyI;

    if (gettimeofday(&gTimer, 0) < 0) {
        exitInet();
    }
    //display ping header part
    ft_memset(buff, 0, ECHO_REQUEST_SIZE);
    cpyI = roundTripGlobal.packetSend % 65536;
    if (cpyI == 0)
        ft_memset(pingMemory, 0, sizeof(pingMemory));
    initPing(&pingMemory[cpyI], cpyI);
    displayPingHeader();
    //end display ping header
    while (!end) {
        for (uint32_t preloadI = 0; preloadI <= t_flags.preload; ++preloadI) {
            if (end)
                break ;
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
            }
        }
        int interrupt = FALSE;

        while (!end && !interrupt) {
            interrupt = icmpGetResponse(&elapsedStartTime);
        }
    }
    if (end != TRUE) {
        dprintf(2, "ping: sending packet: %s\n", strerror(end));
        exitInet();
    }
    signalEnd();
}
