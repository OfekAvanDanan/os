#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <vector>
#include <string>
#include <semaphore>
#include <mutex>
#include <stdexcept>

// A thread-safe bounded queue implementation using semaphores and a mutex
template<typename T>
class BoundedQueue {
private:
    std::vector<T> buffer; // The internal storage for the queue
    int capacity;          // Maximum size of the queue
    int front;             // Index of the front element in the queue
    int rear;              // Index of the rear element in the queue
    int count;             // Current number of items in the queue

    std::binary_semaphore mutex;  // Semaphore for mutual exclusion
    std::counting_semaphore<> empty_slots; // Semaphore to track empty slots
    std::counting_semaphore<> filled_slots; // Semaphore to track filled slots

public:
    // Constructor: Initializes the queue with a fixed size
    BoundedQueue(int size)
        : buffer(size), capacity(size), front(0), rear(0), count(0),
          mutex(1), empty_slots(size), filled_slots(0) {
        if (size <= 0) {
            throw std::invalid_argument("Queue size must be greater than 0");
        }
    }

    // Inserts an item into the queue
    void insert(const T& item) {
        empty_slots.acquire(); // Wait until there is an empty slot available
        mutex.acquire();       // Lock the queue for thread-safe access
        buffer[rear] = item;   // Add the item to the rear of the queue
        rear = (rear + 1) % capacity; // Update the rear index circularly
        count++;               // Increment the count of items
        mutex.release();       // Unlock the queue
        filled_slots.release(); // Signal that a new item has been added
    }

    // Removes and returns the front item from the queue
    T remove() {
        filled_slots.acquire(); // Wait until there is an item in the queue
        mutex.acquire();        // Lock the queue for thread-safe access
        T item = buffer[front]; // Retrieve the item at the front of the queue
        front = (front + 1) % capacity; // Update the front index circularly
        count--;                // Decrement the count of items
        mutex.release();        // Unlock the queue
        empty_slots.release();  // Signal that a slot is now free
        return item;            // Return the retrieved item
    }

    // Checks if the queue is empty (for debugging purposes)
    bool isEmpty() const {
        return count == 0;
    }

    // Checks if the queue is full (for debugging purposes)
    bool isFull() const {
        return count == capacity;
    }
};

#endif // BOUNDED_QUEUE_H
