# Prime Number Generator (Multi-threaded)

---

### What

This is a C++ program that finds all prime numbers up to a user-specified limit, $N$. It saves the results to a text file and measures the total time elapsed.

---

### Why

The program demonstrates a fundamental concept in concurrent programming: the **producer-consumer** pattern. It uses multiple threads to split the work of finding prime numbers (producers) and a single thread to handle the output (consumer). This approach leverages multi-core processors to perform the prime number generation and file writing in parallel, which can be more efficient for large values of $N$.

---

### How

The program uses three main threads:

* **Producer Threads (2):** These threads are responsible for checking a range of numbers for primality. When a prime is found, it is placed into a shared queue.

* **Consumer Thread (1):** This thread continuously monitors the queue. As soon as a prime number is available, it is taken from the queue and written to the `primes.txt` file.


#### 🔒 Thread Synchronization Mechanisms

A `mutex` to ensure safe access to the shared queue and preventing data corruption

A **Custom semaphore** using `condition_variable` to manage thread synchronization, ensuring consumers wait for available items and producers signal availability in the shared queue.



---

### How to Execute

To compile and run this program, you will need a C++ compiler that supports the C++11 standard or newer, such as g++.

1.  **Compile the code:** Open a terminal and use the following command. The `-pthread` flag is essential to link the necessary threading libraries.

    ```bash
    g++ -std=c++11 -o main main.cpp -pthread
    ```

2.  **Run the executable:**

    ```bash
    ./main
    ```

3.  Follow the on-screen prompt to enter your desired upper limit for prime number generation. After the program finishes, you will find the results in a new file named `primes.txt` in the same directory.

<br>

| Input (N)       | Time (s) Single Threaded | Time (s) Multi-threaded (2 Threads) |
|-----------------|--------------------------|-------------------------------------|
| 100M            | 2.59219                  | 1.597                               |
| 1000M           | 64.2714                  | 41.8247                             |

  ![Demo](demo/demo.gif)
