// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include "Producer.h"

// Constructor
Producer::Producer(int id, int numItems, int queueSize) : id(id), numItems(numItems) {
  queue = new BoundedQueue<std::string>(queueSize); // Dynamically allocate a new queue
}

// Destructor
Producer::~Producer() {
  delete queue; // Free the allocated memory for the queue
}

// Simulates production of items and inserts them into the queue
void Producer::produceItems() {
  // int pCounter = 0;

  // Create a random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 2);

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
    // + " producer Counter" + std::to_string(pCounter++);

    queue->insert(output); // Insert the item into the queue
  }
  queue->insert("DONE"); // Signal the end of production
}

// Accessor for the queue
BoundedQueue<std::string> *Producer::getQueue() const { return queue; }
