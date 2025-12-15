/**
 * Пакет HTTP клиента
 *
 * Created by Aleksandr Burdovitsin on 03.12.2025.
**/
#ifndef HTTPSTRESSCLIENTC_ENDPOINT_H
#define HTTPSTRESSCLIENTC_ENDPOINT_H

#define PARAMETERS_SIZE 256
#define REQUEST_SIZE 1024
#define RESPONSE_SIZE 2048
#define AFTER_CONNECT_SLEEP 10

#define REQUEST_PATTERN "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: HTTP-Client-C/1.0 (Cross-Platform)\r\nConnection: close\r\n\r\n"

#include "common.h"
#include <limits.h>
#include <stdio.h>
#include <time.h>

// Платформозависимые заголовки
#ifdef _WIN32

#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#elifdef __linux__
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>

#include <stdatomic.h>
#define InterlockedIncrement(ptr) atomic_fetch_add(ptr, 1) + 1
#define InterlockedDecrement(ptr) atomic_fetch_sub(ptr, 1) - 1

#define SOCKET int
#define SOCKET_ERROR 0
#define INVALID_SOCKET (-1)
#endif

enum failureCodes {
    SOCKET_FAILURE = 0,
    SEND_FAILURE = 1,
    RECEIVE_FAILURE = 2
};
#define FAILURE_CODES_SIZE 3


// Структура для передачи данных в поток
typedef struct {
    unsigned int innerThreadId;
    unsigned long maxIterationRun;

    struct sockaddr_in *serverAddress;

#ifdef _WIN32
    HANDLE handleEvent;
    DWORD processorId;
#endif

    unsigned long success;
#ifdef _WIN32
    LONG *globalFailureCount;
#elifdef __linux__
    long *globalFailureCount;
#endif
    unsigned long failureThreshold;

    unsigned long failure[FAILURE_CODES_SIZE];

    unsigned int allTime; // Не понятно как мерить среднее время - учитывать запуски потоков с ошибками или нет?
    unsigned int minTime;
    unsigned int maxTime;
    unsigned int firstTime;
    unsigned long minTimeStep;
    unsigned long maxTimeStep;

    char *requestParameter[PARAMETERS_SIZE];
    char *responseParameter[PARAMETERS_SIZE];
} ThreadData;

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
