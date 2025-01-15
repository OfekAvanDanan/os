// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include "CoEditor.h"
#include "Config.h"
#include "ConfigParser.h"
#include "Dispatcher.h"
#include "Producer.h"
#include "ScreenManager.h"
#include <iostream>
#include <thread>
#include <vector>

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

  for (const auto &pConfig : config.producers) {
    Producer *producer = new Producer(pConfig.id, pConfig.numberOfProducts, pConfig.queueSize);
    producers.push_back(producer);

    // Start the producer thread
    producerThreads.emplace_back(&Producer::produceItems, producer);
  }

  // Create dispatcher queues for SPORTS, NEWS, and WEATHER
  BoundedQueue<std::string> sportsQueue(config.coEditorQueueSize);
  BoundedQueue<std::string> newsQueue(config.coEditorQueueSize);
  BoundedQueue<std::string> weatherQueue(config.coEditorQueueSize);

  // Create Dispatcher
  Dispatcher dispatcher(&sportsQueue, &newsQueue, &weatherQueue, &producers, producers.size());

  // Start Dispatcher thread
  std::thread dispatcherThread(&Dispatcher::dispatchMessages, &dispatcher);

  // Create CoEditors for each type
  BoundedQueue<std::string> sharedQueue(config.coEditorQueueSize); // Shared queue for ScreenManager
  CoEditor sportsEditor("SPORTS", &sportsQueue, &sharedQueue);
  CoEditor newsEditor("NEWS", &newsQueue, &sharedQueue);
  CoEditor weatherEditor("WEATHER", &weatherQueue, &sharedQueue);

  // Start CoEditor threads
  std::thread sportsThread(&CoEditor::edit, &sportsEditor);
  std::thread newsThread(&CoEditor::edit, &newsEditor);
  std::thread weatherThread(&CoEditor::edit, &weatherEditor);

  // Create and start ScreenManager
  ScreenManager screenManager(&sharedQueue, 3); // Expecting "DONE" from 3 CoEditors
  std::thread screenThread(&ScreenManager::display, &screenManager);

  // Wait for all producer threads to finish
  for (auto &t : producerThreads) {
    if (t.joinable()) {
      t.join();
    }
  }

  // Wait for Dispatcher to finish
  if (dispatcherThread.joinable()) {
    dispatcherThread.join();
  }

  // Wait for CoEditor threads to finish
  if (sportsThread.joinable())
    sportsThread.join();
  if (newsThread.joinable())
    newsThread.join();
  if (weatherThread.joinable())
    weatherThread.join();

  // Wait for ScreenManager thread to finish
  if (screenThread.joinable())
    screenThread.join();

  // Clean up producers
  for (auto &producer : producers) {
    delete producer;
  }

  return 0;
}
