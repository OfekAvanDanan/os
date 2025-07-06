// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include "ScreenManager.h"

// Constructor
ScreenManager::ScreenManager(BoundedQueue<std::string> *sharedQ, int totalDone)
    : sharedQueue(sharedQ), doneCount(0), totalDone(totalDone) {}

// Function to start displaying messages
void ScreenManager::display() {
  while (doneCount < totalDone) {
    std::string message = sharedQueue->remove();

    if (message == "DONE") {
      ++doneCount;
    } else {
      std::cout << message << std::endl;
    }
  }
  std::cout << "DONE" << std::endl;
}
