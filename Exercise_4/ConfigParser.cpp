// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include "ConfigParser.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool parseConfigFile(const std::string &filePath, Config &config) {
  std::ifstream infile(filePath);
  if (!infile.is_open()) {
    std::cerr << "Error: Unable to open configuration file: " << filePath << std::endl;
    return false;
  }

  std::string line;
  ProducerConfig currentProducer;
  bool parsingProducers = true;

  while (std::getline(infile, line)) {
    // Trim leading and trailing whitespace
    line.erase(0, line.find_first_not_of(" \t\r\n"));
    line.erase(line.find_last_not_of(" \t\r\n") + 1);

    if (line.empty()) {
      continue; // Skip empty lines
    }

    if (parsingProducers && line.find("PRODUCER") == 0) {
      // Parse producer ID
      std::istringstream iss(line);
      std::string keyword;
      iss >> keyword; // "PRODUCER"
      iss >> currentProducer.id;

      // Read the next line for number of products
      if (!std::getline(infile, line)) {
        std::cerr << "Error: Unexpected end of file after PRODUCER ID." << std::endl;
        return false;
      }
      currentProducer.numberOfProducts = std::stoi(line);

      // Read the next line for queue size
      if (!std::getline(infile, line)) {
        std::cerr << "Error: Unexpected end of file after number of products." << std::endl;
        return false;
      }
      size_t pos = line.find("queue size =");
      if (pos == std::string::npos) {
        std::cerr << "Error: Expected 'queue size =' but found: " << line << std::endl;
        return false;
      }
      currentProducer.queueSize = std::stoi(line.substr(pos + 12));

      // Add the producer configuration to the list
      config.producers.push_back(currentProducer);
    } else if (line.find("Co-Editor queue size =") == 0) {
      // Parse Co-Editor queue size
      size_t pos = line.find("=");
      if (pos == std::string::npos) {
        std::cerr << "Error: Expected '=' in Co-Editor queue size line." << std::endl;
        return false;
      }
      config.coEditorQueueSize = std::stoi(line.substr(pos + 1));
      parsingProducers = false; // No more producers after this
    } else {
      std::cerr << "Warning: Unrecognized line in config file: " << line << std::endl;
    }
  }

  infile.close();

  // Basic validation
  if (config.producers.empty()) {
    std::cerr << "Error: No producers found in the configuration file." << std::endl;
    return false;
  }
  if (config.coEditorQueueSize <= 0) {
    std::cerr << "Error: Invalid Co-Editor queue size." << std::endl;
    return false;
  }

  return true;
}
