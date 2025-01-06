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
  Producer(int id, int numItems, int queueSize) : id(id), numItems(numItems) {
    queue = new BoundedQueue<std::string>(queueSize); // Dynamically allocate a new queue
  }

  // Destructor: Cleans up the dynamically allocated queue
  ~Producer() {
    delete queue; // Free the allocated memory for the queue
  }

  // Simulates production of items and inserts them into the queue
  void produceItems() {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);

    // GIVE
    for (int i = 0; i < numItems; ++i) {
      int itemNum = dist(gen);

      std::string item;
      int counter = 0;
      switch (itemNum) {
      case 0:
        item = "SPORT";
        counter = SportCounter++;
        break;
      case 1:
        item = "NEWS";
        counter = NewsCounter++;
        break;
      default:
        item = "WETHER";
        counter = WetherCounter++;
        break;
      }

      std::string output = "Producer " + std::to_string(id) + " " + item + " " + std::to_string(counter);
      queue->insert(output); // Insert the item into the queue
    }
    queue->insert("DONE"); // Signal the end of production
  }

  // Accessor for the queue
  BoundedQueue<std::string> *getQueue() const { return queue; }
};

#endif // PRODUCER_H
