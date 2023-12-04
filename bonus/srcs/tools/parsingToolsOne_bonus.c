#include "libft.h"
#include <stdio.h>
#include <stdint.h>
#include "parsing_bonus.h"

static void    findArgumentEq(char **str, int i, int j) {
    if (str[i][j] == '=') {
        str[i] += j + 1;
    }
}

static char    findArgument(char **str, int i) {
    if (str[i][0] && str[i][1] && str[i][2]) {
        str[i] += 2;
        return (TRUE);
    }
    return (FALSE);
}

/* searchBigOption part start*/
uint32_t    bigCallParseArgument(char *argv[], int i, int j, uint32_t maxValue) {
    uint32_t value = 0;

    findArgumentEq(argv, i, j);
    if (!argv[i][j]) {
        value = parseArgument(argv[i], argv[i + 1], &argv[i + 1], maxValue);
        argv[i] = NULL;
        argv[i + 1] = NULL;
    }
    else {
        value = parseArgument(argv[i], argv[i], &argv[i], maxValue);
        argv[i] = NULL;
    }
    return (value);
}

ssize_t    bigCallParsePreload(char *argv[], int i, int j, uint32_t maxValue) {
    ssize_t value = 0;

    findArgumentEq(argv, i, j);
    if (!argv[i][j]) {
        value = parsePreload(argv[i], argv[i + 1], &argv[i + 1], maxValue);
        argv[i] = NULL;
        argv[i + 1] = NULL;
    }
    else {
        value = parsePreload(argv[i], argv[i], &argv[i], maxValue);
        argv[i] = NULL;
    }
    return (value);
}

void    bigCallParsePattern(char *argv[], int i, int j) {
    findArgumentEq(argv, i, j);
    if (!argv[i][j]) {
        parsePattern(argv[i], argv[i + 1], &argv[i + 1]);
        argv[i] = NULL;
        argv[i + 1] = NULL;
    }
    else {
        parsePattern(argv[i], argv[i], &argv[i]);
        argv[i] = NULL;
    }
}

/* searchBigOption end */

/* simple search option part start */
uint32_t    callParseArgument(char *argv[], int i, uint32_t maxValue) {
    uint32_t value = 0;

    if (findArgument(argv, i) == TRUE) {
        value = parseArgument(argv[i], argv[i], &argv[i], maxValue);
        argv[i] = NULL;
    }
    else {
        value = parseArgument(argv[i], argv[i + 1], &argv[i + 1], maxValue);
        argv[i] = NULL;
        argv[i + 1] = NULL;
    }
    return (value);
}

ssize_t    callParsePreload(char *argv[], int i, uint32_t maxValue) {
    ssize_t value = 0;

    if (findArgument(argv, i) == TRUE) {
        value = parsePreload(argv[i], argv[i], &argv[i], maxValue);
        argv[i] = NULL;
    }
    else {
        value = parsePreload(argv[i], argv[i + 1], &argv[i + 1], maxValue);
        argv[i] = NULL;
        argv[i + 1] = NULL;
    }
    return (value);
}

void    callParsePattern(char *argv[], int i) {
    if (findArgument(argv, i) == TRUE) {
        parsePattern(argv[i], argv[i], &argv[i]);
        argv[i] = NULL;
    }
    else {
        parsePattern(argv[i], argv[i + 1], &argv[i + 1]);
        argv[i] = NULL;
        argv[i + 1] = NULL;
    }
}

/* simple search option part end */