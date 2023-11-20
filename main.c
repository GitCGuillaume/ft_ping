#include "tools.h"
#include "flags.h"
#include "ft_icmp.h"

struct s_ping_memory    pingMemory[65536];
struct  addrinfo *listAddr = 0;//need to be cleaned in CTRL+C + alarm(function is sigHandlerAlrm)
struct s_round_trip  roundTripGlobal;//mainly for signal function...
struct s_flags t_flags;
int fdSocket;//must be also closed on CTRL+C etc

/*
    Heron's method
    https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Heron's_method
*/
static double   ftSqrt(double num) {
    double x = num;
    double old = 0.000000;

    if (num < 0 || num == 0)
        return (0.0f);
    while (x != old){
        old = x;
        x = (x + num / x) / 2;
    }
    return (x);
}

/*
    https://en.wikipedia.org/wiki/Standard_deviation
    https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
*/
static void    sigHandlerInt(int sigNum) {
    double  average;
    double  stdDev = 0.0f;

    if (sigNum != SIGINT)
        return ;
    if (!listAddr) {
        if (fdSocket >= 0)
            close(fdSocket);
        exit(1);
    }
    if (fdSocket >= 0)
        close(fdSocket);
    if (roundTripGlobal.number != 0) {
        average = roundTripGlobal.sum / roundTripGlobal.number;
        stdDev = ftSqrt((roundTripGlobal.squareSum / roundTripGlobal.number) - (average * average));
    }
    printf("--- %s ping statistics ---\n", listAddr->ai_canonname);
    printf("%u packets transmitted, %u packets received",
        roundTripGlobal.packetSend,
        roundTripGlobal.packetReceive);
    if (roundTripGlobal.packetDuplicate != 0)
        printf(", +%u duplicates", roundTripGlobal.packetDuplicate);
    if (roundTripGlobal.packetReceive > roundTripGlobal.packetSend)
        printf(", -- somebody is printing forged packets!\n");
    else if (roundTripGlobal.packetSend != 0) {
        //inetutils's ping command seem to not display packet loss
        double loseRatePct = (((double)roundTripGlobal.packetSend - (
            double)roundTripGlobal.packetReceive) / (double)roundTripGlobal.packetSend)
            * 100.000000;
        printf(", %.0d%% packet loss\n", (int)loseRatePct);
    }
    if  (listAddr)
        freeaddrinfo(listAddr);
    if (roundTripGlobal.number != 0) {
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f\n",
        roundTripGlobal.rtt[0],
        average,
        roundTripGlobal.rtt[1],
        stdDev);
        exit(0);
    }
    exit(1);
}

/* code /usr/include/x86_64-linux-gnu/bits/in.h
    Options for use with `getsockopt' and `setsockopt' at the IP level.
    IP_TTL...
 */
static int openSocket() {
    if (!listAddr)
        exit(EXIT_FAILURE);
    struct addrinfo *mem = listAddr;
    int    ttl = 60;
    int     fd = -1;

    if (ttl == 0) {
        dprintf(2, "ping: option value too small: %d\n", ttl);
        freeaddrinfo(listAddr);
        exit(1);
    } else if (ttl < 0 || ttl > 255) {
        dprintf(2, "ping: option value too big: %d\n", ttl);
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
            break ;
        }
        mem = mem->ai_next;
    }
    return (fd);
}

static void     searchBigOption(char *argv[], int argc) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i] && argv[i][0] == '-' && argv[i][1] == '-') {
            if (!ft_strncmp(argv[i], "--verbose", 9))
                    t_flags.v = TRUE;
            else if (!ft_strncmp(argv[i], "--help", 6)) {
                t_flags.interrogation = TRUE;
                    flagInterrogation();
                    exit(0);
            } else if (!ft_strncmp(argv[i], "--usage", 7)) {
                    flagUsage();
                    exit(0);
            } else {
                dprintf(2, "%s%s%c\n", "ping: unrecognized option \'", argv[i], '\'');
                dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                exit(64);
            }
        }
    }
}

static void    searchFlags(char *argv[], int argc) {
    int j = 0;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'v')
                    t_flags.v = TRUE;
                else if (argv[i][j] == '?') {
                    t_flags.interrogation = TRUE;
                    flagInterrogation();
                    exit(0);
                }
                else {
                    dprintf(2, "%s%c%c\n", "ping: invalid option -- \'", argv[i][j], '\'');
                    dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                    exit(64);
                }
            }
        }
    }
}

/*man getaddrinfo > man 5 services (0)*/
static struct addrinfo *getIp(char *argv[], int argc, int *i) {
    struct addrinfo *list = 0, client;
    //struct sockaddr_in *translate;
    int result = 0;

    for (; *i < argc; ++(*i)) {
        if (argv[*i] && argv[*i][0] != '-') {
            ft_memset(&client, 0, sizeof(struct addrinfo));
            client.ai_family = AF_INET;
            client.ai_socktype = SOCK_RAW;
            client.ai_protocol = IPPROTO_ICMP;
            client.ai_flags = AI_CANONNAME;
            result = getaddrinfo(argv[*i], NULL, &client, &list);
            if (result != 0) {
                dprintf(STDERR, "ping: %s\n", "unknown host");
                if (list)
                    freeaddrinfo(list);
                exit(EXIT_FAILURE);
            }
            break ;
        }
    }
    if (t_flags.interrogation == FALSE && !list) {
        dprintf(2, "%s",
            "ping: missing host operand\nTry 'ping -?' for more information.\n");
        if (list)
            freeaddrinfo(list);
        exit(64);
    }
    return (list);
}

static void    pingStart(int argc, char *argv[]) {
    int     i = 1;

    //init part
    if (signal(SIGINT, sigHandlerInt) == SIG_ERR)
        exitInet();
    for (; i < argc; ++i) {
        listAddr = getIp(argv, argc, &i);
        fdSocket = openSocket();
        //next part ping here
        runIcmp();
        freeaddrinfo(listAddr);
        if (fdSocket >= 0)
            close(fdSocket);
    }
}

//ping [OPTIONS] host
int main(int argc, char *argv[]) {
    char    *cpyArgv[argc];

    //copy argv since I will manipulate argv values
    for (int i = 0; i < argc; i++)
        cpyArgv[i] = argv[i];
    cpyArgv[argc] = NULL;
    ft_memset(pingMemory, 0, sizeof(pingMemory));
    ft_memset(&roundTripGlobal, 0, sizeof(struct s_round_trip));
    t_flags.v = FALSE;
    t_flags.interrogation = FALSE;
    //init round trip time
    if (getuid() != 0) {
        dprintf(2, "%s", "Please use root privileges.\n");
        return (EXIT_FAILURE);
    }
    if (argc < 2) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping -?' for more information.\n");
        exit(64);
    }
    for (int i = 1; i < argc; i++) {
        if (cpyArgv[i][0] == '-'
            && cpyArgv[i][1] == '-'
            && cpyArgv[i][2] == 'v') {
            dprintf(2, "ping: option '--v' is ambiguous; possibilities: '--verbose' '--version'\n");
            dprintf(2, "Try 'ping --help' or 'ping --usage' for more information.\n");
            exit(64);
        }
        if (cpyArgv[i] && cpyArgv[i][0] == '-'
            && cpyArgv[i][1] != '-') {
            searchFlags(cpyArgv, argc);
            break ;
        }
    }
    for (int i = 1; i < argc; i++) {
        if (cpyArgv[i] && cpyArgv[i][0] == '-'
            && cpyArgv[i][1] == '-') {
            searchBigOption(cpyArgv, argc);
            break ;
        }
    }
    pingStart(argc, cpyArgv);
    return (0);
}
