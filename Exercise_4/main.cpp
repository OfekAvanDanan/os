#include "Config.h"
#include "ConfigParser.h"
#include "Producer.h"
#include <iostream>
#include <thread>
#include <vector>

// Function to consume items from a producer's queue
void consumeItems(Producer *producer, const std::string &consumerName) {
  auto *queue = producer->getQueue();
  while (true) {
    std::string item = queue->remove();
    if (item == "DONE") {
      break;
    }
    std::cout << consumerName << " consumed: " << item << std::endl;
  }
}

int main(int argc, char *argv[]) {
  // Check for configuration file argument
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
    return 1;
  }

  std::string configFilePath = argv[1];
  Config config;

  // Parse the configuration file
  if (!parseConfigFile(configFilePath, config)) {
    std::cerr << "Error: Failed to parse the configuration file." << std::endl;
    return 1;
  }

  // Create producers based on the configuration
  std::vector<Producer *> producers;
  std::vector<std::thread> producerThreads;
  std::vector<std::thread> consumerThreads;

  for (const auto &pConfig : config.producers) {
    Producer *producer = new Producer(pConfig.id, pConfig.numberOfProducts, pConfig.queueSize);
    producers.push_back(producer);

    // Start the producer thread
    producerThreads.emplace_back(&Producer::produceItems, producer);

    // Start the consumer thread
    std::string consumerName = "Consumer " + std::to_string(pConfig.id);
    consumerThreads.emplace_back(consumeItems, producer, consumerName);
  }

  // Wait for all producer threads to finish
  for (auto &t : producerThreads) {
    if (t.joinable()) {
      t.join();
    }
  }

  // Wait for all consumer threads to finish
  for (auto &t : consumerThreads) {
    if (t.joinable()) {
      t.join();
    }
  }

  // Clean up producers
  for (auto &producer : producers) {
    delete producer;
  }

  std::cout << "All producers and consumers have finished." << std::endl;
  return 0;
}
