#include "flags.h"
#include "icmpCodeTools.h"
#include "tools.h"

static void    destUnreach(uint8_t code) {
    const char *arr[16] = {
        "Destination Net Unreachable", "Destination Host Unreachable", "Destination Protocol Unreachable",
        "Destination Port Unreachable", "Fragmentation needed and DF set", "Source Route Failed",
        "Network Unknown", "Host Unknown", "Host Isolated",
        NULL, NULL,
        "Destination Network Unreachable At This TOS",
        "Destination Host Unreachable At This TOS",
        "Packet Filtered", "Precedence Violation", "Precedence Cutoff"
    };
    int i;

    for (i = 0; i < 16; ++i) {
        if (i == code)
            break ;
    }
    if (i != 16 && i != 9 && i != 10)
        printf("%s", arr[i]);
    else
        printf("%s: %hhu", "Dest Unreachable, Unknown Code", code);
}

static void    redirect(uint8_t code) {
    if (code == 0)
        printf("%s", "Redirect Network");
    else if (code == 1)
        printf("%s", "Redirect Host");
    else if (code == 2)
        printf("%s", "Redirect Type of Service and Network");
    else if (code == 3)
        printf("%s", "Redirect Type of Service and Host");
    else
        printf("%s: %hhu", "Redirect, Unknown Code", code);
}

static void    timeExceed(uint8_t code) {
    const char  *arr[2] = {
        "Time to live exceeded",
        "Frag reassembly time exceeded"
    };
    int i;

    for (i = 0; i < 2; ++i) {
        if (i == code)
            break ;
    }
    if (i == 0 || i == 1)
        printf("%s", arr[i]);
    else
        printf("%s: %hhu", "Time exceeded, Unknown Code", code);
}

/* Source Quench is deprecated */
static void    paramProbOrSQ(struct icmphdr *icmp,
    struct iphdr *ipOriginal, struct icmphdr *icmpOriginal, int isSource) {
    if (!isSource) {
        char str[INET_ADDRSTRLEN];
        uint32_t addr = icmp->un.gateway;
        ft_memset(&str, 0, INET_ADDRSTRLEN);
        const char *inetResult = inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);

        printf("Parameter problem: IP address = %s", inetResult);
    } else {
        printf("%s", "Source Quench");
    }
    headerDumpIp(ipOriginal, FALSE);
    headerDumpData(icmpOriginal,
        ntohs(ipOriginal->tot_len) - sizeof(struct iphdr));
    printf("\n");
}

void    runCode(struct icmphdr *icmp, struct iphdr *ipOriginal,
    struct icmphdr *icmpOriginal) {
    unsigned int types[19] = {
        NONE, NONE, NONE,
        ICMP_DEST_UNREACH, NONE,
        ICMP_REDIRECT, NONE,NONE, NONE,
        NONE, NONE, ICMP_TIME_EXCEEDED,
        NONE, NONE, NONE,
        NONE, NONE, NONE,
        NONE
    };
    //list of function to call from macro number
    void    *functionArray[19] = {
        NULL, NULL, NULL,
        &destUnreach, NULL, &redirect,
        NULL, NULL, NULL, NULL, NULL,
        &timeExceed, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    };
    void    (*functionCall)(uint8_t) = NULL;
    unsigned int i;

    for (i = 0; i < 19; ++i) {
        if (icmp->type == types[i]
            && types[i] != NONE) {
            functionCall = functionArray[i];
            break ;
        }
    }
    if (icmp->type == ICMP_PARAMETERPROB) {
        paramProbOrSQ(icmp, ipOriginal, icmpOriginal, FALSE);
    } else if (icmp->type == ICMP_SOURCE_QUENCH)
        paramProbOrSQ(icmp, ipOriginal, icmpOriginal, TRUE);
    else if (functionCall) {
        functionCall(icmp->code);
        if (t_flags.v == TRUE) {
            headerDumpIp(ipOriginal, TRUE);
            headerDumpData(icmpOriginal,
                ntohs(ipOriginal->tot_len) - sizeof(struct iphdr));
        }
        printf("\n");
    }
}