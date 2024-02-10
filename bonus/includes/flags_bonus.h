#ifndef FLAGS_BONUS_H
# define FLAGS_BONUS_H

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

#include <stdint.h>
#include <stddef.h>

struct s_flags {
    char    v;
    char    interrogation;
    uint32_t    tos;
    uint32_t    ttl;
    uint32_t    w;
    size_t    preload;
    char    pattern[40];
    char    ttlActivate;
};

void    flagInterrogation(void);
void    flagUsage(void);

extern struct s_flags t_flags;

#endif