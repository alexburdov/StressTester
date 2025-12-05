//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#ifndef HTTPSTRESSCLIENTC_ENDPOINT_H
#define HTTPSTRESSCLIENTC_ENDPOINT_H

//TO DO Remove
#define DEFAULT_NUM_THREADS 100
#define DEFAULT_MAX_LOOP 1000
#define DEFAULT_BATCH_SIZE 20
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8080
#define DEFAULT_PATH "/echo"
#define DEFAULT_EXPECTED_RESPONSE "Hello World!"
#define DEFAULT_TIMEOUT_SECONDS 10
//TO DO End Remove

#define REQUEST_PATTERN "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: HTTP-Client-C/1.0 (Cross-Platform)\r\nConnection: close\r\n\r\n"

#include "common.h"

// Платформозависимые заголовки
#ifdef _WIN32

#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#elifdef __linux__

#endif


// Структура для передачи данных в поток
typedef struct {
    unsigned int innerThreadId;
    unsigned long loopNumber;

    struct sockaddr_in *serverAddress;

#ifdef _WIN32
    HANDLE handleEvent;
    DWORD processorId;
#endif

    unsigned long success;
    unsigned long failure[2];

    unsigned int allTime; // Не понятно как мерить среднее время - учитывать запуски потоков с ошибками или нет?
    unsigned int minTime;
    unsigned int maxTime;
    unsigned int firstTime;
    unsigned long minTimeStep;
    unsigned long maxTimeStep;

    char *requestParameter[256];
    char *responseParameter[256];
} ThreadData;

/**
 * Функция для инициализации сети (только для Windows)
 *
 * @return
 */
#ifdef _WIN32
int init_winsock();
#endif

/**
 * Основная функция потока
 * @param arg - Параметры потока
 * @return
 */
#ifdef _WIN32
DWORD WINAPI httpEndpoint(LPVOID parameters);
#elifdef __linux__
void *httpEndpoint(void *parameters);
#endif

#endif //HTTPSTRESSCLIENTC_ENDPOINT_H
