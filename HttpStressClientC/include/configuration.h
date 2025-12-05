//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#ifndef HTTPSTRESSCLIENTC_CONFIGURATION_H
#define HTTPSTRESSCLIENTC_CONFIGURATION_H

#include <psdk_inc/_ip_types.h>

/**
 *  Структура конфигурации приложения
 */
typedef struct {
    struct sockaddr_in serverAddress;
    char* hostname;
    int port;

    unsigned int howManyThreadRun;
    unsigned long numberOfCycles;
} Configuration;

/**
 * Получить конфигурацию по умолчанию
 *
 * @return - конфигурация
 */
Configuration getDefaultConfiguration();

/**
 * Функция для получения IP-адреса хоста
 *
 * @param config  - конфигурация
 *
 *
 */
void initServerAdress(Configuration *config);

#endif //HTTPSTRESSCLIENTC_CONFIGURATION_H