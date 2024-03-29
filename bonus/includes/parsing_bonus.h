#ifndef PARSING_BONUS_H
# define PARSING_BONUS_H

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

uint32_t    bigCallParseArgument(const char *cmd, char *argv[],
    int i, int j, uint32_t maxValue);
ssize_t    bigCallParsePreload(const char *cmd, char *argv[],
    int i, int j, uint32_t maxValue);
void    bigCallParsePattern(const char *cmd, char *argv[], int i, int j);
uint32_t    callParseArgument(char *argv[], int i, uint32_t maxValue);
ssize_t    callParsePreload(char *argv[], int i, uint32_t maxValue);
void    callParsePattern(char *argv[], int i);

uint32_t    parseArgument(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue);
size_t    parsePreload(const char *cmd,
    const char *original, char **str,
    uint32_t maxValue);
void    parsePattern(const char *cmd,
    const char *original, char **str);

#endif