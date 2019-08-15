#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

unsigned int snooze(unsigned int secs) {
    unsigned int leftsleep = sleep(secs);
    printf("Slept for %d of %d seconds.\n", secs - leftsleep, secs);
}

void sigint_handler(int sig) {
    printf("Do you think Ctrl-C is useful?\n");
    int i;
    for (i=0; i<10; i++) {
        sleep(1);
        printf(". ");
        fflush(stdout);
    }
    printf("OK. It is.\n");
}

int main(int argc, char const *argv[], char const *env[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <secs>\n", argv[0]);
        exit(0);
    }
    // install the SIGINT handler
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("signal error!");
        exit(0);
    }

    // do not allow interrupt
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    
    sig_atomic_t x;
    // run normal procedure
    int secs = atoi(argv[1]);
    snooze(secs);

    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    return 0;
}