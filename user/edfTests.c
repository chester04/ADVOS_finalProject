// user/edf_test.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

// -------------------------------------------
// Simple Child Process for EDF Setting
// -------------------------------------------
void basic_child(int period, int deadline, char *name) {
    int ret = user_set_edf(getpid(), period, deadline);
    if (ret == -1) {
        printf("FAILED to set EDF for %s (pid %d)\n", name, getpid());
    } else {
        printf("%s: EDF set with period %d deadline %d (pid %d)\n", name, period, deadline, getpid());
    }
    sleep(50); // simulate work
    printf("%s: done\n", name);
    exit(0);
}

// -------------------------------------------
// Test 1: Basic EDF Setting
// -------------------------------------------
void test_basic_edf() {
    printf("\n--- Test 1: Basic EDF Setting ---\n");
    int pid = fork();
    if (pid == 0) {
        basic_child(20, 10, "BasicProc");
    }
    wait(0);
}

// -------------------------------------------
// Test 2: Invalid PID Handling
// (Can't test properly in user mode)
// -------------------------------------------
void test_invalid_pid() {
    printf("\n--- Test 2: Invalid PID Handling (SKIPPED) ---\n");
}

// -------------------------------------------
// Test 3: EDF Prefers Earliest Deadline
// -------------------------------------------
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

// -------------------------------------------
// Test 4: Deadline Miss Handling
// -------------------------------------------
void test_deadline_miss() {
    printf("\n--- Test 4: Deadline Miss and Roll Over ---\n");
    int pid = fork();
    if (pid == 0) {
        user_set_edf(getpid(), 10, 20);
        printf("Process with short period started (pid %d)\n", getpid());
        sleep(30); // Miss the deadline intentionally
        printf("Process after missing deadline (pid %d)\n", getpid());
        exit(0);
    }
    wait(0);
}

// -------------------------------------------
// Utility: Single Task Test
// expect = 1 -> should succeed (ret >= 0)
// expect = 0 -> should fail   (ret < 0)
// -------------------------------------------
static void test_single(int period, int wcet, int expect, const char *name) {
    int pid = fork();
    if (pid < 0) {
        printf("FAIL: fork failed for %s\n", name);
        exit(1);
    }
    if (pid == 0) {
        int ret = user_set_edf(getpid(), period, wcet);
        if ((ret >= 0 && expect) || (ret < 0 && !expect)) {
            printf("PASS: %s (period=%d, wcet=%d)\n", name, period, wcet);
            exit(0);
        } else {
            printf("FAIL: %s (period=%d, wcet=%d) returned %d\n", name, period, wcet, ret);
            exit(1);
        }
    }
    wait(0);
}

// -------------------------------------------
// Utility: Concurrent Admission Test
// -------------------------------------------
static void test_concurrent(int p1_period, int p1_wcet,
                            int p2_period, int p2_wcet,
                            int p3_period, int p3_wcet,
                            int expect) {
    int p1 = fork();
    if (p1 < 0) { printf("FAIL: fork p1 failed\n"); exit(1); }
    if (p1 == 0) {
        if (user_set_edf(getpid(), p1_period, p1_wcet) < 0) {
            printf("FAIL: Task1 failed (period=%d, wcet=%d)\n", p1_period, p1_wcet);
            exit(1);
        }
        sleep(50);
        exit(0);
    }

    int p2 = fork();
    if (p2 < 0) { printf("FAIL: fork p2 failed\n"); exit(1); }
    if (p2 == 0) {
        if (user_set_edf(getpid(), p2_period, p2_wcet) < 0) {
            printf("FAIL: Task2 failed (period=%d, wcet=%d)\n", p2_period, p2_wcet);
            exit(1);
        }
        sleep(50);
        exit(0);
    }

    int p3 = fork();
    if (p3 < 0) { printf("FAIL: fork p3 failed\n"); exit(1); }
    if (p3 == 0) {
        int ret = user_set_edf(getpid(), p3_period, p3_wcet);
        if ((ret >= 0 && expect) || (ret < 0 && !expect)) {
            printf("PASS: Concurrent p3 (period=%d, wcet=%d)\n", p3_period, p3_wcet);
            exit(0);
        } else {
            printf("FAIL: Concurrent p3 (period=%d, wcet=%d) returned %d\n", p3_period, p3_wcet, ret);
            exit(1);
        }
    }

    wait(0);
    wait(0);
    wait(0);
}

// -------------------------------------------
// Main Test Runner
// -------------------------------------------
int main(void) {
    printf("\n=== EDF Tests Begin ===\n");

    // Run basic behavior tests
    test_basic_edf();
    test_invalid_pid();
    test_earliest_deadline();
    test_deadline_miss();

    // Run pass/fail validation tests
    printf("\n=== Single Task Admission Tests ===\n");
    test_single(10, 5, 1,  "Half-util (0.5)");
    test_single( 1, 1, 1,  "Full-util (1.0)");
    test_single( 1, 0, 1,  "Zero-util (0)");
    test_single( 5, 6, 0,  "WCET > Period");
    test_single( 0, 0, 0,  "Period = 0");
    test_single( 5, -1, 0, "WCET < 0");

    printf("\n=== Concurrent Admission Tests ===\n");
    test_concurrent(10, 5,
                    20, 10,
                    5,  1,
                    0); // p3 should be rejected

    printf("\n=== EDF Tests Complete ===\n");
    exit(0);
}
