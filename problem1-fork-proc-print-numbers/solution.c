#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        return 1;
    }

    for (int i = 1; i <= n; ++i) {
        if (i < n) {
            printf("%d ", i);
            fflush(stdout);

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                return 1;
            }
            if (pid > 0) {
                int status = 0;
                waitpid(pid, &status, 0);
                return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
            }
        } else {
            printf("%d\n", i);
            fflush(stdout);
        }
    }

    return 0;
}
