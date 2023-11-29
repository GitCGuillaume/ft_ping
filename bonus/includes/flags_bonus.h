#ifndef FLAGS_BONUS_H
# define FLAGS_BONUS_H

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

struct s_flags {
    char    v;
    char    interrogation;
    uint32_t    tos;
    uint32_t    ttl;
    uint32_t    w;
    ssize_t    preload;
    double    interval;
    char    pattern[40];
};

void    flagInterrogation(void);
void    flagUsage(void);

extern struct s_flags t_flags;

#endif