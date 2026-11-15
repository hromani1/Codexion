*This project has been created as part of the 42 curriculum by hromani.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads. A group of coders sits around a circular table, with one USB dongle between each pair of neighbours (as many dongles as coders). To compile their quantum code, a coder needs **two dongles at the same time**, the left one and the right one. After compiling, a coder puts both dongles back, debugs, refactors, and then tries to compile again.

A coder **burns out** if they do not start a compile within `time_to_burnout` milliseconds of the start of their previous compile (or of the simulation, for the first one). The simulation ends when a coder burns out, or when every coder has compiled at least `number_of_compiles_required` times.

The goal of the project is to share a scarce resource fairly between threads without deadlock, without starvation, and with precise timing:

- one thread per coder, plus one monitor thread that detects burnout;
- each dongle is protected by a mutex, has a **cooldown** after being released, and has its own **priority queue (a binary heap)** of waiting coders;
- the queue is ordered by a scheduler chosen at launch: `fifo` (First In, First Out) or `edf` (Earliest Deadline First, with deadline = `last_compile_start + time_to_burnout`);
- every state change is logged on a single serialized line.

Log format (one line per state change):

```
<timestamp_in_ms> <coder_id> has taken a dongle
<timestamp_in_ms> <coder_id> is compiling
<timestamp_in_ms> <coder_id> is debugging
<timestamp_in_ms> <coder_id> is refactoring
<timestamp_in_ms> <coder_id> burned out
```

### Code layout

| File | Role |
|---|---|
| `main.c` | Parses the arguments, builds the simulation, starts and joins the threads |
| `parse.c` | Argument validation and conversion |
| `sim.c`, `sim_clean.c` | Allocation, initialization and cleanup of every structure |
| `coder.c` | The routine run by each coder thread |
| `dongles.c`, `dongles2.c` | Dongle acquisition, release and waiting |
| `heap*.c` | The priority queue (min-heap) and its comparison functions |
| `monitor.c` | The monitor thread |
| `helpers*.c` | Time, stop flag, serialized logging, interruptible sleep |
| `codexion.h` | Types and prototypes (there are no global variables) |

## Instructions

### Compilation

```
make        # builds ./codexion (cc -Wall -Wextra -Werror -pthread)
make clean  # removes object files
make fclean # removes object files and the binary
make re     # fclean + all
```

### Execution

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | Number of coders, and also of dongles |
| `time_to_burnout` | ms allowed between the start of two consecutive compiles |
| `time_to_compile` | ms a coder holds both dongles while compiling |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `number_of_compiles_required` | The simulation stops when every coder has compiled this many times |
| `dongle_cooldown` | ms a released dongle stays unavailable |
| `scheduler` | `fifo` or `edf` (exactly, case-sensitive) |

All eight arguments are mandatory. The program rejects, with an error message on `stderr`, a wrong number of arguments, negative numbers, non-integers, empty strings, values that do not fit in an `int`, zero for `number_of_coders` and for the three phase durations and the burnout time, and any scheduler other than `fifo` or `edf`.

### Examples

```
./codexion 5 800 200 200 200 3 0 fifo     # feasible: ends when everyone has compiled 3 times
./codexion 5 800 200 200 200 3 100 edf    # same, with a 100 ms dongle cooldown
./codexion 4 310 200 100 100 5 0 fifo     # a coder burns out
./codexion 1 800 200 200 200 3 0 fifo     # one dongle only: burnout at 800 ms
```

### Testing suggestions

Concurrency bugs are intermittent, so run each scenario many times, for example:

```
for i in $(seq 100); do timeout 15 ./codexion 5 800 200 200 200 3 0 edf > out.txt; echo "$?"; done | sort | uniq -c
```

Useful checks on an output file: exactly zero or one `burned out` line and, if present, it is the last line; timestamps never decrease; each `is compiling` is preceded by two `has taken a dongle` lines from the same coder. The program was also checked with `valgrind --leak-check=full`, `valgrind --tool=helgrind` and a `-fsanitize=thread` build. Note that some versions of Helgrind report a "dubious" warning from inside glibc's `pthread_cond_timedwait`, which does not point at project code.

## Resources

**Documentation and references**

- Linux manual pages: `pthread_create(3)`, `pthread_join(3)`, `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`, `pthread_cond_timedwait(3)`, `pthread_cond_broadcast(3)`, `gettimeofday(2)`, `clock_gettime(2)`.
- E. G. Coffman, M. Elphick, A. Shoshani, *System Deadlocks*, ACM Computing Surveys 3(2), 1971 (the four conditions for deadlock).
- E. W. Dijkstra, *Hierarchical ordering of sequential processes* (EWD310), 1971 (the dining philosophers problem, which this project resembles).
- C. L. Liu, J. W. Layland, *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment*, Journal of the ACM 20(1), 1973 (Earliest Deadline First).
- Valgrind documentation (Memcheck, Helgrind) and the ThreadSanitizer documentation, used to look for leaks, data races and lock-order problems.

**How AI was used**

An AI assistant (Claude) was used as a tutor and reviewer, not as a replacement for understanding the code:

- *Planning and explanation:* breaking the subject into ordered steps (parser, structures, time and logging, coder loop, dongles, monitor), and explaining concepts such as Coffman's conditions, why a shared flag needs a mutex, and how condition variables and `pthread_cond_timedwait` behave.
- *Code review:* reviewing code written by the author and pointing out bugs, for example a function that returned nothing, an uninitialized mutex, mixed absolute and relative time bases, a self-deadlock from locking a mutex twice, and the lock-order problem in the two-dongle wait.
- *Debugging:* analysing log output and tool reports (valgrind, Helgrind) to find the cause of hangs and false burnouts, such as a wrong pointer passed to the monitor thread and head-of-line blocking in the dongle queues.

Everything submitted was reviewed, tested with the tools above, and is understood by the author.

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)

A deadlock needs four conditions at once: mutual exclusion, hold and wait, no preemption, and circular wait.

- **Mutual exclusion** is inherent: a dongle can be used by one coder at a time. It is kept.
- **No preemption** is kept: nobody takes a dongle from a coder who is compiling.
- **Hold and wait is broken.** A coder never holds one dongle while waiting for the other. It creates a single request and puts that same request in the waiting queue of both of its dongles. Only when both dongles are free, both cooldowns have elapsed and the request is at the top of both queues does it take them, in one step, under the mutexes of both dongles. If any condition fails it waits while holding nothing. Because a waiting coder owns no dongle, nobody can be blocked behind an idle holder.
- **Circular wait on the mutexes** is also avoided: when a thread needs both dongle mutexes, it always locks the one with the lower id first. While waiting on a condition variable it holds only one mutex (the other is released first), and it takes the second back only after getting the first, preserving the order.

### Starvation prevention

- With `edf`, the dongle queues are ordered by `last_compile_start + time_to_burnout`, so the coder closest to burnout is served first.
- With `fifo`, requests are served in arrival order. The request's key is its sequence number, which is strictly increasing, so there are no ties.
- A single sequence number is allocated per attempt (under its own mutex) and used for both queues. Every queue therefore ranks requests in the same global order, so the highest-priority waiting coder is at the top of both of its queues and can only be delayed by dongles that are in use or cooling down, and those are released in bounded time. This is also what prevents two neighbours from each being first in one queue and waiting for each other.
- Equal deadlines under `edf` are broken by the sequence number, so the order is fully deterministic.

### Dongle cooldown

When a dongle is released, its `available_at` field is set to the release time plus `dongle_cooldown`. A coder can take a dongle only when the current time is at or after `available_at`. Waiters re-check after short timed waits, so the end of a cooldown is noticed within about one millisecond even though nobody signals it.

### Precise burnout detection

A dedicated monitor thread runs a loop every millisecond. For each coder it reads `last_compile_start` under that coder's mutex and compares `elapsed time - last_compile_start` with `time_to_burnout`. On burnout it sets the stop flag first and prints the `burned out` line afterwards, so the burnout message is detected and printed within a few milliseconds of the real deadline (the subject allows 10 ms) and is always the last line of the output. The monitor also checks completion (every coder has compiled enough times) before checking burnout.

### Log serialization

All output goes through one function protected by a dedicated mutex, so lines never interleave. The timestamp is taken **inside** the locked section, which guarantees that timestamps never go backwards in the output. After the stop flag is set, every message is suppressed except the single `burned out` line.

### Single coder

With one coder there is only one dongle, so the coder can take it but never gets a second one. The coder thread logs `has taken a dongle` and ends without locking the same mutex twice, and the monitor reports the burnout at `time_to_burnout`.

### Clean and prompt shutdown

When the stop flag is set, nothing may stay blocked: `precise_sleep` sleeps in short slices and checks the flag, condition-variable waits are capped at 1 ms, the monitor broadcasts on every dongle after stopping, and a coder that gives up removes its own request from both queues. Threads are joined (coders and monitor) before any mutex is destroyed and any memory is freed. If initialization fails partway, only what was actually created is destroyed, using counters that record how many mutexes, coders and dongles were initialized.

### Known limitation

Requests are served strictly by priority in each queue, so a coder at the head of one queue can delay a coder behind it even if the dongle is free. When all coders start at the same instant, this can serialize the first round of compiles, which matters for large counts with tight parameters.

## Thread synchronization mechanisms

There are no global variables. All shared state lives in a `t_simulation` structure that every thread reaches through a pointer.

### Primitives and what each protects

| Primitive | Protects |
|---|---|
| `dongle->lock` (`pthread_mutex_t`) | `in_use`, `available_at` and the dongle's waiting queue |
| `dongle->cond` (`pthread_cond_t`) | Waiting for a dongle to become available (used with `pthread_cond_timedwait`); signalled with `pthread_cond_broadcast` on release and at shutdown |
| `coder->lock` (`pthread_mutex_t`) | `last_compile_start` and `compile_count`, written by the coder and read by the monitor |
| `stop_mutex` | The `stop` flag, written by the monitor and read by every thread through `is_stopped()` |
| `log_mutex` | Output, and the timestamp taken at print time |
| `seq_mutex` | The counter that produces request sequence numbers |

### Lock rules

- A thread never holds a coder lock, the log mutex or the stop mutex while taking another lock, except that the log function reads the stop flag while holding the log mutex. The stop mutex is a leaf lock: nothing is ever locked while holding it.
- Dongle mutexes are always taken in ascending dongle id.
- Logging happens after the dongle mutexes are released.
- A thread blocks only inside a condition-variable wait, which releases its mutex while sleeping.

### How race conditions are prevented

- **The stop flag:** every read goes through a function that locks `stop_mutex`, copies the value into a local variable and unlocks, so threads never read it without synchronization and the compiler cannot cache it in a register.
- **Taking a dongle:** checking that the dongle is free, its cooldown has passed and the request is at the top of the queue, and then marking it in use, all happen within one hold of the dongle's mutex(es), so two coders can never take the same dongle.
- **Burnout data:** the coder updates `last_compile_start` and `compile_count` under its own mutex and the monitor reads both under the same mutex, copies them into local variables, and unlocks before doing any comparison.
- **Sequence numbers:** the shared counter is incremented under its own mutex.

### Thread-safe communication between coders and the monitor

- Coders to monitor: through `last_compile_start` and `compile_count`, always under the coder's mutex.
- Monitor to coders: through the stop flag (read under `stop_mutex`) and a broadcast on every dongle's condition variable.
- Condition waits use a 1 ms timeout and are always re-checked in a loop, which also handles spurious wake-ups. The timeout also matters because the stop flag is protected by a different mutex than the dongle's condition variable: a thread could check the flag, then the monitor could set it and broadcast before the thread goes to sleep. The timeout bounds how long such a missed wake-up can last.

### Priority queue

Each dongle owns a small binary min-heap of requests. Each request stores the coder id, a key (arrival sequence for `fifo`, burnout deadline for `edf`) and a sequence number used as tie-breaker. The heap holds a pointer to a comparison function chosen once from the scheduler, so scheduling policy lives in a single place. Its capacity is 2, because a dongle can only ever be requested by its two neighbours, and a coder has at most one pending request per dongle.

### Time

Timestamps come from `gettimeofday` in milliseconds. Internally all times are relative to the simulation start (`elapsed_time`), and the absolute clock is only used to build the deadline for timed waits.
