# xv6 EDF Scheduling Policy 

## Project Overview
Earliest Deadline First (EDF) is a dynamic scheduling algorithm where the process with the nearest upcoming deadline is selected to run next. Unlike static priority-based schedulers, EDF priorities can change at runtime based on each process's deadline.

This project integrates EDF scheduling into the xv6 operating system to allow real-time task management, deadline monitoring, and scheduling based on temporal constraints.

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

# How to run and testing
- firstly run `make qemu` to boot up xv6
- next while in the qemu terminal, run ./edfTests
