#ifndef TOOLS_H
# define TOOLS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TRUE 1
#define FALSE 0

struct s_flags {
    char v;
    char interrogation;
};

void	*ft_memset(void *b, int c, size_t len);
size_t ft_strlen(const char *str);

#endif