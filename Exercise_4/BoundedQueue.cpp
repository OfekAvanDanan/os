#include "BoundedQueue.h"

// Constructor: Initializes the queue with a fixed size
template <typename T>
BoundedQueue<T>::BoundedQueue(int size)
    : buffer(size), capacity(size), front(0), rear(0), count(0), mutex(1), empty_slots(size), filled_slots(0) {
  if (size <= 0) {
    throw std::invalid_argument("Queue size must be greater than 0");
  }
}

// Inserts an item into the queue
template <typename T> void BoundedQueue<T>::insert(const T &item) {
  empty_slots.acquire();        // Wait until there is an empty slot available
  mutex.acquire();              // Lock the queue for thread-safe access
  buffer[rear] = item;          // Add the item to the rear of the queue
  rear = (rear + 1) % capacity; // Update the rear index circularly
  count++;                      // Increment the count of items
  mutex.release();              // Unlock the queue
  filled_slots.release();       // Signal that a new item has been added
}

// Removes and returns the front item from the queue
template <typename T> T BoundedQueue<T>::remove() {
  filled_slots.acquire();         // Wait until there is an item in the queue
  mutex.acquire();                // Lock the queue for thread-safe access
  T item = buffer[front];         // Retrieve the item at the front of the queue
  front = (front + 1) % capacity; // Update the front index circularly
  count--;                        // Decrement the count of items
  mutex.release();                // Unlock the queue
  empty_slots.release();          // Signal that a slot is now free
  return item;                    // Return the retrieved item
}

// Checks if the queue is empty (for debugging purposes)
template <typename T> bool BoundedQueue<T>::isEmpty() const { return count == 0; }

// Checks if the queue is full (for debugging purposes)
template <typename T> bool BoundedQueue<T>::isFull() const { return count == capacity; }
