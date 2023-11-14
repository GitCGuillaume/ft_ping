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
    if (!original) {
        if (cmd[0] == '-' && cmd[1] == '-')
            dprintf(2, "ping: option \'%s\' requires an argument\n", cmd);
        else
            dprintf(2, "ping: option requires an argument -- \'%s\'\n", cmd);
        dprintf(2, "Try 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
}

uint32_t    parseArgument(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    uint32_t    result = 0;
    uint32_t    i = 0;

    requireArgument(cmd, original);
    if (!str[0][i]) {
        dprintf(2, "ping: option value too small: %s\n", original);
        exit(1);
    }
    while (str[0][i] && str[0][i] == ' ')
        ++i;
    if (ft_isdigit(str[0][i])) {
        (*str) += i;
    }
    i = 0;
    if (!str[0][i]) {
        dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
            original, str[0]);
        exit(1);
    }
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

float    parseArgumentI(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    float   result = 0.0f;
    uint32_t    i = 0;

    requireArgument(cmd, original);
    while (str[0][i] && str[0][i] == ' ')
        ++i;
    if (ft_isdigit(str[0][i])) {
        (*str) += i;
    }
    i = 0;
    if (!str[0][i]) {
        return (0.0f);
    }
    //for (; *str[0] != 0; (*str)++) {
    while (*str[0] && *str[0] != ',') {
        if (ft_isdigit(*str[0])) {
            result = result * 10 + *str[0] - '0';
        } else {
            dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                original, str[0]);
            dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
            exit(1);
        }
        printf("res: %f\n", result);
        if (result > maxValue) {
            dprintf(2, "ping: option value too big: %s\n", original);
            dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
            exit(1);
        }
        (*str)++;
    }
    while (*str[0]) {
        if (*str[0] == ',')
            (*str)++;
        if (!*str[0])
            break ;
        if (ft_isdigit(*str[0])) {
            t_flags.dividend *= 10;
            result = result + (float)(*str[0] - '0') / t_flags.dividend;
        } else {
            dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                original, str[0]);
            dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
            exit(1);
        }
        //printf("res: %f\n", result);
        /*if (result > maxValue) {
            dprintf(2, "ping: option value too big: %s\n", original);
            dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
            exit(1);
        }*/
        (*str)++;
    }
    printf("rr:%f\n", result);
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
    if (ft_isdigit(str[0][i])) {
        (*str) += i;
    }
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
    return (result);
}

char    findArgument(char **str, int i) {
    if (str[i][0] && str[i][1] && str[i][2]) {
        str[i] += 2;
        return (TRUE);
    }
    return (FALSE);
}

char    findArgumentEq(char **str, int i, int j) {
    if (!str[i][j]) {
        return (TRUE);
    }
    if (str[i][j] == '=') {
        str[i] += j + 1;
        return (TRUE);
    }
    return (FALSE);
}

static void     searchBigOption(char *argv[], int argc) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i] && argv[i][0] == '-' && argv[i][1] == '-') {
            if (!ft_strncmp(argv[i], "--verbose", 9))
                    t_flags.v = TRUE;
            else if(!ft_strncmp(argv[i], "--tos", 5)
                && findArgumentEq(argv, i, 5)
            ) {
                if (!argv[i][5]) {
                    t_flags.tos = parseArgument(argv[i], argv[i + 1], &argv[i + 1], 255);
                    argv[i + 1] = NULL;
                }
                else {
                    t_flags.tos = parseArgument(argv[i], argv[i], &argv[i], 255);
                    argv[i] = NULL;
                }
                //break ;
            } else if(!ft_strncmp(argv[i], "--ttl", 5)
                && findArgumentEq(argv, i, 5)
            ) {
                if (!argv[i][5]) {
                    t_flags.ttl = parseArgument(argv[i], argv[i + 1], &argv[i + 1], 255);
                    argv[i + 1] = NULL;
                }
                else {
                    t_flags.ttl = parseArgument(argv[i], argv[i], &argv[i], 255);
                    argv[i] = NULL;
                }
                //break ;
            } else if ((!ft_strncmp(argv[i], "--preload", 9)
                && findArgumentEq(argv, i, 9))) {
                if (!argv[i][9]) {
                    t_flags.preload = parsePreload(argv[i], argv[i + 1], &argv[i + 1], 2147483647);
                    argv[i + 1] = NULL;
                }
                else {
                    t_flags.preload = parsePreload(argv[i], argv[i], &argv[i], 2147483647);
                    argv[i] = NULL;
                }
                //break ;
            } else if (!ft_strncmp(argv[i], "--help", 6)) {
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
    int j;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] && argv[i][0] == '-' && argv[i][1] != '-') {
            for (j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'v')
                    t_flags.v = TRUE;
                else if (argv[i][j] == '?') {
                    t_flags.interrogation = TRUE;
                    flagInterrogation();
                    exit(0);
                } else if (argv[i][j] == 'w') {
                    if (findArgument(argv, i) == TRUE) {
                        t_flags.w = parseArgument("w", argv[i], &argv[i], 2147483647);
                        argv[i] = NULL;
                    }
                    else {
                        t_flags.w = parseArgument("w", argv[i + 1], &argv[i + 1], 2147483647);
                        argv[i + 1] = NULL;
                    }
                    break ;
                } else if (argv[i][j] == 'T') {
                    if (findArgument(argv, i) == TRUE) {
                        t_flags.tos = parseArgument("T", argv[i], &argv[i], 2147483647);
                        argv[i] = NULL;
                    }
                    else {
                        t_flags.tos = parseArgument("T", argv[i + 1], &argv[i + 1], 2147483647);
                        argv[i + 1] = NULL;
                    }
                    break ;
                } else if (argv[i][j] == 'l') {
                    if (findArgument(argv, i) == TRUE) {
                        t_flags.preload = parsePreload(argv[i], argv[i], &argv[i], 2147483647);
                        argv[i] = NULL;
                    }
                    else {
                        t_flags.preload = parsePreload(argv[i], argv[i + 1], &argv[i + 1], 2147483647);
                        argv[i + 1] = NULL;
                    }
                    break ;
                } else if (argv[i][j] == 'i') {
                    if (findArgument(argv, i) == TRUE) {
                        t_flags.interval = parseArgumentI(argv[i], argv[i], &argv[i], 2147483647);
                        argv[i] = NULL;
                    }
                    else {
                        t_flags.interval = parseArgumentI(argv[i], argv[i + 1], &argv[i + 1], 2147483647);
                        argv[i + 1] = NULL;
                    }
                    break ;
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
            "ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
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
    ft_memset(pingMemory, 0, sizeof(pingMemory));
    ft_memset(&roundTripGlobal, 0, sizeof(struct s_round_trip));
    //init flags
    t_flags.v = FALSE;
    t_flags.interrogation = FALSE;
    t_flags.ttl = 60;
    t_flags.tos = 0;
    t_flags.w = 0;
    t_flags.preload = 0;
    t_flags.dividend = 1;
    t_flags.interval = -1.0f;
    if (getuid() != 0) {
        dprintf(2, "%s", "Please use root privileges.\n");
        return (EXIT_FAILURE);
    }
    if (argc < 2) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
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
