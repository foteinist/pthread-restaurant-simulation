#   Running Guide - Pizza Restaurant Simulation

This guide provides detailed instructions on how to compile, run, and interpret the results of the multithreaded pizza restaurant simulation.

##  Table of Contents
- [Prerequisites](#-prerequisites)
- [Project Structure](#-project-structure)
- [Quick Start](#-quick-start)
- [Running the Simulation](#-running-the-simulation)
- [Understanding the Output](#-understanding-the-output)
- [Example Runs](#-example-runs)
- [Configuration Tuning](#-configuration-tuning)
- [Troubleshooting](#-troubleshooting)
- [Testing Scenarios](#-testing-scenarios)
- [Quick Reference](#-quick-reference)

---

##  Prerequisites

Before running the simulation, ensure you have:

- **Linux/Unix environment** (tested on Ubuntu 20.04+)
- **GCC compiler** (version 7.0 or higher)
- **POSIX Threads library** (included with GCC)

### Check your environment:
```bash
# Check GCC version
gcc --version

# Check if pthread library is available
ls /usr/lib/libpthread.so || ls /lib/x86_64-linux-gnu/libpthread.so
```

If GCC is not installed:
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install gcc build-essential

# Fedora/RHEL
sudo dnf install gcc glibc-devel

# Arch Linux
sudo pacman -S gcc
```

---
##  Project Structure

The project is organized as follows:

```
pizza-restaurant-simulation/
│
├── src/
│   ├── restaurant.c          # Main implementation file
│   └── config.h               # Configuration constants
│
├── test-res.sh                # Compilation and execution script
├── README.md                   # Project documentation
└── RUNNING.md                  # This guide
```

### File Descriptions

| File | Description |
|------|-------------|
| `src/restaurant.c` | Main source code with all pthread logic |
| `src/config.h` | Configuration constants (resources, timing, prices) |
| `test-res.sh` | Bash script to compile and run the simulation |
| `README.md` | Project overview and documentation |
| `RUNNING.md` | Detailed running instructions (this file) |

---

##  Quick Start

Follow these steps to quickly clone, compile, and run the simulation.

---

### 1. Clone or download the project
```bash
git clone https://github.com/foteinist/pizza-restaurant-simulation.git
cd pizza-restaurant-simulation
```

### 2. Compile the Program

**Option A: Using the run script (recommended)**
```bash
# Make the script executable
chmod +x test-res.sh

# Run it
./test-res.sh
```

**Option B: Manual compilation**
```bash
# Create bin directory
mkdir -p bin

# Compile the program (pthread flag is required)
gcc -Wall -pthread src/restaurant.c -o bin/restaurant

# Check if compilation was successful
ls -la bin/
```

**Option C: One-liner compile and run**
```bash
gcc -Wall -pthread src/restaurant.c -o bin/restaurant && ./bin/restaurant 100 10
```

---

##  Running the Simulation

### Basic Syntax
```bash
./bin/restaurant <number_of_customers> <random_seed>
```

### Parameters
| Parameter | Description | Constraints | Example |
|-----------|-------------|-------------|---------|
| `number_of_customers` | Total customers to simulate | Must be > 0 | `100`, `50`, `1000` |
| `random_seed` | Seed for random number generator | Must be ≥ 0 | `42`, `123`, `0` |

### Examples

1. **Small test run** (5 customers)
```bash
./bin/restaurant 5 42
```

2. **Normal operation** (100 customers)
```bash
./bin/restaurant 100 10
```

3. **Stress test** (500 customers)
```bash
./bin/restaurant 500 123
```

4. **Multiple runs with different seeds**
```bash
# Run 5 different simulations
for seed in 1 2 3 4 5; do
    echo "========== Run with seed $seed =========="
    ./bin/restaurant 100 $seed
    echo ""
done
```

5. **Run with timing measurement**
```bash
time ./bin/restaurant 100 42
```

---

##  Understanding the Output

### 1. **Real-time Logs**
During execution, you'll see messages like this:

```
The number of customers is 100!
Main:Costumers id 1
Ring Ring!
The costumer 1 communicates with the worker!
The costumer with id 1 has to pay 34!
Pizza maker is preparing 3 pizzas for customer 1.
Preparation for order 1 is done!
Cook is baking 3 pizzas for order 1.
Baking completed for order 1.
Deliverer is packing order 1.
Deliverer is delivering order 1.
Total delivery time for order 1: 25 minutes.
```

**What each message means:**

| Message | Meaning |
|---------|---------|
| `Ring Ring!` | A new customer is calling |
| `Operator is Busy!` | All phone lines are occupied |
| `No pizza maker available` | All cooks are busy |
| `Not enough ovens available` | Waiting for oven space |
| `No deliverer available` | All deliverers are on delivery |
| `The order X has failed!` | Payment failed (5% probability) |

### 2. **Final Statistics**
After all customers are processed, you'll see:

```
Total revenue: 1234!
Number of pizza margarita: 45!
Number of pizza peperoni: 32!
Number of pizza special: 38!
Number of successful orders: 95!
Number of failed orders: 5!
Average customer service time: 28 minutes
Maximum customer service time: 45 minutes
Average cooling time: 12 minutes
Maximum cooling time: 18 minutes
```

### 3. **What the statistics mean**

| Metric | Description | Good Range | Interpretation |
|--------|-------------|------------|----------------|
| **Total revenue** | Total money earned | 800-1200 | Higher = more successful orders |
| **Margarita count** | Number sold | ~35% of total | Should match Pm=0.35 |
| **Pepperoni count** | Number sold | ~25% of total | Should match Pp=0.25 |
| **Special count** | Number sold | ~40% of total | Should match Ps=0.40 |
| **Success rate** | Successful orders | ~95% | Pfail=0.05 means 95% success |
| **Failed orders** | Failed payments | ~5% | Should be close to Pfail |
| **Avg service time** | Order to delivery | 25-40 min | Affected by resource availability |
| **Max service time** | Longest order | < 60 min | Indicates bottlenecks |
| **Avg cooling time** | Bake to delivery | 8-15 min | Indicates delivery efficiency |
| **Max cooling time** | Longest cooling | < 25 min | Shouldn't be too high |

---

##  Example Runs with Expected Results

### Example 1: Default Configuration
Using the default settings in `config.h`:
```bash
./bin/restaurant 100 10
```
**Expected output:**
```
Total revenue: ~950 (varies with seed)
Number of pizza margarita: ~35
Number of pizza peperoni: ~25
Number of pizza special: ~40
Number of successful orders: ~95
Number of failed orders: ~5
Average customer service time: ~30 minutes
```

### Example 2: High Failure Rate
First, edit `src/config.h`:
```c
float Pfail = 0.20;  // Change from 0.05 to 0.20 (20% failure)
```
Then compile and run:
```bash
gcc -Wall -pthread src/restaurant.c -o bin/restaurant
./bin/restaurant 100 42
```
**Expected output:**
```
Number of successful orders: ~80
Number of failed orders: ~20
Total revenue: ~800 (lower due to more failures)
```

### Example 3: Resource Starvation
Edit `src/config.h` to reduce resources:
```c
int Ncook = 1;      // Only 1 cook (was 2)
int Noven = 3;      // Only 3 ovens (was 10)
int Ndeliverer = 2; // Only 2 deliverers (was 10)
```
Run:
```bash
./bin/restaurant 50 123
```
**Expected output:** Many waiting messages:
```
No pizza maker available. Customer 5 is waiting.
Not enough ovens available. Order 3 is waiting.
No deliverer available. Order 7 is waiting.
Average customer service time: ~60 minutes (much higher)
```

### Example 4: Fast Food Configuration
Edit `src/config.h`:
```c
int Tprep = 0;      // Instant preparation
int Tbake = 2;      // Fast baking (2 sec)
int Tpack = 0;      // Instant packing
int Tdellow = 1;    // Fast delivery (1-3 sec)
int Tdelhigh = 3;
```
Run:
```bash
./bin/restaurant 100 7
```
**Expected output:** Very fast service times:
```
Average customer service time: ~8 minutes
Maximum customer service time: ~12 minutes
```

---

##  Configuration Tuning

You can modify `src/config.h` to experiment with different scenarios. Here are all the parameters you can tune:

### Resource Counts
```c
int Ntel = 2;           // Telephone operators (phone lines)
int Ncook = 2;          // Number of cooks
int Noven = 10;         // Number of ovens
int Ndeliverer = 10;    // Number of deliverers
```

### Timing Parameters (in seconds)
```c
int Torderlow = 1;      // Min time between calls
int Torderhigh = 5;     // Max time between calls
int Norderlow = 1;      // Min pizzas per order
int Norderhigh = 5;     // Max pizzas per order
int Tpaymentlow = 1;    // Min payment time
int Tpaymenthigh = 3;   // Max payment time
int Tprep = 1;          // Prep time per pizza
int Tbake = 10;         // Baking time (constant)
int Tpack = 1;          // Packing time per pizza
int Tdellow = 5;        // Min delivery time
int Tdelhigh = 15;      // Max delivery time
```

### Pizza Probabilities
```c
float Pm = 0.35;        // Margarita probability (35%)
float Pp = 0.25;        // Pepperoni probability (25%)
float Ps = 0.40;        // Special probability (40%)
float Pfail = 0.05;     // Order failure probability (5%)
```

### Pizza Prices
```c
int Cm = 10;            // Margarita price (€)
int Cp = 11;            // Pepperoni price (€)
int Cs = 12;            // Special price (€)
```

### Configuration Examples

**Budget Pizzeria**
```c
int Ncook = 1;          // Only one cook
int Noven = 5;          // Few ovens
int Tprep = 2;          // Slower preparation
int Cm = 8;             // Cheaper prices
int Cp = 9;
int Cs = 10;
```

**High-End Restaurant**
```c
int Ncook = 5;          // Many cooks
int Noven = 20;         // Many ovens
int Tprep = 3;          // Artisanal preparation
int Tbake = 15;         // Longer baking
int Cm = 15;            // Higher prices
int Cp = 16;
int Cs = 18;
```

**Party Central (Large Orders)**
```c
int Norderlow = 3;      // Min 3 pizzas
int Norderhigh = 10;    // Max 10 pizzas
int Ncook = 5;          // More cooks
int Noven = 30;         // Many ovens
```

---

##  Troubleshooting

### Common Issues and Solutions

#### Issue 1: Segmentation Fault (Core Dumped)
```
Segmentation fault (core dumped)
```
**Solutions:**
```bash
# 1. Check if you're using too many customers
./bin/restaurant 50 42  # Try with fewer customers

# 2. Check for memory leaks with valgrind
valgrind --leak-check=full ./bin/restaurant 10 42

# 3. Compile with debug symbols and run in gdb
gcc -Wall -pthread -g src/restaurant.c -o bin/restaurant-debug
gdb ./bin/restaurant-debug
(gdb) run 100 10
(gdb) backtrace
```

#### Issue 2: Program Hangs / No Output
**Solutions:**
```bash
# 1. Check if it's actually running (Ctrl+C to stop)
./bin/restaurant 100 42
# Press Ctrl+C to stop if hung

# 2. Run with strace to see system calls
strace -f -o trace.log ./bin/restaurant 100 42

# 3. Check for deadlocks - attach with gdb
ps aux | grep restaurant
gdb -p <PID>
(gdb) thread apply all bt
```

#### Issue 3: "ERROR: return code from pthread_mutex_lock() is 22"
**Solution:** Mutex not initialized. Make sure all mutexes are initialized in main():
```c
// Check that your main() has these initializations
pthread_mutex_init(&lock_Ntel, NULL);
pthread_mutex_init(&lock_Ncook, NULL);
pthread_mutex_init(&lock_Noven, NULL);
pthread_mutex_init(&lock_Ndeliverer, NULL);
pthread_mutex_init(&lock_output, NULL);
pthread_mutex_init(&lock_statistics, NULL);
```

#### Issue 4: Unexpected Statistics
**Problem:** Pizza counts don't match probabilities (35%/25%/40%)

**Solutions:**
```bash
# 1. Run with more customers for better statistical significance
./bin/restaurant 1000 42

# 2. Run multiple times and average
for seed in 1 2 3 4 5 6 7 8 9 10; do
    ./bin/restaurant 100 $seed | grep "Number of pizza"
done

# 3. Check random seed initialization
# In main(), ensure rand_r is properly seeded:
Customers_Id[threadCount].random = seed + threadCount + 1;
```

#### Issue 5: "WARNING: unintialized random generator"
**Solution:** Make sure you're using `rand_r()` (thread-safe) not `rand()`:
```c
// CORRECT (thread-safe)
int randomValue = rand_r(&(tid->random));

// WRONG (not thread-safe)
int randomValue = rand();
```

---

##  Testing Scenarios

### Scenario 1: Deadlock Detection Test
Run with minimal resources to test for deadlocks:
```bash
# Edit config.h
int Ntel = 1;
int Ncook = 1;
int Noven = 1;
int Ndeliverer = 1;

# Compile and run
gcc -Wall -pthread src/restaurant.c -o bin/restaurant
./bin/restaurant 20 42
```
**Expected:** Program should complete without hanging. If it hangs, there's a deadlock.

### Scenario 2: Race Condition Test
Run the same seed multiple times - output should be identical:
```bash
# Run with same seed, save outputs
./bin/restaurant 100 123 > run1.txt
./bin/restaurant 100 123 > run2.txt

# Compare - should be identical
diff run1.txt run2.txt
```
Minor differences in log ordering may occur due to thread scheduling.
However, final statistics (revenue, counts, averages) should remain identical
for the same seed.

### Scenario 3: Stress Test
Push the system to its limits:
```bash
# Increase customers gradually
for customers in 10 50 100 200 500 1000; do
    echo "=== Testing $customers customers ==="
    time ./bin/restaurant $customers 42 > /dev/null
done
```

### Scenario 4: Determinism Test
Verify that the same seed produces the same results:
```bash
#!/bin/bash
echo "=== Determinism Test ==="
for i in {1..5}; do
    echo "Run $i:"
    ./bin/restaurant 50 777 | grep "Total revenue"
done
```
All runs should show the same revenue.

### Scenario 5: Resource Utilization Test
Monitor system resources while running:
```bash
# In one terminal, run the simulation
./bin/restaurant 200 42

# In another terminal, monitor
top -H -p $(pgrep restaurant)
# OR
htop
```

---

##  Logging and Debugging

### Save Output to File
```bash
# Save everything to a file
./bin/restaurant 100 42 > simulation.log

# Save and also see on screen
./bin/restaurant 100 42 | tee simulation.log

# Append to existing file
./bin/restaurant 100 43 >> simulation.log
```

### Enable Verbose Debug Output
Edit `restaurant.c` and uncomment debug lines:
```c
// Around line 150
printf("number of pizzas %d! \n", randomNumberOfPizzas);

// Around line 160
printf("Propability is %d !\n", rand_r(&(tid->random)) % 100);
```

Then recompile:
```bash
gcc -Wall -pthread src/restaurant.c -o bin/restaurant-debug
./bin/restaurant-debug 10 42
```

### Create a Test Script
Create `test.sh`:
```bash
#!/bin/bash
echo "=== Pizza Restaurant Test Suite ==="

# Test 1: Basic functionality
echo "Test 1: Basic run with 5 customers"
./bin/restaurant 5 42 > /dev/null
if [ $? -eq 0 ]; then
    echo " Test 1 passed"
else
    echo " Test 1 failed"
fi

# Test 2: Edge case - 0 customers (should fail)
echo "Test 2: Edge case - 0 customers"
./bin/restaurant 0 42 2>/dev/null
if [ $? -ne 0 ]; then
    echo " Test 2 passed (correctly failed)"
else
    echo " Test 2 failed (should have failed)"
fi

# Test 3: Large number of customers
echo "Test 3: Stress test - 500 customers"
time ./bin/restaurant 500 42 > /dev/null
if [ $? -eq 0 ]; then
    echo " Test 3 passed"
else
    echo " Test 3 failed"
fi

echo "=== Test Suite Complete ==="
```

Make it executable and run:
```bash
chmod +x test.sh
./test.sh
```

---

##  Performance Analysis

### Measure Execution Time
```bash
# Basic timing
time ./bin/restaurant 100 42

# More detailed with /usr/bin/time
/usr/bin/time -v ./bin/restaurant 100 42
```

### Memory Usage Analysis
```bash
# Check for memory leaks
valgrind --leak-check=full --show-leak-kinds=all ./bin/restaurant 50 42

# Check memory usage during execution
valgrind --tool=massif ./bin/restaurant 100 42
ms_print massif.out.*
```

### Thread Analysis
```bash
# Show thread creation/destruction
strace -f -e clone ./bin/restaurant 20 42

# Count thread creations
strace -f -e clone ./bin/restaurant 100 42 2>&1 | grep clone | wc -l
```

---

##  Educational Exercises

### Exercise 1: Modify Pizza Types
Add a new pizza type in `config.h`:
```c
// Add new pizza type
float P_veggie = 0.15;  // 15% probability
int C_veggie = 13;      // 13€ price

// Adjust existing probabilities (must sum to 1.0)
// Pm = 0.30, Pp = 0.20, Ps = 0.35, P_veggie = 0.15
```

### Exercise 2: Implement Priority System
Modify the code to prioritize large orders (>4 pizzas) by giving them access to resources first.

### Exercise 3: Add Statistics
Add new statistics to track:
- Average waiting time for each resource
- Busiest time of day
- Most popular pizza by hour

### Exercise 4: Deadlock Detection
Implement a deadlock detection mechanism that times out if a thread waits too long.

---

##  Quick Reference Card

### Common Commands

| Action | Command |
|--------|---------|
| Compile | `gcc -Wall -pthread src/restaurant.c -o bin/restaurant` |
| Run (100 cust) | `./bin/restaurant 100 42` |
| Run & save | `./bin/restaurant 100 42 \| tee output.txt` |
| Time execution | `time ./bin/restaurant 100 42` |
| Check memory | `valgrind ./bin/restaurant 20 42` |
| Debug with gdb | `gdb ./bin/restaurant` |

### Expected Results (100 customers)

| Metric | Expected Range |
|--------|---------------|
| Total revenue | 800-1200 € |
| Success rate | 94-96% |
| Failed orders | 4-6 |
| Margarita pizzas | 30-40 |
| Pepperoni pizzas | 20-30 |
| Special pizzas | 35-45 |
| Avg service time | 25-40 min |
| Avg cooling time | 8-15 min |

### Common Errors & Fixes

| Error | Solution |
|-------|----------|
| `undefined reference to pthread_create` | Add `-pthread` flag |
| `Segmentation fault` | Reduce customers, check pointers |
| Program hangs | Check resource deadlocks |
| Wrong statistics | Verify random seed initialization |
| Mutex errors | Ensure all mutexes are initialized |

### Configuration Quick Reference

```c
// Resource counts
int Ntel = 2;           // Phone lines
int Ncook = 2;          // Cooks
int Noven = 10;         // Ovens
int Ndeliverer = 10;    // Deliverers

// Timing (seconds)
int Tprep = 1;          // Prep per pizza
int Tbake = 10;         // Baking time
int Tpack = 1;          // Packing per pizza
int Tdellow = 5;        // Min delivery
int Tdelhigh = 15;      // Max delivery

// Probabilities
float Pm = 0.35;        // Margarita (35%)
float Pp = 0.25;        // Pepperoni (25%)
float Ps = 0.40;        // Special (40%)
float Pfail = 0.05;     // Failure (5%)
```

---

##  Quick Test Script

Save this as `quick_test.sh`:

```bash
#!/bin/bash
echo " Quick Test - Pizza Restaurant Simulation"
echo "========================================"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Test with different customer counts
for cust in 5 10 20; do
    echo -e "\n${YELLOW}Testing with $cust customers...${NC}"
    ./bin/restaurant $cust 42 | grep -E "Total revenue|successful|failed|Average"
done

echo -e "\n${GREEN}Test complete!${NC}"
```

Run it:
```bash
chmod +x quick_test.sh
./quick_test.sh
```

---

##  Additional Resources

- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- [Mutex vs Condition Variables](https://www.geeksforgeeks.org/mutex-vs-semaphore/)
- [Thread Safety in C](https://en.wikipedia.org/wiki/Thread_safety)
- [Valgrind Documentation](https://valgrind.org/docs/manual/quick-start.html)
- [GDB Debugging Guide](https://sourceware.org/gdb/current/onlinedocs/gdb/)

---

##  Getting Help

If you encounter issues:

1. **Check the error message** - Most errors are descriptive
2. **Verify your compilation command** - Don't forget `-pthread`
3. **Test with small numbers** - Start with 5-10 customers
4. **Check config.h values** - Ensure they make sense
5. **Run with valgrind** - Check for memory issues
6. **Search the error** - Many pthread errors are common

---

**Happy simulating!** 
