#ifndef FLAGS_H
# define FLAGS_H

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

void    flagInterrogation(void);
void    flagUsage(void);

struct s_flags {
    char v;
    char interrogation;
    char ttlActivate;
};

extern struct s_flags t_flags;

#endif