//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/configuration.h"

Configuration getDefaultConfiguration() {
    Configuration config;
    config.howThreadRun = 10000;
    config.maxLoopNumber = 10000;
    return config;
}