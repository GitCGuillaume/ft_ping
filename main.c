#include "tools.h"
#include "flags.h"
#include "ft_icmp.h"

struct  addrinfo *listAddr = 0;
struct s_flags t_flags;
int fdSocket;

static void    sigHandlerInt(int sigNum) {
    if (sigNum != SIGINT)
        return ;
    if  (listAddr)
        freeaddrinfo(listAddr);
    exit(0);
}

/* code /usr/include/x86_64-linux-gnu/bits/in.h
    Options for use with `getsockopt' and `setsockopt' at the IP level.
    IP_TTL...
 */
static int openSocket(/*struct addrinfo *listAddr*/) {
    if (!listAddr)
        exit(EXIT_FAILURE);
    struct addrinfo *mem = listAddr;
    int    ttl = 255;
    int     fd = -1;

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

static void    searchFlags(char *argv[]/*, struct addrinfo *client*/) {
    for (int i = 1; argv[i] != NULL; ++i) {
        if (argv[i][0] == '-' && argv[i][1] == 'v')
            t_flags.v = TRUE;
        else if (argv[i][0] == '-' && argv[i][1] == '?')
            t_flags.interrogation = TRUE;
    }
}

static struct addrinfo *getIp(struct addrinfo *client, char *argv[], int *i) {
    struct addrinfo *list = 0;
    //struct sockaddr_in *translate;
    int result = 0;

    for (; argv[*i] != NULL; ++(*i)) {
        if (argv[*i][0] != '-') {
            result = getaddrinfo(argv[*i], NULL, client, &list);
            if (result != 0) {
                dprintf(2, "%s\n", gai_strerror(result));
                exit(result);
            }
            break ;
        }
    }
    /*char str[1000];
    ft_memset(str, 0, 1001);
    for (struct addrinfo *i = list; i != NULL; i = i->ai_next) {
        translate = (struct sockaddr_in *)i->ai_addr;
        printf("%x\n%s\n", translate->sin_addr.s_addr,
            inet_ntop(AF_INET, &translate->sin_addr, str, INET_ADDRSTRLEN));
        ft_memset(str, 0, 1001);
    }*/
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
    struct  addrinfo client;
    int     i = 1;

    //init part
    if (signal(SIGINT, sigHandlerInt) == SIG_ERR)
        exitInet();
    for (; i < argc; ++i) {
        ft_memset(&client, 0, sizeof(struct addrinfo));
        client.ai_family = AF_INET;
        client.ai_socktype = SOCK_RAW;
        client.ai_protocol = IPPROTO_ICMP;
        client.ai_flags = AI_CANONNAME;
        listAddr = getIp(&client, argv, &i);
        if (t_flags.interrogation == TRUE) {
            flagInterrogation();
            break ;
        }
        fdSocket = openSocket(/*listAddr*/);
        //next part ping here
        runIcmp(/*listAddr*/);
        freeaddrinfo(listAddr);
        if (fdSocket >= 0)
            close(fdSocket);
    }
}

//ping [OPTIONS] host
int main(int argc, char *argv[]) {
    t_flags.v = FALSE;
    t_flags.interrogation = FALSE;
    if (getuid() != 0) {
        dprintf(2, "%s", "Please use root privileges.\n");
        return (EXIT_FAILURE);
    }
    if (argc < 2) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping -?' for more information.\n");
        exit(64);
    }
    searchFlags(argv);
    pingStart(argc, argv);
    return (0);
}