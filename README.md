# Multithreaded Restaurant Simulation (POSIX Threads)

A concurrent restaurant order simulation implemented in C using POSIX threads (pthreads), mutexes, and condition variables.

This project models a pizza restaurant where each customer is represented as a separate thread competing for limited shared resources (telephone operators, cooks, ovens, and deliverers).

Developed as part of an Operating Systems course.

---

## Overview

The system simulates the full lifecycle of a restaurant order:

1. Customer calls
2. Order placement
3. Pizza preparation
4. Baking
5. Delivery
6. Statistics calculation

Each customer is executed as an independent thread.  
Shared resources are synchronized using mutexes and condition variables to prevent race conditions.

---

## Concurrency Model

The system uses:

- POSIX Threads (pthreads)
- Mutex locks
- Condition variables
- Shared global state protection
- Thread creation and joining
- Resource contention handling

### Shared Resources

- Telephone operators (Ntel)
- Cooks (Ncook)
- Ovens (Noven)
- Deliverers (Ndeliverer)

If a resource is unavailable, threads wait using condition variables until they are signaled.

---

## Configuration

System parameters are defined in `config.h`, including:

- Number of operators, cooks, ovens, deliverers
- Pizza probabilities
- Order failure probability
- Preparation, baking, packing, and delivery times
- Pricing configuration

This allows easy modification of workload and system behavior.

---

## Key Concepts Demonstrated

- Thread lifecycle management
- Synchronization using mutexes
- Conditional waiting (pthread_cond_wait)
- Signaling and broadcasting
- Shared memory protection
- Performance metric tracking
- Randomized workload simulation
- Resource allocation control

---

## Compilation

Compile manually:

gcc -Wall -pthread src/restaurant.c -o restaurant


Or use the provided script:

./test-res.sh


---

## Execution

./restaurant <number_of_customers> <random_seed>


Example:

./restaurant 100 10


---

## Output

The program prints:

- Order progress logs
- Resource waiting messages
- Total revenue
- Successful and failed orders
- Average and maximum service time
- Average and maximum cooling time

---

## Example Simulation Flow

Customer Thread →  
Call Handling →  
Preparation →  
Baking →  
Delivery →  
Statistics Update

All steps are synchronized to prevent resource conflicts.

---

## Limitations

- No deadlock detection
- No priority scheduling
- No dynamic resource scaling
- In-memory simulation only
- Some shared counters are updated without fine-grained locking (possible improvement)

---

## Environment

Developed and tested on:

- Ubuntu Linux (VM)
- GCC
- POSIX Threads

---

## Authors

- Konstantina Karapetsa
- Foteini Sotiropoulou
