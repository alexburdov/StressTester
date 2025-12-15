//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock.h>
#endif

//  Получить конфигурацию по умолчанию
Configuration getDefaultConfiguration() {
    Configuration config;
#ifdef _WIN32
    config.globalFailureCount = 0;
#endif


    config.hostname = DEFAULT_HOST;
    config.port = DEFAULT_PORT;

    initServerAddress(&config);

    config.howManyThreadRun = DEFAULT_HOW_MANY_THREADS;
    config.maxIterationRun = DEFAULT_NUMBER_OF_CYCLES;

    config.isFailureThresholdUse = DEFAULT_FAILURE_THRESHOLD_USE;
    config.addThreadBatchSize = DEFAULT_ADD_THREAD_BATCH_SIZE;
    config.addThreadTimeInSecond =  DEFAULT_ADD_THREAD_TIME_IN_SECONDS;
    config.maxAddCount = DEFAULT_MAX_ADD_COUNT;

    config.failureThreshold = DEFAULT_FAILURE_THRESHOLD;;


    return config;
}

// Функция для получения IP-адреса хоста
void initServerAddress(Configuration *config) {
#ifdef _WIN32
    ZeroMemory(&config->serverAddress, sizeof(config->serverAddress));
#    elifdef __linux__
    memset(&config->serverAddress, 0, sizeof(config->serverAddress));
#endif


#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock.\n");
        exit(EXIT_FAILURE);
    }
#endif

    struct hostent *host = gethostbyname(config->hostname);
    if (host == NULL) {
        perror("Failed to resolve hostname.\n");
        exit(EXIT_FAILURE);
    }

    config->serverAddress.sin_family = AF_INET;
    config->serverAddress.sin_port = htons(config->port);
    memcpy(&config->serverAddress.sin_addr, host->h_addr_list[0], host->h_length);

    free(host);
}
