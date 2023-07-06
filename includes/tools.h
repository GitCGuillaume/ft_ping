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
#include <signal.h>

#define TRUE 1
#define FALSE 0

struct s_flags {
    char v;
    char interrogation;
};

void	*ft_memset(void *b, int c, size_t len);
void	*ft_memcpy(void *dst, const void *src, size_t n);
size_t  ft_strlen(const char *str);

extern struct s_flags t_flags;
extern int fdSocket;

#endif