#include "tools_bonus.h"
#include "flags_bonus.h"
#include "ft_icmp_bonus.h"

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
    //x is whatever value near square root
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
    https://fr.wikipedia.org/wiki/Variance_(mathématiques)
*/
void    sigHandlerInt(int sigNum) {
    double  average;
    double  stdDev = 0.0f;

    if (sigNum != SIGINT)
        return ;
    if (!listAddr) {
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
        printf(", %d%% packet loss\n", (int)loseRatePct);
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
    ssize_t    ttl = t_flags.ttl;
    int     fd = -1;

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
            break ;
        }
        mem = mem->ai_next;
    }
    return (fd);
}

static void requireArgument(const char *cmd, const char *original) {
    //printf("or:%s\n", original);
    if (!original) {
        if (cmd[0] == '-')
            dprintf(2, "ping: option \'-%s\' requires an argument\n", cmd);
        else
            dprintf(2, "ping: option \'%s\' requires an argument\n", cmd);
        dprintf(2, "Try 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
}

uint32_t    parseArgument(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    uint32_t    result = 0;
    uint32_t    jmpSpace = 0;

    requireArgument(cmd, original);
    while (str[0][jmpSpace] && str[0][jmpSpace] == ' ') {
        ++jmpSpace;
    }
    if (!str[0][jmpSpace]) {
        dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
            original, str[0]);
        exit(1);
    }
    (*str) += jmpSpace;
    for (; *str[0] != 0; (*str)++) {
        if (ft_isdigit(*str[0])) {
            result = result * 10 + *str[0] - '0';
        } else {
            dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                original, str[0]);
            exit(1);
        }
        if (result == 0) {
            dprintf(2, "ping: option value too small: %s\n", original);
            exit(1);
        } else if (result > maxValue) {
            dprintf(2, "ping: option value too big: %s\n", original);
            exit(1);
        }
    }
    //printf("res:%u\n", result);
    return (result);
}

uint32_t    parsePreload(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    uint32_t    result = 0;
    uint32_t    i = 0;

    requireArgument(cmd, original);
    while (str[0][i] && str[0][i] == ' ')
        ++i;
    if (str[0][i])
        (*str) += i;
    for (; *str[0] != 0; (*str)++) {
        if (ft_isdigit(*str[0])) {
            result = result * 10 + *str[0] - '0';
        } else {
            dprintf(2, "ping: invalid preload value (%s)\n", original);
            exit(1);
        }
        if (result > maxValue) {
            dprintf(2, "ping: invalid preload value (%s)\n", original);
            exit(1);
        }
    }
    //printf("res:%u\n", result);
    return (result);
}

char    findArgument(char **str) {
    if (str[0][2]) {
        (*str) += 2;
        /*while (str[0][i] && str[0][i] == ' ')
            ++i;
        if (str[0][i])
            (*str) += i;
        */
        return (TRUE);
    }
    return (FALSE);
}


char    findEqualValue(char **str) {
    while (*str[0] && *str[0] != '=') {
        (*str)++;
    } 
   if (*str[0] && *str[0] == '=') {
        (*str)++;
        return (TRUE);
   }
   return (FALSE);
}

static void    searchFlags(char *argv[]) {
    int j;

    for (int i = 1; argv[i] != NULL; ++i) {
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'v')
                    t_flags.v = TRUE;
                else if (argv[i][j] == '?') {
                    t_flags.interrogation = TRUE;
                    flagInterrogation();
                    exit(0);
                } else if (argv[i][j] == 'w') {
                    if (findArgument(&argv[i]) == TRUE)
                        t_flags.w = parseArgument("w", argv[i], &argv[i], 2147483647);
                    else
                        t_flags.w = parseArgument("w", argv[i + 1], &argv[i + 1], 2147483647);
                    //if (t_flags.w == 0)
                    //    dprintf(2, "ping: option value too small: %d\n", t_flags.w);
                    //printf("f:%u\n", t_flags.w);
                    //t_flags.w = ft_strToUInt("w", (argv[i + 1]), &argv[i + 1]);
                    break ;
                } else if (argv[i][j] == 'T') {
                    if (findArgument(&argv[i]) == TRUE)
                        t_flags.tos = parseArgument("T", argv[i], &argv[i], 2147483647);
                    else
                        t_flags.tos = parseArgument("T", argv[i + 1], &argv[i + 1], 2147483647);
                    //if (t_flags.tos == 0)
                    //    dprintf(2, "ping: option value too small: %d\n", t_flags.tos);
                    //t_flags.tos = ft_strToShort("T", (argv[i + 1]), &argv[i + 1]);
                    break ;
                } else if(!ft_strncmp(&argv[i][j], "-ttl", 4)) {
                    if (findEqualValue(&argv[i]) == TRUE) {
                        //if (!argv[i][0])
                        t_flags.ttl = parseArgument("-ttl", argv[i], &argv[i], 255);
                    }
                    else
                        t_flags.ttl = parseArgument("-ttl", argv[i + 1], &argv[i + 1], 255);
                   // if (t_flags.ttl == 0)
                     //   dprintf(2, "ping: option value too small: %d\n", t_flags.ttl);
                    //t_flags.ttl = findTtl((argv[i] + 6), &argv[i]);
                    break ;
                }
                else if (argv[i][j] == 'l'
                    || !ft_strncmp(&argv[i][j], "-preload", 8)) {
                    if (argv[i][1] && argv[i][1] == '-') {
                        //check github issues
                        if (findEqualValue(&argv[i]) == TRUE)
                            t_flags.preload
                                = parsePreload(argv[i], argv[i], &argv[i], 2147483647);
                        else
                            t_flags.preload
                                = parsePreload(argv[i], argv[i + 1], &argv[i + 1], 2147483647);
                    } else {
                        t_flags.preload
                            = parsePreload(argv[i], argv[i + 1], &argv[i + 1], 2147483647);
                    }
                    break ;
                    //t_flags.preload = parsePreload(argv[i], argv[i + 1], &argv[i + 1]);
                }
                else if (argv[i][j] == 'i'
                    || !ft_strncmp(&argv[i][j], "-interval", 8)) {
                    if (argv[i][1] && argv[i][1] == '-') {
                        findEqualValue(&argv[i]);
                        t_flags.interval
                            = parseArgument(argv[i], argv[i], &argv[i], 2147483647);
                    } else {
                        t_flags.interval
                            = parseArgument(argv[i], argv[i + 1], &argv[i + 1], 2147483647);
                    }
                    break ;
                } else {
                    dprintf(2, "%s%c%c\n", "ping: invalid option -- \'", argv[i][j], '\'');
                    dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                    exit(64);
                }
            }
        }
    }
}

/*man getaddrinfo > man 5 services (0)*/
static struct addrinfo *getIp(char *argv[], int *i) {
    struct addrinfo *list = 0, client;
    //struct sockaddr_in *translate;
    int result = 0;

    for (; argv[*i] != NULL; ++(*i)) {
        if (*argv[*i] && argv[*i][0] != '-') {
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
        listAddr = getIp(argv, &i);
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
    char **memArgv = NULL;

    ft_memset(pingMemory, 0, sizeof(pingMemory));
    ft_memset(&roundTripGlobal, 0, sizeof(struct s_round_trip));
    t_flags.v = FALSE;
    t_flags.interrogation = FALSE;
    t_flags.ttl = 60;
    t_flags.tos = 0;
    t_flags.w = 0;
    t_flags.preload = 0;
    t_flags.interval = 0;
    //init round trip time
    if (getuid() != 0) {
        dprintf(2, "%s", "Please use root privileges.\n");
        return (EXIT_FAILURE);
    }
    if (argc < 2) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping -?' for more information.\n");
        exit(64);
    }
    memArgv = argv;
    //printf("argv: %s mem: %s\n", argv[1], memArgv[1]);
    searchFlags(memArgv);
    //*(argv + 1) = memArgv;
    //printf("argv: %s mem: %s\n", argv[1], memArgv[1]);
    pingStart(argc, argv);
    return (0);
}
