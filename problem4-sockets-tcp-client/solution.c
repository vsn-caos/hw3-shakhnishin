#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int write_all(int fd, const void *buf, size_t size) {
    const char *p = (const char *)buf;
    while (size > 0) {
        ssize_t written = send(fd, p, size, 0);
        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (written == 0) {
            return -1;
        }
        p += written;
        size -= (size_t)written;
    }
    return 0;
}

static int read_all(int fd, void *buf, size_t size) {
    char *p = (char *)buf;
    while (size > 0) {
        ssize_t got = recv(fd, p, size, 0);
        if (got < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (got == 0) {
            return 0;
        }
        p += got;
        size -= (size_t)got;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ipv4_addr> <port>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1) {
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    int32_t value;
    while (scanf("%d", &value) == 1) {
        if (write_all(sock, &value, sizeof(value)) < 0) {
            close(sock);
            return 0;
        }

        int32_t response;
        int rc = read_all(sock, &response, sizeof(response));
        if (rc <= 0) {
            close(sock);
            return 0;
        }
        printf("%d\n", response);
        fflush(stdout);
    }

    close(sock);
    return 0;
}
