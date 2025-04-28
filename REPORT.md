# xv6 EDF Scheduling Policy 

## Project Overview
Earliest Deadline First (EDF) is a dynamic scheduling algorithm where the process with the nearest upcoming deadline is selected to run next. Unlike static priority-based schedulers, EDF priorities can change at runtime based on each process's deadline.

This project integrates EDF scheduling into the xv6 operating system to allow real-time task management, deadline monitoring, and scheduling based on temporal constraints.

▶️ [Watch the Demo Video](https://github.com/chester04/ADVOS_finalProject/blob/riscv/demo_video.mov) 


## Key Features and Changes
- in `proc.h` added
```c
int deadline; //process deadline
int period;   //process period
int wcet;              // C: worst‑case execution time (ticks)
int time_used;         // ticks used in current period
int edf;              // is this process in EDF scheduling?
```
- in `proc.c` added
  - functionality in `void schedular(void)` to choose a processes with the earliest deadline to run
      - if no edf procs, defaults to
  - new function `int kern_set_edf(int pid, int period, int wcet)` to allow the user to set edf deadlines for a given process.

- in `trap.c` - 

## How to run and testing
- firstly run `make qemu` to boot up xv6
- next while in the qemu terminal, run: ./edfTests

## Tasks
1. Sameen
  - Implemented a safety check in kernel/proc.c in the function kern_set_edf() so a process is admitted to EDF scheduling only when the total CPU utilization ≤ 100%
    - computed the task's capacity in milli-percent: cap = (wcet/period)*1000
    - loop through existing EDF tasks to count and then return -1 if we’re already over capacity
    - find and allow the target process
  - Updated kernel/trap.c in the usertrap() function to:
    - account for one tick of execution time per tick
    - roll a job to its next period when its WCET is exhausted or its deadline is reached
    - always yield on every timer interrupt
  - Expanded the test cases in EdfTests with Single-Task and Concurrent EDF test cases
2. Liza
- Implemented functionality in `schedular()` to:
    - find process with earliest dealine and runs it
    - the schedular defaults to round robin if no edf processes in ptable or the edf processes have reached their deadlines
- Updated `kernel/proc.c` in `fork()` to:
    - copy parent processes edf parameters into child
- Updated `kernel/proc.c` in `allocproc()` to:
    - initialize every processes edf variables
- Expanded upon test cases 1-3 in `edfTests.c` that Amy wrote initially
3. Amy
  - Contributing to the scheduler() function in the proc.c file, making sure processes with the earliest deadlines are picked first.
  - Writing and expanding user-level test cases in the edftest.c file to check basic EDF setup, scheduling orders, and how deadline misses are handled.
  - Making sure user-level functions could correctly call into the kernel’s kern_set_edf() to set deadlines and periods.
  - Testing and fixing issues where processes needed to roll over to their next period after using up their allowed time.
  - Setting up both simple and concurrent tests to make sure EDF scheduling worked as expected under different conditions.

