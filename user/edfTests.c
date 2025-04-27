// user/edf_test.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

//basic child that sets EDF and does some simulated work
void basic_child(int period, int deadline, char *name) {
    int ret = user_set_edf(getpid(), period, deadline);
    if (ret == -1) {
        printf("FAILED to set EDF for %s (pid %d)\n", name, getpid());
    } else {
        printf("%s: EDF set with period %d deadline %d (pid %d)\n", name, period, deadline, getpid());
    }
    sleep(50); //simulate work
    printf("%s: done\n", name);
    exit(0);
}

void test_basic_edf() {
    printf("\n--- Test 1: Basic EDF Setting ---\n");
    int pid = fork();
    if (pid == 0) {
        basic_child(10, 20, "BasicProc");
    }
    wait(0);
}

//need to test invalid pid handling in kernel mode
void test_invalid_pid() {
    printf("\n--- Test 2: Invalid PID Handling (SKIPPED) ---\n");
    printf("(Would need kernel-mode unit test)\n");
}

// see if the scheduler prefers earliest deadline
void test_earliest_deadline() {
    printf("\n--- Test 3: EDF Scheduler Preference ---\n");

    int pid1 = fork();
    if (pid1 == 0) {
        user_set_edf(getpid(), 50, 50); // later deadline
        for (int i = 0; i < 5; i++) {
            printf("Long deadline process running (pid %d)\n", getpid());
            sleep(5);
        }
        exit(0);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        user_set_edf(getpid(), 10, 10); // earlier deadline
        for (int i = 0; i < 5; i++) {
            printf("Short deadline process running (pid %d)\n", getpid());
            sleep(5);
        }
        exit(0);
    }

    wait(0);
    wait(0);
}

//what happenes if we miss a deadline (need to fix)
void test_deadline_miss() {
    printf("\n--- Test 4: Deadline Miss and Roll Over ---\n");
    int pid = fork();
    if (pid == 0) {
        user_set_edf(getpid(), 10, 20);
        printf("Process with short period started (pid %d)\n", getpid());
        sleep(30); //miss deadline 
        printf("Process after missing deadline (pid %d)\n", getpid());
        exit(0);
    }
    wait(0);
}

int main() {
    printf("\n=== Starting EDF Tests ===\n");

    test_basic_edf();
    test_invalid_pid();
    test_earliest_deadline();
    test_deadline_miss();

    printf("\n=== EDF Tests Complete ===\n");
    exit(0);
}
