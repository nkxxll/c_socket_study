#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define BUFFERLEN 256
#define PORT 9999

extern int errno;

/*
 * Program that starts a server on 9999 localhost
 *
 * The server should listen for connections and read from the connections file descriptor.
 * The user has the opportunity to also write back to the connection.
 *
 * I tried to use error handling right and use the C language as it is meant to
 * be used. The project is actually to use strace, ltrace and gdb to see how
 * this code interacts with the system and how the asm of this code looks.
*/
int main(int argc, char *argv[])
{
    int error_number = 0;
    char *localhost = "localhost";
    struct sockaddr_in addr = { AF_INET, htons(PORT), 0 };

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int true = 1;
    // make the socket exit with clean up
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));

    if (sockfd == -1) {
        error_number = errno;
        fprintf(
            stderr,
            "Error number: %d.\nError: socket file descriptor was not created",
            error_number);
        fprintf(stderr, "Error description: %s", strerror(error_number));
    }

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
        error_number = errno;
        fprintf(
            stderr,
            "Error number: %d.\nError while binding to the port with the socket.",
            error_number);
        fprintf(stderr, "Error description: %s", strerror(error_number));
    }

    if (listen(sockfd, BACKLOG)) {
        error_number = errno;
        fprintf(stderr,
                "Error number: %d.\nError while listening for connection.",
                error_number);
        fprintf(stderr, "Error description: %s", strerror(error_number));
    }

    socklen_t addr_len = sizeof(addr);
    int confd = accept(sockfd, 0, 0);
    if (confd == -1) {
        error_number = errno;
        fprintf(stderr,
                "Error number: %d.\nError while accepting the connection.",
                error_number);
        fprintf(stderr, "Error description: %s", strerror(error_number));
    }

    struct pollfd fds[2] = { { 0, POLLIN, 0 }, { confd, POLLIN, 0 } };

    for (;;) {
        char buf[BUFFERLEN] = { 0 };
        if (poll(fds, 2, 50000) == -1) {
            error_number = errno;
            fprintf(
                stderr,
                "Error number: %d.\nError while polling for input from client or stdin.",
                error_number);
            fprintf(stderr, "Error description: %s", strerror(error_number));
        }

        if (fds[0].revents & POLLIN) {
            int n = read(0, buf, 255);
            if (n == -1) {
                error_number = errno;
                fprintf(stderr, "Error number: %d.\nError reading from stdin.",
                        error_number);
                fprintf(stderr, "Error description: %s",
                        strerror(error_number));
            } else if (n == 0 || n == 1) {
                close(confd);
                close(sockfd);
                return EXIT_SUCCESS;
            }
            if (send(confd, buf, 255, 0) == -1) {
                error_number = errno;
                fprintf(stderr, "Error number: %d.\nError reading from stdin.",
                        error_number);
                fprintf(stderr, "Error description: %s",
                        strerror(error_number));
            }
        } else if (fds[1].revents & POLLIN) {
            if (recv(confd, buf, 255, 0) == -1) {
                error_number = errno;
                fprintf(stderr, "Error number: %d.\nError reading from stdin.",
                        error_number);
                fprintf(stderr, "Error description: %s",
                        strerror(error_number));
            }
            printf("Client:%s", buf);
        }
    }

    return EXIT_SUCCESS;
}
