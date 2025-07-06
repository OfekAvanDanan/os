// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

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
  ScreenManager(BoundedQueue<std::string> *sharedQ, int totalDone);

  // Function to start displaying messages
  void display();
};
