#include "libft.h"
#include <stdio.h>
#include <stdint.h>

static void requireArgument(const char *cmd, const char *original) {
    if (!original) {
        if (cmd[0] == '-' && cmd[1] == '-')
            dprintf(2, "ping: option \'%s\' requires an argument\n", cmd);
        else
            dprintf(2, "ping: option requires an argument -- \'%s\'\n", cmd);
        dprintf(2, "Try 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
}

static void    findFirstDigit(char **str, uint32_t *i) {
    while (str[0][*i] && str[0][*i] == ' ')
        ++(*i);
    if (ft_isdigit(str[0][*i])) {
        (*str) += *i;
    }
}

uint32_t    parseArgument(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    uint32_t    result = 0;
    uint32_t    i = 0;

    requireArgument(cmd, original);
    if (!str[0][i]) {
        dprintf(2, "ping: option value too small: %s\n", original);
        exit(1);
    }
    findFirstDigit(str, &i);
    if (!str[0][0]) {
        dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
            original, str[0]);
        exit(1);
    }
    for (; *str[0] != 0; (*str)++) {
        if (ft_isdigit(*str[0])) {
            result = result * 10 + *str[0] - '0';
        } else {
            dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                original, str[0]);
            exit(1);
        }
        if (result == 0) {
            dprintf(2, "ping: option value too small: %s\n", original);
            exit(1);
        } else if (result > maxValue) {
            dprintf(2, "ping: option value too big: %s\n", original);
            exit(1);
        }
    }
    return (result);
}

/* convert integer and decimal parts into a result, and test it's value */
static double convertIntoDouble(uint64_t integerPart, uint64_t decimalPart,
    size_t dividend, const char *original) {
    double   result = 0.0f;
    size_t  convert = 0;

    result = (double)integerPart + ((double)decimalPart / dividend);
    convert = result * 1000000;
    if (convert < 200000) {
        dprintf(2, "ping: option value too small: %s\n", original);
        exit(1);
    }
    return (result);
}

double    parseArgumentI(const char *cmd,
    const char *original, char **str) {
    uint32_t    i = 0;
    uint64_t    integerPart = 0;
    uint64_t    decimalPart = 0;
    size_t    dividend = 1;

    requireArgument(cmd, original);
    findFirstDigit(str, &i);
    if (!str[0][0]) {
        return (0.0f);
    }
    if (str[0][i] == ',') {
        (*str) += i;
    }
    if (*str[0] && *str[0] != ',') {
        //get integer part into it's variable
        while (*str[0] && *str[0] != ',') {
            if (ft_isdigit(*str[0])) {
                integerPart = integerPart * 10 + *str[0] - '0';
            } else {
                dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                    original, str[0]);
                dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
                exit(1);
            }
            (*str)++;
        }
    }
    //get decimal part into it's variable
    while (*str[0]) {
	    if (*str[0] == ',')
            (*str)++;
        if (!*str[0])
            break ;
        if (ft_isdigit(*str[0])) {
            dividend *= 10;
            decimalPart = decimalPart * 10 + *str[0] - '0';
        } else {
            dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                original, str[0]);
            dprintf(2, "Try \'ping --help\' or \'ping --usage\' for more information.\n");
            exit(1);
        }
        (*str)++;
    }
    return (convertIntoDouble(integerPart, decimalPart, dividend, original));
}

uint32_t    parsePreload(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    uint32_t    result = 0;
    uint32_t    i = 0;

    requireArgument(cmd, original);
    findFirstDigit(str, &i);
    for (; *str[0] != 0; (*str)++) {
        if (ft_isdigit(*str[0])) {
            result = result * 10 + *str[0] - '0';
        } else {
            dprintf(2, "ping: invalid preload value (%s)\n", original);
            exit(1);
        }
        if (result > maxValue) {
            dprintf(2, "ping: invalid preload value (%s)\n", original);
            exit(1);
        }
    }
    return (result);
}