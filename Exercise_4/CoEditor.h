// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

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
  CoEditor(const std::string &type, BoundedQueue<std::string> *dispatcherQ, BoundedQueue<std::string> *sharedQ);

  // Function to start editing messages
  void edit();
};
