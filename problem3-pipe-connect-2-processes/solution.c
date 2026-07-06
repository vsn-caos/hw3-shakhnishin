#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <CMD1> <CMD2>\n", argv[0]);
        return 1;
    }

    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t first = fork();
    if (first < 0) {
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        return 1;
    }

    if (first == 0) {
        if (dup2(fds[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            _exit(1);
        }
        close(fds[0]);
        close(fds[1]);
        execlp(argv[1], argv[1], (char *)NULL);
        perror("execlp");
        _exit(1);
    }

    pid_t second = fork();
    if (second < 0) {
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        waitpid(first, NULL, 0);
        return 1;
    }

    if (second == 0) {
        if (dup2(fds[0], STDIN_FILENO) < 0) {
            perror("dup2");
            _exit(1);
        }
        close(fds[0]);
        close(fds[1]);
        execlp(argv[2], argv[2], (char *)NULL);
        perror("execlp");
        _exit(1);
    }

    close(fds[0]);
    close(fds[1]);

    int st1 = 0, st2 = 0;
    waitpid(first, &st1, 0);
    waitpid(second, &st2, 0);

    if (WIFEXITED(st2)) {
        return WEXITSTATUS(st2);
    }
    return 1;
}
