/**
 *
 * Пакет конфигурации
 *
 * Created by Aleksandr Burdovitsin on 03.12.2025.
**/
#ifndef HTTPSTRESSCLIENTC_CONFIGURATION_H
#define HTTPSTRESSCLIENTC_CONFIGURATION_H

#include "common.h"

#include <psdk_inc/_ip_types.h>

#include "endpoint.h"

/**
 *  Структура конфигурации приложения
 */
typedef struct {
    struct sockaddr_in serverAddress; // Переменная для хранения адреса сервера при инициализации

    char* hostname; // Имя хоста сервера
    int port; // Порт сервера

    unsigned int howManyThreadRun;
    unsigned long maxIterationRun;

    uint8_t addThreadBatchSize;
    uint8_t addThreadTimeInSecond;
    uint8_t maxAddCount;

#ifdef _WIN32
    LONG globalFailureCount;
#endif

    bool isFailureThresholdUse;
    unsigned long failureThreshold;

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