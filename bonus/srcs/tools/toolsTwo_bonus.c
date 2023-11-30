#include "tools_bonus.h"
#include "flags_bonus.h"
#include "parsing_bonus.h"

void    switchFlags(char *argv[], int pos, int i, int len) {
    switch (pos) {
        case 0:
            t_flags.v = TRUE;
            break ;
        case 1:
            t_flags.w = bigCallParseArgument(argv, i, len, 2147483647);
            break ;
        case 2:
            t_flags.tos = bigCallParseArgument(argv, i, len, 255);
            break ;
        case 3:
            t_flags.ttl = bigCallParseArgument(argv, i, len, 255);
            break ;
        case 4:
            t_flags.preload = bigCallParsePreload(argv, i, len, 2147483647);
            break ;
        case 5:
            t_flags.interval = bigCallParseInterval(argv, i, len);
            break ;
        case 6:
            bigCallParsePattern(argv, i, len);
            break ;
        case 7:
            t_flags.interrogation = TRUE;
            flagInterrogation();
            exit(0);
            break ;
        case 8:
            flagUsage();
            exit(0);
            break ;
    }
}

int    similarFlags(int same[9], const char *memory) {
    //display ambiguous
    const char  *memFlag[9] = {
        "--verbose", "--timeout", "--tos", "--ttl",
        "--preload", "--interval", "--pattern",
        "--help", "--usage"
    };
    //if > 1 error
    int similar = 0;
    //save function to launch into pos
    int pos = 0;

    for (int i = 0; i < 9; i++) {
        if (same[i] == TRUE) {
            ++similar;
            pos = i;
        }
    }
    if (similar > 1) {
        printf("ping: option \'%s\' is ambiguous; possibilities:", memory);
        for (int i = 0; i < 9; i++) {
            if (same[i] == TRUE) {
                printf(" \'%s\'", memFlag[i]);
            }
        }
        printf("\nTry 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
    return (pos);
}