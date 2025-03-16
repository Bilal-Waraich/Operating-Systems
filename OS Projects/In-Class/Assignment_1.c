#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;
static int verbose = 0;

static int isenv(const char *arg) {
    return strchr(arg, '=') != NULL;
}

static void set(char *arg) {
    if (verbose) {
        fprintf(stderr, "setenv:\t%s\n", arg);
    }
    if (putenv(arg) == -1) {
        perror("putenv");
    }
}

static void unset(char *arg) {
    if (verbose) {
        fprintf(stderr, "unset:\t%s\n", arg);
    }
    if (unsetenv(arg) == -1) {
        perror("unsetenv");
    }
}

static void show(char *env[]) {
    for (int i = 0; env[i]; i++) {
        puts(env[i]);
    }
}

static void exec(char *argv[]) {
    if (verbose) {
        fprintf(stderr, "executing: %s\n", argv[0]);
        for (int i = 0; argv[i]; i++) {
            fprintf(stderr, "arg[%d]= '%s'\n", i, argv[i]);
        }
    }
    execvp(argv[0], argv);
    perror("execvp");
    _exit(1);
}

int main(int argc, char *argv[]) {
    int opt, nargc = 0;
    char *nargv[argc];

    while ((opt = getopt(argc, argv, "vu:")) != -1) {
        switch (opt) {
        case 'v':
            verbose = 1;
            break;
        case 'u':
            unset(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "usage: env [-v] [-u name] [name=value ...] command [arg ...]]\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        if (isenv(argv[i])) {
            set(argv[i]);
        } else {
            nargv[nargc++] = argv[i];
        }
    }

    nargv[nargc] = NULL;

    if (nargv[0] == NULL) {
        show(environ);
    } else {
        exec(nargv);
    }

    return EXIT_SUCCESS;
}
