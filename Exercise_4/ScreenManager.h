#pragma once

#include "BoundedQueue.h"
#include <iostream>
#include <string>

// ScreenManager class to display messages from the shared queue
class ScreenManager {
private:
  BoundedQueue<std::string> *sharedQueue; // Reference to the shared queue
  int doneCount;                          // Count of "DONE" messages received
  int totalDone;                          // Total "DONE" messages expected

public:
  // Constructor
  ScreenManager(BoundedQueue<std::string> *sharedQ, int totalDone)
      : sharedQueue(sharedQ), doneCount(0), totalDone(totalDone) {}

  // Function to start displaying messages
  void display() {
    while (doneCount < totalDone) {
      std::string message = sharedQueue->remove();

      if (message == "DONE") {
        ++doneCount;
      } else {
        std::cout << "ScreenManager displays: " << message << std::endl;
      }
    }
    std::cout << "ScreenManager: All messages processed. Exiting." << std::endl;
  }
};
