// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include "Dispatcher.h"

// Constructor
Dispatcher::Dispatcher(BoundedQueue<std::string> *S_dispatcher_queue, BoundedQueue<std::string> *N_dispatcher_queue,
                       BoundedQueue<std::string> *W_dispatcher_queue, std::vector<Producer *> *producerVector,
                       int numOfProducers)
    : S_dispatcher_queue(S_dispatcher_queue), N_dispatcher_queue(N_dispatcher_queue),
      W_dispatcher_queue(W_dispatcher_queue), producerVector(producerVector), numOfProducers(numOfProducers),
      allProducersDone(false) {}

// Function to process messages from producers
void Dispatcher::dispatchMessages() {
  std::vector<bool> producersDone(numOfProducers, false); // Tracks if each producer is done
  int activeProducers = numOfProducers;                   // Tracks the number of active producers

  while (activeProducers > 0) {
    for (int i = 0; i < numOfProducers; ++i) {
      if (producersDone[i]) {
        continue; // Skip producers that are already done
      }

      Producer *producer = (*producerVector)[i];
      BoundedQueue<std::string> *queue = producer->getQueue();

      if (!queue->isEmpty()) {
        std::string message = queue->remove();

        if (message == "DONE") {
          producersDone[i] = true;
          --activeProducers;
        } else {
          routeMessage(message);
        }
      }
    }
  }

  // Send "DONE" to all dispatcher queues
  S_dispatcher_queue->insert("DONE");
  N_dispatcher_queue->insert("DONE");
  W_dispatcher_queue->insert("DONE");
}

// Function to route a message to the appropriate dispatcher queue
void Dispatcher::routeMessage(const std::string &message) {
  if (message.find("SPORT") != std::string::npos) {
    S_dispatcher_queue->insert(message);
  } else if (message.find("NEWS") != std::string::npos) {
    N_dispatcher_queue->insert(message);
  } else if (message.find("WETHER") != std::string::npos) {
    W_dispatcher_queue->insert(message);
  } else {
    std::cerr << "Unknown message type: " << message << std::endl;
  }
}
