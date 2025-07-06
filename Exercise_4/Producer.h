// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#ifndef PRODUCER_H
#define PRODUCER_H

#include "BoundedQueue.h"
#include <iostream>
#include <random>
#include <string>

// Producer class to represent a producer with an ID and associated queue
class Producer {
private:
  int id;       // Producer's unique ID
  int numItems; // Number of items to produce
  int SportCounter = 0;
  int WetherCounter = 0;
  int NewsCounter = 0;
  BoundedQueue<std::string> *queue; // Pointer to the queue for the producer

public:
  // Constructor: Initializes the producer with ID, number of items, and queue size
  Producer(int id, int numItems, int queueSize);

  // Destructor: Cleans up the dynamically allocated queue
  ~Producer();

  // Simulates production of items and inserts them into the queue
  void produceItems();

  // Accessor for the queue
  BoundedQueue<std::string> *getQueue() const;
};

#endif // PRODUCER_H
