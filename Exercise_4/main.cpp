#include "Producer.h"
#include <iostream>
#include <thread>

// Function to consume items from a producer's queue
void consumeItems(Producer *producer, const std::string &producerName) {
  auto *queue = producer->getQueue();
  while (true) {
    std::string item = queue->remove();
    if (item == "DONE") {
      break;
    }
    std::cout << producerName << " consumed: " << item << std::endl;
  }
}

int main() {
  // Create two producers
  Producer producer1(1, 10, 5); // ID 1, 10 items, queue size 5
  Producer producer2(2, 15, 5); // ID 2, 15 items, queue size 5

  // Start producing items in parallel
  std::thread producerThread1(&Producer::produceItems, &producer1);
  std::thread producerThread2(&Producer::produceItems, &producer2);

  // Consume items in parallel
  std::thread consumerThread1(consumeItems, &producer1, "Consumer 1");
  std::thread consumerThread2(consumeItems, &producer2, "Consumer 2");

  // Wait for all threads to finish
  producerThread1.join();
  producerThread2.join();
  consumerThread1.join();
  consumerThread2.join();

  std::cout << "All producers and consumers have finished." << std::endl;
  return 0;
}
