#include "tools.h"

struct s_flags t_flags;
/*
    man 7 icmp
*/

int openSocket(void) {
    int fdSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (fdSocket < 0) {
        dprintf(2, "%s", "Couldn't open socket.\n");
        exit(EXIT_FAILURE);
    }
    return (fdSocket);
}

void    searchFlags(char *argv[], struct sockaddr_in *client) {
    int result = 0;
    char test[1000];
    ft_memset(test, 0, 1001);
    for (int i = 1; argv[i] != NULL; ++i) {
        printf("val: %s\n", argv[i]);
        if (argv[i][0] == '-' && argv[i][1] == 'v')
            t_flags.v = TRUE;
        else if (argv[i][0] == '-' && argv[i][1] == '?')
            t_flags.interrogation = TRUE;
        else {
            result = inet_pton(AF_INET, argv[i], &client->sin_addr);
            if (result == 1) {
                printf("break\n");
                break ;
            } else if (result < 0) {
                exit(EXIT_FAILURE);
            } else {
                getnameinfo((struct sockaddr *)client,
                    sizeof(struct sockaddr_in *),
                    argv[i], sizeof(argv[i]),
                    test, sizeof(test), NI_NUMERICHOST | NI_NUMERICSERV);
                printf("test: %s\n", test);
                ft_memset(test, 0, 1001);
            }
        }
    }
    printf("%d %d\n", t_flags.v, t_flags.interrogation);
    char test2[1000];
    ft_memset(test2, 0, 1000);
    printf("%x\n%s\n", client->sin_addr.s_addr, inet_ntop(AF_INET, &client->sin_addr, test2, INET_ADDRSTRLEN));
    if (!client->sin_addr.s_addr) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping -?' for more information.\n");
        exit(64);
    }
}

//ping [OPTIONS] host
int main(int argc, char *argv[]) {
    struct  sockaddr_in client;
    int     fdSocket = 0;

    t_flags.v = FALSE;
    t_flags.interrogation = FALSE;
    if (getuid() != 0) {
        dprintf(2, "%s", "Please use root privileges.\n");
        return (EXIT_FAILURE);
    }
    if (argc < 1) {
        dprintf(2, "%s", "ping: missing host operand\nTry 'ping -?' for more information.\n");
        exit(64);
    }
    ft_memset(&client, 0, sizeof(struct sockaddr_in));
    searchFlags(argv, &client);
    fdSocket = openSocket();
    if (fdSocket >= 0)
        close(fdSocket);
    return (0);
}