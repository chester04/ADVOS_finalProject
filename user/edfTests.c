#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

// void child(int pid, int period, int deadline, char * name) {
//   user_set_edf(pid,period,deadline);
//   printf( "%s running with deadline %d\n", name, deadline);
//   sleep(50);  // simulate work
//   printf( "%s done\n", name);

// }

// int main() {

//   if (fork() == 0) {
//     int pida = fork();
//     child(pida, 10, 20, "Process C");
//   }

//   if (fork() == 0) {
//     int pidb = fork();
//     child(pidb,5, 10, "Process A");
//   }

//   if (fork() == 0) {
//     int pidc = fork();
//     child(pidc,6,36, "Process B");
//   }


//   printf("EDF test complete\n");

// }




// Single-task  test
// expect = 1 -> should succeed (ret>=0)
// expect = 0 -> should fail   (ret<0)
static void
test_single(int period, int wcet, int expect, const char *name)
{
    int pid = fork();
    if (pid < 0) 
    {
        printf("FAIL: fork failed for %s\n", name);
        exit(1);
    }
    if (pid == 0) 
    {
        int ret = user_set_edf(getpid(), period, wcet);
        if ((ret >= 0 && expect) || (ret < 0 && !expect)) 
        {
            printf("PASS: %s (period=%d, wcet=%d)\n", name, period, wcet);
            exit(0);
        } 
        else 
        {
            printf("FAIL: %s (period=%d, wcet=%d) returned %d\n",
                   name, period, wcet, ret);
            exit(1);
        }
    }
    wait(0);
}

// Concurrent-task admission test
// First two tasks will stay active for 50 ticks
// The third should be rejected (expect=0)
static void
test_concurrent(int p1_period, int p1_wcet,
                int p2_period, int p2_wcet,
                int p3_period, int p3_wcet,
                int expect)
{
    int p1 = fork();
    if (p1 < 0) { printf("FAIL: fork p1 failed\n"); exit(1); }
    if (p1 == 0) 
    {
        if (user_set_edf(getpid(), p1_period, p1_wcet) < 0) 
        {
            printf("FAIL: Task1 failed (period=%d, wcet=%d)\n", p1_period, p1_wcet);
            exit(1);
        }
        sleep(50);
        exit(0);
    }

    int p2 = fork();
    if (p2 < 0) { printf("FAIL: fork p2 failed\n"); exit(1); }
    if (p2 == 0) 
    {
        if (user_set_edf(getpid(), p2_period, p2_wcet) < 0) 
        {
            printf("FAIL: Task2 failed (period=%d, wcet=%d)\n", p2_period, p2_wcet);
            exit(1);
        }
        sleep(50);
        exit(0);
    }

    int p3 = fork();
    if (p3 < 0) { printf("FAIL: fork p3 failed\n"); exit(1); }
    if (p3 == 0) 
    {
        int ret = user_set_edf(getpid(), p3_period, p3_wcet);
        if ((ret >= 0 && expect) || (ret < 0 && !expect)) 
        {
            printf("PASS: Concurrent p3 (period=%d, wcet=%d)\n", p3_period, p3_wcet);
            exit(0);
        } 
        else 
        {
            printf("FAIL: Concurrent p3 (period=%d, wcet=%d) returned %d\n",
                   p3_period, p3_wcet, ret);
            exit(1);
        }
    }

    wait(0);
    wait(0);
    wait(0);
}

int
main(void)
{
    // Single-task tests
    test_single(10, 5, 1,  "Half-util (0.5)");
    test_single( 1, 1, 1,  "Full-util (1.0)");
    test_single( 1, 0, 1,  "Zero-util (0)");
    test_single( 5, 6, 0,  "WCET > Period");
    test_single( 0, 0, 0,  "Period = 0");
    test_single( 5, -1, 0, "WCET < 0");

    // Concurrent test: expects rejection for p3
    test_concurrent
    (
         10, 5,
         20,10,
          5, 1,
          0
    );

    printf("ALL TESTS DONE\n");
    exit(0);
}