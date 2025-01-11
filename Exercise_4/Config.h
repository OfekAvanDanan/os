#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

// Structure to hold individual producer configuration
struct ProducerConfig {
  int id;               // Producer's unique ID
  int numberOfProducts; // Number of products to produce
  int queueSize;        // Size of the producer's queue
};

// Structure to hold the entire configuration
struct Config {
  std::vector<ProducerConfig> producers; // List of producer configurations
  int coEditorQueueSize;                 // Size of the Co-Editor queue
};

#endif // CONFIG_H
