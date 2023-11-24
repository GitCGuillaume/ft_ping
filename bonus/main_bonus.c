#include "tools_bonus.h"
#include "flags_bonus.h"
#include "ft_icmp_bonus.h"
#include "parsing_bonus.h"

struct s_ping_memory    pingMemory[65536];
struct  addrinfo *listAddr = 0;//need to be cleaned in CTRL+C + alarm(function is sigHandlerAlrm)
struct s_round_trip  roundTripGlobal;//mainly for signal function...
struct s_flags t_flags;
int fdSocket;//must be also closed on CTRL+C etc
//https://www.gnu.org/software/libc/manual/html_node/Atomic-Types.html
volatile sig_atomic_t   end = FALSE;

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

static char    findArgumentEq(char **str, int i, int j) {
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
            else if(!ft_strncmp(argv[i], "--timeout", 9)
                && findArgumentEq(argv, i, 9)
            ) {
                t_flags.w = bigCallParseArgument(argv, i, 9, 2147483647);
            }
            else if(!ft_strncmp(argv[i], "--tos", 5)
                && findArgumentEq(argv, i, 5)
            ) {
                t_flags.tos = bigCallParseArgument(argv, i, 5, 255);
            } else if(!ft_strncmp(argv[i], "--ttl", 5)
                && findArgumentEq(argv, i, 5)
            ) {
                t_flags.ttl = bigCallParseArgument(argv, i, 5, 255);
            } else if (!ft_strncmp(argv[i], "--preload", 9)
                && findArgumentEq(argv, i, 9)) {
                t_flags.preload = bigCallParsePreload(argv, i, 2147483647);
            } else if (!ft_strncmp(argv[i], "--interval", 10)
                && findArgumentEq(argv, i, 10)) {
                t_flags.interval = bigCallParseInterval(argv, i);
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
                switch (argv[i][j]) {
                    case 'v':
                        t_flags.v = TRUE;
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
                    default:
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
        printf("continue?\n");
        freeaddrinfo(listAddr);
        if (fdSocket >= 0)
            close(fdSocket);
    }
}

//ping [OPTIONS] host
int main(int argc, char *argv[]) {
    char    *cpyArgv[argc + 1];

    //copy argv since I will manipulate argv values
    for (int i = 0; i < argc; i++) {
        cpyArgv[i] = argv[i];
    }
    cpyArgv[argc] = NULL;
    ft_memset(pingMemory, 0, sizeof(pingMemory));
    ft_memset(&roundTripGlobal, 0, sizeof(struct s_round_trip));
    //init flags
    t_flags.v = FALSE;
    t_flags.interrogation = FALSE;
    t_flags.ttl = 60;
    t_flags.tos = 0;
    t_flags.w = 0;
    t_flags.preload = 0;
    t_flags.interval = 1.0f;
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
