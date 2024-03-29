#include "tools.h"

/*
    Heron's method
    https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Heron's_method
*/
static double   ftSqrt(double num) {
    if (num < 0.0)
        num *= -1.0;
    //x is whatever value near square root
    double x = num;
    double old = 0.0;
    double  eqZ = num;

    if (num < 0.0 || num == 0.0)
        return (0.0);
    while (x != old) {
        old = x;
        x = (x + num / x) * 0.5;
        eqZ = (x * x) - num;
        if (eqZ < 0.0)
            eqZ *= -1.0;
	if (eqZ < EPSILON)
		break ;
    }
    return (x);
}

void    sigHandlerInt(int sigNum) {
    if (sigNum != SIGINT)
        return ;
    end = TRUE;
}

void    exitInet(void) {
    if  (listAddr) {
        freeaddrinfo(listAddr);
    }
    listAddr = NULL;
    if (fdSocket >= 0)
        close(fdSocket);
    fdSocket = -1;
    exit(1);
}

/*
    https://en.wikipedia.org/wiki/Standard_deviation
    https://fr.wikipedia.org/wiki/Variance_(mathématiques)
    Called on SIGINT signal
*/
void    signalEnd(void) {
    double  average;
    double  stdDev = 0.0;

    alarm(0);
    if (!listAddr) {
        if (fdSocket != -1) {
            close(fdSocket);
            fdSocket = -1;
        }
        exit(1);
    }
    if (fdSocket >= 0) {
        close(fdSocket);
        fdSocket = -1;
    }
    if (roundTripGlobal.packetReceive != 0) {
        average = roundTripGlobal.sum / roundTripGlobal.number;
        stdDev = ftSqrt((roundTripGlobal.squareSum / roundTripGlobal.number) - (average * average));
    }
    printf("--- %s ping statistics ---\n", listAddr->ai_canonname);
    printf("%lu packets transmitted, %lu packets received",
        roundTripGlobal.packetSend,
        roundTripGlobal.packetReceive);
    if (roundTripGlobal.packetDuplicate != 0)
        printf(", +%lu duplicates", roundTripGlobal.packetDuplicate);
    if (roundTripGlobal.packetReceive > roundTripGlobal.packetSend)
        printf(", -- somebody is printing forged packets!\n");
    else if (roundTripGlobal.packetSend != 0) {
        double loseRatePct = (((double)roundTripGlobal.packetSend -
            (double)roundTripGlobal.packetReceive) / (double)roundTripGlobal.packetSend)
            * 100.0;
        printf(", %d%% packet loss\n", (int)loseRatePct);
    }
    if  (listAddr)
        freeaddrinfo(listAddr);
    listAddr = NULL;
    if (roundTripGlobal.packetReceive != 0) {
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
            roundTripGlobal.rtt[0],
            average,
            roundTripGlobal.rtt[1],
            stdDev);
    }
}

/*
    max 16bits (65535 bits) to return
*/
uint16_t    checksum(uint16_t *hdr, size_t len) {
    size_t  sum = 0;
    uint8_t  minus = sizeof(uint16_t);

    //1 < because unsigned len
    while (1 < len) {
        sum += *hdr++;
        len -= minus;
    }
    //rfc say last bit if odd, then zero bytes
    if (len != 0)
        sum += *(unsigned char *)hdr & 0xFF;
    //if overflow > 16bits, cut in two and do a sum
    //remove 16 bits first part and take whatever value
    while (sum >> 16) {
        sum = (sum >> 16) + (sum & 65535);
    }
    return (~sum);
}

static void    AskIntt(char *ask) {
    ft_memcpy(ask, "Usage: ping [OPTION...] HOST ...\n", 33);
    ask += 33;
    ft_memcpy(ask, "Send ICMP ECHO_REQUEST packets to network hosts.\n\n", 50);
    ask += 50;
    ft_memcpy(ask, " Options controlling ICMP request types:\n", 41);
    ask += 41;
    ft_memcpy(ask,
        "      --address              send ICMP_ADDRESS packets (root only)\n", 67);
    ask += 67;
    ft_memcpy(ask, "      --echo                 send ICMP_ECHO packets (default)\n", 62);
    ask += 62;
    ft_memcpy(ask, "      --mask                 same as --address\n", 47);
    ask += 47;
    ft_memcpy(ask, "      --timestamp            send ICMP_TIMESTAMP packets\n", 57);
    ask += 57;
    ft_memcpy(ask, "  -t, --type=TYPE            send TYPE packets\n\n", 48);
    ask += 48;
    ft_memcpy(ask, " Options valid for all request types:\n\n", 39);
    ask += 39;
    ft_memcpy(ask, "  -c, --count=NUMBER         stop after sending NUMBER packets\n", 63);
    ask += 63;
    ft_memcpy(ask, "  -d, --debug                set the SO_DEBUG option\n", 53);
    ask += 53;
    ft_memcpy(ask, "  -i, --interval=NUMBER      wait NUMBER seconds between sending each packet\n", 77);
    ask += 77;
    ft_memcpy(ask, "  -n, --numeric              do not resolve host addresses\n", 59);
    ask += 59;
    ft_memcpy(ask,
        "  -r, --ignore-routing       send directly to a host on an attached network\n", 76);
    ask += 76;
    ft_memcpy(ask, "      --ttl=N                specify N as time-to-live\n", 55);
    ask += 55;
    ft_memcpy(ask, "  -T, --tos=NUM              set type of service (TOS) to NUM\n", 62);
    ask += 62;
    ft_memcpy(ask, "  -v, --verbose              verbose output\n", 44);
    ask += 44;
    ft_memcpy(ask, "  -w, --timeout=N            stop after N seconds\n", 50);
    ask += 50;
    ft_memcpy(ask, "  -W, --linger=N             number of seconds to wait for response\n\n", 69);
    ask += 69;
    ft_memcpy(ask, " Options valid for --echo requests:\n\n", 37);
    ask += 37;
    ft_memcpy(ask, "  -f, --flood                flood ping (root only)\n", 52);
    ask += 52;
    ft_memcpy(ask, "      --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of\n", 72);
    ask += 72;
    ft_memcpy(ask, "                             \"tsonly\" and \"tsaddr\"\n", 51);
    ask += 51;
    ft_memcpy(ask, "  -l, --preload=NUMBER       send NUMBER packets as fast as possible before\n", 76);
    ask += 76;
    ft_memcpy(ask, "                             falling into normal mode of behavior (root only)\n", 78);
    ask += 78;
    ft_memcpy(ask, "  -p, --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n", 71);
    ask += 71;
    ft_memcpy(ask, "  -q, --quiet                quiet output\n", 42);
    ask += 42;
    ft_memcpy(ask, "  -R, --route                record route\n", 42);
    ask += 42;
    ft_memcpy(ask, "  -s, --size=NUMBER          send NUMBER data octets\n\n", 54);
    ask += 54;
    ft_memcpy(ask, "  -?, --help                 give this help list\n", 49);
    ask += 49;
    ft_memcpy(ask, "      --usage                give a short usage message\n", 56);
    ask += 56;
    ft_memcpy(ask, "  -V, --version              print program version\n\n", 52);
    ask += 52;
    ft_memcpy(ask, "Mandatory or optional arguments to long options are also mandatory or optional\n", 79);
    ask += 79;
    ft_memcpy(ask, "for any corresponding short options.\n\n", 38);
    ask += 38;
    ft_memcpy(ask, "Options marked with (root only) are available only to superuser.\n\n", 66);
    ask += 66;
    ft_memcpy(ask, "Report bugs to <bug-inetutils@gnu.org>.\n", 40);
}

static void    AskUsage(char *ask) {
    ft_memcpy(ask, "Usage: ping [-dnrvfqR?V] [-t TYPE] [-c NUMBER] [-i NUMBER] [-T NUM] [-w N]\n", 75);
    ask += 75;
    ft_memcpy(ask, "            [-W N] [-l NUMBER] [-p PATTERN] [-s NUMBER] [--address] [--echo]\n", 77);
    ask += 77;
    ft_memcpy(ask, "            [--mask] [--timestamp] [--type=TYPE] [--count=NUMBER] [--debug]\n", 76);
    ask += 76;
    ft_memcpy(ask,
        "            [--interval=NUMBER] [--numeric] [--ignore-routing] [--ttl=N]\n", 73);
    ask += 73;
    ft_memcpy(ask, "            [--tos=NUM] [--verbose] [--timeout=N] [--linger=N] [--flood]\n", 73);
    ask += 73;
    ft_memcpy(ask, "            [--ip-timestamp=FLAG] [--preload=NUMBER] [--pattern=PATTERN]\n", 73);
    ask += 73;
    ft_memcpy(ask, "            [--quiet] [--route] [--size=NUMBER] [--help] [--usage] [--version]\n", 79);
    ask += 79;
    ft_memcpy(ask, "            HOST ...\n", 21);
}

void    flagInterrogation(void) {
    char    ask[2008];

    ft_memset(ask, 0, 2009);
    AskIntt(ask);
    printf("%s", ask);
}

void    flagUsage(void) {
    char    ask[548];

    ft_memset(ask, 0, 548);
    AskUsage(ask);
    printf("%s", ask);
}
