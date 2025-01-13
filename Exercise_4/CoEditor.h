#pragma once

#include "BoundedQueue.h"
#include <chrono>
#include <string>
#include <thread>

// CoEditor class to edit messages from Dispatcher queues
class CoEditor {
private:
  std::string type;                           // Type of messages this CoEditor handles (SPORTS, NEWS, WEATHER)
  BoundedQueue<std::string> *dispatcherQueue; // Reference to Dispatcher queue
  BoundedQueue<std::string> *sharedQueue;     // Shared queue with Screen Manager

public:
  // Constructor
  CoEditor(const std::string &type, BoundedQueue<std::string> *dispatcherQ, BoundedQueue<std::string> *sharedQ)
      : type(type), dispatcherQueue(dispatcherQ), sharedQueue(sharedQ) {}

  // Function to start editing messages
  void edit() {
    while (true) {
      std::string message = dispatcherQueue->remove();

      if (message == "DONE") {
        // Pass "DONE" to shared queue
        sharedQueue->insert("DONE");
        break;
      }

      // Simulate editing delay
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      // Insert edited message into the shared queue
      sharedQueue->insert(message);
    }
  }
};
