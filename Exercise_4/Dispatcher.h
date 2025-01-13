#pragma once

#include "Producer.h"
#include <iostream>
#include <string>
#include <vector>

// Dispatcher class to handle routing messages from producers to dispatcher queues
class Dispatcher {
private:
  BoundedQueue<std::string> *S_dispatcher_queue; // Queue for SPORTS messages
  BoundedQueue<std::string> *N_dispatcher_queue; // Queue for NEWS messages
  BoundedQueue<std::string> *W_dispatcher_queue; // Queue for WEATHER messages
  std::vector<Producer *> *producerVector;       // Vector of pointers to producer objects
  int numOfProducers;                            // Number of producers
  bool allProducersDone;                         // Flag to check if all producers are done

public:
  // Constructor
  Dispatcher(BoundedQueue<std::string> *S_dispatcher_queue, BoundedQueue<std::string> *N_dispatcher_queue,
             BoundedQueue<std::string> *W_dispatcher_queue, std::vector<Producer *> *producerVector,
             int numOfProducers);

  // Function to process messages from producers
  void dispatchMessages();

private:
  // Function to route a message to the appropriate dispatcher queue
  void routeMessage(const std::string &message);
};
