#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    char expr[4096];
    if (fgets(expr, sizeof(expr), stdin) == NULL) {
        return 1;
    }

    expr[strcspn(expr, "\n")] = '\0';

    const char prefix[] = "print(";
    const char suffix[] = ")";
    size_t code_len = strlen(prefix) + strlen(expr) + strlen(suffix) + 1;
    char *code = malloc(code_len);
    if (code == NULL) {
        return 1;
    }

    snprintf(code, code_len, "%s%s%s", prefix, expr, suffix);

    execlp("python3", "python3", "-c", code, (char *)NULL);
    perror("execlp");
    free(code);
    return 1;
}
