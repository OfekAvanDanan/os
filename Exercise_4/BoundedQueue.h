#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <mutex>
#include <semaphore>
#include <stdexcept>
#include <string>
#include <vector>

// A thread-safe bounded queue implementation using semaphores and a mutex
template <typename T> class BoundedQueue {
private:
  std::vector<T> buffer; // The internal storage for the queue
  int capacity;          // Maximum size of the queue
  int front;             // Index of the front element in the queue
  int rear;              // Index of the rear element in the queue
  int count;             // Current number of items in the queue

  std::binary_semaphore mutex;            // Semaphore for mutual exclusion
  std::counting_semaphore<> empty_slots;  // Semaphore to track empty slots
  std::counting_semaphore<> filled_slots; // Semaphore to track filled slots

public:
  // Constructor: Initializes the queue with a fixed size
  BoundedQueue(int size);

  // Inserts an item into the queue
  void insert(const T &item);

  // Removes and returns the front item from the queue
  T remove();

  // Checks if the queue is empty (for debugging purposes)
  bool isEmpty() const;

  // Checks if the queue is full (for debugging purposes)
  bool isFull() const;
};

#include "BoundedQueue.cpp"

#endif // BOUNDED_QUEUE_H
