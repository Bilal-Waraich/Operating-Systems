<!--
Suggested GitHub Topics: c operating-systems concurrency pthreads semaphores systems-programming processes
-->

# Operating Systems — Systems Programming in C

A collection of low-level systems programming implementations in C, covering process management, POSIX thread concurrency, and synchronisation primitives. Written as part of an OS course at Constructor University.

## Overview

This repository contains hands-on implementations of core operating systems concepts: process creation with `fork`/`exec`, multi-threaded programming with POSIX threads, synchronisation with mutexes and condition variables, and classic concurrency problems. Each program is standalone and buildable with GCC.

## Implementations

| File | Concept | Description |
|------|---------|-------------|
| `src/xargs-implementation.c` | Process Management | A `xargs`-like utility — reads lines from stdin, builds argument lists, and spawns child processes with `fork`/`execvp`. Supports `-n` (max args), `-t` (trace), and `-j` (parallel jobs). |
| `src/pdi-numbers-threads.c` | POSIX Threads | Finds Perfect Digital Invariant numbers in a range using multiple threads. Partitions the search space across `N` threads and joins them. |
| `src/100-prisoners.c` | Concurrency + Algorithms | Simulates the 100 Prisoners Problem with 4 strategies (random global, random per-drawer, smart global, smart per-drawer) using pthreads and mutexes. Measures and compares win rates. |
| `src/vending-machine-sync.c` | Condition Variables | Models a vending machine with supplier, machine, and student threads. Uses `pthread_mutex` and `pthread_cond` to synchronise coin insertion, drink dispensing, and machine refilling. |
| `src/fork-exec-curl.c` | fork/exec/pipe | Fetches a URL by forking a child process, replacing its stdout with a pipe, and exec'ing `curl`. Parent reads the response via the pipe. |

## Highlights

The **100 Prisoners Problem** (`src/100-prisoners.c`) is the most algorithmically interesting implementation. The problem: 100 prisoners each need to find their own number among 100 randomly arranged drawers, but each may only open 50. The naive random strategy fails with near-certainty (probability ≈ (1/2)^100), but the cyclic permutation strategy — starting from your own number and following the chain — succeeds with probability ≈ 30.7%. The implementation runs all four strategies in parallel using pthreads, with either a global lock or per-drawer locks, and reports win rates over many simulated games.

## Build & Run

Each file is a standalone C program. Compile and run:

```bash
# Without threading
gcc src/xargs-implementation.c -o xargs-impl
gcc src/fork-exec-curl.c -o fork-curl  # requires src/quiz.h (included)

# With POSIX threads (-lpthread required)
gcc src/pdi-numbers-threads.c -o pdi-numbers -lpthread -lm
gcc src/100-prisoners.c -o prisoners -lpthread
gcc src/vending-machine-sync.c -o vending -lpthread

# Run
./prisoners
./pdi-numbers -s 1 -e 100000 -t 4
echo "one two three" | ./xargs-impl
```

## Raw Assignments

The original homework submissions and weekly project files are preserved in [`/raw_assignments`](raw_assignments/README.md).
