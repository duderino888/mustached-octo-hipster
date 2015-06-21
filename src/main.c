#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "log.h"
#include "irc.h"


#define SERVER_HOST         "chat.freenode.net"
#define SERVER_PORT         6667
#define BOT_NICK            "mustached-octo-hipster"
#define BOT_NAME            "Simple Bot"




#define streq(A,B)      (strcmp((A),(B)) == 0)


static FILE *fsockopen(const char *, int);
static char *strtrim(char *);
static void parse_irc_msg(char *, char **, char **, char **);


int
main(void)
{
    FILE *sock = fsockopen(SERVER_HOST, SERVER_PORT);
    if (!sock) {
        log_error("Failed to connect");
        exit(EXIT_FAILURE);
    }

    fprintf(sock, "USER %s 0 * :%s\r\n", BOT_NICK, BOT_NAME);
    fprintf(sock, "NICK %s\r\n", BOT_NICK);

    char line[512];
    while (fgets(line, sizeof(line), sock)) {
        char *prefix, *cmd, *params;
        parse_irc_msg(strtrim(line), &prefix, &cmd, &params);

        if (strlen(cmd) == 3) {
            switch (strtol(cmd, NULL, 10)) {
                case RPL_WELCOME:
                case RPL_YOURHOST:
                case RPL_CREATED:
                case RPL_MYINFO:
                case RPL_BOUNCE:
                    break;

                case RPL_MOTDSTART:
                    log_info("Receiving MOTD");
                    break;

                case RPL_MOTD:
                    break;

                case RPL_ENDOFMOTD:
                    log_info("Received MOTD");
                    break;

                default: log_info("%s %s %s", prefix, cmd, params);
            }
        }
        else if (streq(cmd, "PING")) {
            fprintf(sock, "PONG %s\r\n", params);
            log_info("Responding to PING");
        }
        else {
            log_info("%s %s %s", prefix, cmd, params);
        }
    }

    fclose(sock);
    return 0;
}


static FILE *
fsockopen(const char *host, int port)
{
    FILE *file = NULL;
    int fd = -1;
    struct hostent *he;
    struct sockaddr_in sa;


    he = gethostbyname(host);
    if (!he) {
        log_error("Failed to look up host");
        return NULL;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    memcpy(&sa.sin_addr, he->h_addr_list[0], 4);

    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        log_error("Failed to create socket: %d", errno);
        return NULL;
    }

    if (connect(fd, (struct sockaddr*)&sa, sizeof(sa)) != 0) {
        log_error("Failed to connect: %d", errno);
        close(fd);
        return NULL;
    }

    file = fdopen(fd, "r+b");
    if (!file) {
        log_error("Failed to create file stream: %d", errno);
        close(fd);
        return NULL;
    }

    return file;
}


static char *
strtrim(char *str)
{
    size_t len = strlen(str);
    char *tmp = str;

    while (len > 0 && isspace(str[len - 1])) {
        str[--len] = '\0';
    }
    while (len > 0 && isspace(*tmp)) {
        ++tmp;
        --len;
    }
    if (tmp != str) {
        memmove(str, tmp, len + 1);
    }
    return str;
}


static void
parse_irc_msg(char *msg, char **prefix, char **cmd, char **params)
{
    *prefix = *cmd = *params = "";

    if (*msg == ':') {
        *prefix = ++msg;
        while (*msg && !isspace(*msg)) ++msg;
        while (isspace(*msg)) *msg++ = '\0';
    }

    *cmd = msg;
    while (*msg && !isspace(*msg)) ++msg;
    while (isspace(*msg)) *msg++ = '\0';

    *params = msg;
}

