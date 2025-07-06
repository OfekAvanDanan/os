// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include "CoEditor.h"

// Constructor
CoEditor::CoEditor(const std::string &type, BoundedQueue<std::string> *dispatcherQ, BoundedQueue<std::string> *sharedQ)
    : type(type), dispatcherQueue(dispatcherQ), sharedQueue(sharedQ) {}

// Function to start editing messages
void CoEditor::edit() {
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
