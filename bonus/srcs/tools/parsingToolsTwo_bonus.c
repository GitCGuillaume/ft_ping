#include "libft.h"
#include <stdio.h>
#include <stdint.h>
#include "flags_bonus.h"

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
    int neg = FALSE;

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
        if (str[0][0] == '-' && str[0][1] != '-') {
            (*str)++;
            neg = TRUE;
        }
        if (ft_isdigit(*str[0])) {
            result = result * 10 + *str[0] - '0';
            if (neg)
                result *= -1;
        } else {
            dprintf(2, "ping: invalid value (`%s\' near `%s\')\n",
                original, str[0]);
            exit(1);
        }
        neg = FALSE;
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

size_t    parsePreload(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue) {
    size_t    result = 0;
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

int8_t    patternResolver(char **str, uint8_t *minus) {
    char c;

    if (str[0][0] == '-' && str[0][1] != '-') {
        c = ft_tolower(str[0][1]) - '0';
        if (c > 9) {
            c -= 39;
        }
        c = c * -1;
        if (c >= 16 || !(c >= -15 && c < 0)) {
            dprintf(2, "ping: error in pattern near %s\n", str[0]);
            exit(1);
        }
        ++*str;
        *minus = TRUE;
    } else {
        c = ft_tolower(*str[0]) - '0';

        if (c > 9) {
            c -= 39;
        }
        if (c >= 16 || c < 0) {
            dprintf(2, "ping: error in pattern near %s\n", str[0]);
            exit(1);
        }
    }
    return (c);
}

void    parsePattern(const char *cmd,
    const char *original, char **str) {
    uint32_t    i = 0;
    int8_t bitOne = 0;
    uint8_t minus = FALSE;
    char c;

    requireArgument(cmd, original);
    findFirstDigit(str, &i);
    i = 0;
    for (; *str[0] != 0; (*str)++) {
        c = patternResolver(str, &minus);
        if (minus == TRUE) {
            t_flags.pattern[i++] = c;
            minus = FALSE;
            bitOne = 0;
        } else {
            if (!bitOne) {
                bitOne = c;
                t_flags.pattern[i] = c;
            }
            else {
                t_flags.pattern[i] = (bitOne * 16) + c;
                bitOne = 0;
                ++i;
            }
        }
    }
    if (bitOne)
        ++i;
    //to fill array and must be used as mod
    if (!i)
        i = 1;
    for (int j = i; j < 40; j++) {
        t_flags.pattern[j] = t_flags.pattern[j % i];
    }
}