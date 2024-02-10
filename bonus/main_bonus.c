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
    man 7 ip
    man setsockopt
 */
static int openSocket(void) {
    if (!listAddr)
        exit(EXIT_FAILURE);
    struct timeval new;
    struct addrinfo *mem = listAddr;
    ssize_t    ttl = t_flags.ttl;
    int     fd = -1;

    new.tv_sec = 1;
    new.tv_usec = 0;
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
            if (t_flags.ttlActivate) {
                if (setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
                    dprintf(2, "%s", "Couldn't set option TTL socket.\n");
                    if (fdSocket >= 0)
                        close(fdSocket);
                    exit(EXIT_FAILURE);
                }
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

size_t	ftStrlenWiEqual(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0' && s[i] != '=')
	{
		i++;
	}
	return (i);
}

static int doubleFlag(char **argv, int i) {
    int nullable = FALSE;
    char    *memory = NULL;
    int len;
    int same[8];

    ft_memset(&same, 0, 8 * sizeof(int));
    len = ftStrlenWiEqual(argv[i]);
    if (len == 2 && !ft_strncmp(argv[i], "--", len)) {
        argv[i] = NULL;
        return (STOP);
    }
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
    if (len <= 9 && !ft_strncmp(argv[i], "--pattern", len))
        same[5] = TRUE;
    if (len <= 6 && !ft_strncmp(argv[i], "--help", len)) {
        same[6] = TRUE;
    }
    if (len <= 7 && !ft_strncmp(argv[i], "--usage", len)) {
        same[7] = TRUE;
    }
    if (!memory)
        memory = argv[i];
    int pos = similarFlags(same, memory);
    memory = NULL;
    switchFlags(argv, pos, i, len);
    ft_memset(&same, 0, 8 * sizeof(int));
    return (nullable);
}

static int singleFlag(char **argv, int i) {
    int nullable = FALSE;

    for (int j = 1; argv[i][j] != '\0'; j++) {
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
            case 'p':
                callParsePattern(argv, i);
                break ;
            default:
                dprintf(2, "%s%c%c\n", "ping: invalid option -- \'", argv[i][j], '\'');
                dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                exit(64);
        }
        if (!argv[i])
            break ;
    }
    return (nullable);
}

static void searchFlags(char *argv[], int argc) {
    int nullable = FALSE;

    for (int i = 1; i < argc; ++i) {
        if (argv[i]) {
            if (argv[i][0] == '-' && argv[i][1] != '-')
                nullable = singleFlag(argv, i);
            else if (argv[i][0] == '-' && argv[i][1] == '-'){
                nullable = doubleFlag(argv, i);
            }
            if (nullable == STOP){
                break ;
            }
            if (nullable) {
                argv[i] = NULL;
                nullable = FALSE;
            }
        }
    }
}

/*
    man getaddrinfo > man 5 services (0)
    man icmp
    man 7 raw
*/
static struct addrinfo *getIp(char *argv[], int argc, int *i) {
    struct addrinfo *list = NULL, client;
    int result = 0;

    for (; *i < argc; ++(*i)) {
        if (argv[*i]) {
            ft_memset(&client, 0, sizeof(struct addrinfo));
            client.ai_family = AF_INET;
            client.ai_socktype = SOCK_RAW;
            client.ai_protocol = IPPROTO_ICMP;
            client.ai_flags = AI_CANONNAME;
            client.ai_addr = NULL;
            client.ai_next = NULL;
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
    return (list);
}

static void    pingStart(int argc, char *argv[]) {
    size_t    preload = t_flags.preload;
    int quit = FALSE;
    int hostFound = FALSE;
    int     i = 1;

    //init part
    if (signal(SIGINT, sigHandlerInt) == SIG_ERR)
        exitInet();
    for (; i < argc; ++i) {
        listAddr = getIp(argv, argc, &i);
        if (listAddr && !hostFound)
            hostFound = TRUE;
        if (!hostFound) {
            dprintf(2, "%s",
            "ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
            if (listAddr)
                freeaddrinfo(listAddr);
            exit(64);
        }
        fdSocket = openSocket();
        //next part ping here
        runIcmp();
        signalEnd();
        if (!roundTripGlobal.packetReceive)
            quit = TRUE;
        ft_memset(pingMemory, 0, sizeof(pingMemory));
        ft_memset(&roundTripGlobal, 0, sizeof(struct s_round_trip));
        if (listAddr)
            freeaddrinfo(listAddr);
        listAddr = NULL;
        if (fdSocket >= 0)
            close(fdSocket);
        t_flags.preload = preload;
    }
    if (quit)
        exit(1);
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
    t_flags.ttl = 64;
    t_flags.tos = 0;
    t_flags.w = 0;
    t_flags.preload = 0;
    t_flags.ttlActivate = FALSE;
    for (int i = 0; i < 40; i++)
        t_flags.pattern[i] = i;
    if (getuid() != 0) {
        dprintf(2, "%s", "Please use root privileges.\n");
        return (EXIT_FAILURE);
    }
    if (argc < 2) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
    searchFlags(cpyArgv, argc);
    pingStart(argc, cpyArgv);
    return (0);
}
