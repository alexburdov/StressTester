//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

//  Получить конфигурацию по умолчанию
Configuration getDefaultConfiguration() {
    Configuration config;

    config.hostname = "localhost";
    config.port = 8080;

    initServerAdress(&config);

    config.howManyThreadRun = 1000;
    config.numberOfCycles = 10000;

    return config;
}

// Функция для получения IP-адреса хоста
void initServerAdress(Configuration *config) {
    ZeroMemory(&config->serverAddress, sizeof(config->serverAddress));

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
