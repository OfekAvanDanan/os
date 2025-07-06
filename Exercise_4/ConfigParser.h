// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "Config.h"
#include <string>

// Function to parse the configuration file
bool parseConfigFile(const std::string &filePath, Config &config);

#endif // CONFIG_PARSER_H
