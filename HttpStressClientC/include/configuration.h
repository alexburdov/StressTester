//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#ifndef HTTPSTRESSCLIENTC_CONFIGURATION_H
#define HTTPSTRESSCLIENTC_CONFIGURATION_H

/**
 *  Структура конфигурации приложения
 */
typedef struct {
    int howThreadRun;
    long maxLoopNumber;
} Configuration;

Configuration getDefaultConfiguration();

#endif //HTTPSTRESSCLIENTC_CONFIGURATION_H