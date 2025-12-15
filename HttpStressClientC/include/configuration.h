/**
 *
 * Пакет конфигурации
 *
 * Created by Aleksandr Burdovitsin on 03.12.2025.
**/
#ifndef HTTPSTRESSCLIENTC_CONFIGURATION_H
#define HTTPSTRESSCLIENTC_CONFIGURATION_H

#include "common.h"
#ifdef _WIN32
#include <psdk_inc/_ip_types.h>
#elifdef __linux__
#include <netinet/in.h>
#include <netdb.h>      // Основной заголовок
#include <sys/socket.h> // Для констант
#endif


#include "endpoint.h"

/**
 *  Структура конфигурации приложения
 */
typedef struct {
    struct sockaddr_in serverAddress; // Переменная для хранения адреса сервера при инициализации

    char *hostname; // Имя хоста сервера
    int port; // Порт сервера

    unsigned int howManyThreadRun; // Сколько потоков будут выполнятся
    unsigned long maxIterationRun; // Сколько итераций будет выполнено в потоке

    uint8_t addThreadBatchSize; // Сколько потоков будет добавляться при стратегии автоувеличения количества потоков
    uint8_t addThreadTimeInSecond; // Через сколько времени будет добавляться новые потоки при стратегии автоувеличения количества потоков
    uint8_t maxAddCount; // Сколько добавлений потоков будет выполнено при автоувеличения количества потоков

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
void initServerAddress(Configuration *config);

#endif //HTTPSTRESSCLIENTC_CONFIGURATION_H
