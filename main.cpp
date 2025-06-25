#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <chrono>

// Custom semaphore 
struct Semaphore {
    std::mutex mtx;
    std::condition_variable cv;
    int count;

    Semaphore(int initial_count = 0) : count(initial_count) {}

    // The 'wait' operation ('acquire' or 'P').
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return count > 0; });
        count--;
    }

    // The 'signal' operation ('release' or 'V').
    void signal() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};


// --- Global Variables (Shared Resources) ---
// These variables are shared between producer and consumer threads.
std::queue<int> prime_queue;
bool producers_done = false;

std::mutex mtx; // Mutex to protect the shared queue
Semaphore prime_semaphore; // Semaphore to track items in the queue

std::ofstream output_file; // File I/O

// Checks if a given integer 'n' is a prime number.
bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// Producer Thread Function
// This function finds prime numbers within a range and adds them to the queue.
void producer_task(int start, int end) {
    for (int i = start; i <= end; ++i) {
        if (isPrime(i)) {
            // Protect the shared queue with the global mutex.
            std::unique_lock<std::mutex> lock(mtx);
            prime_queue.push(i);
            lock.unlock();

            // Signal the consumer that a new item is ready.
            prime_semaphore.signal();
        }
    }
}

// Consumer Thread Function
// This function takes prime numbers from the queue and writes them to a file.
void consumer_task() {
    while (true) {
        // Wait on the semaphore for an item to become available.
        // This will block until a producer signals.
        prime_semaphore.wait();

        // After waiting, acquire the lock to access the shared queue.
        std::unique_lock<std::mutex> lock(mtx);

        // Check the termination condition. If the queue is empty AND
        // all producers are done, we can terminate.
        if (prime_queue.empty() && producers_done) {
            // Release the lock before breaking.
            lock.unlock();
            break;
        }

        // Get the prime number and pop it from the queue.
        int prime = prime_queue.front();
        prime_queue.pop();

        // Release the lock as we are done with the shared resource.
        lock.unlock();

        // Write the prime number to the output file.
        output_file << prime << std::endl;
    }
}


int main() {
    int N;
    std::cout << "Enter N (upper limit for prime numbers): ";
    std::cin >> N;

    output_file.open("primes.txt");

    // to calculate exec time
    // Start time.
    auto start_time = std::chrono::high_resolution_clock::now();
    int mid = N / 2;

    // Producer threads.
    std::thread producer1(producer_task, 0, mid);
    std::thread producer2(producer_task, mid + 1, N);

    // Single consumer thread.
    std::thread consumer(consumer_task);

    // Wait for both producer threads to finish their execution.
    producer1.join();
    producer2.join();

    // to Inform the consumer that all producers have finished.
    {
        std::unique_lock<std::mutex> lock(mtx);
        producers_done = true;
        lock.unlock();

        // to Wake up the consumer in case it's waiting on an
        // empty queue after all primes have been processed.
        // to prevent deadlock
        prime_semaphore.signal();
    }

    // Wait for the consumer thread to finish its execution.
    consumer.join();

    // End time.
    auto end_time = std::chrono::high_resolution_clock::now();
    // Calculate time.
    std::chrono::duration<double> elapsed = end_time - start_time;

    output_file.close();

    std::cout << "Prime numbers up to " << N << " have been written to primes.txt" << std::endl;
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
