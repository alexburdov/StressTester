//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#ifndef HTTPSTRESSCLIENTC_ENDPOINT_H
#define HTTPSTRESSCLIENTC_ENDPOINT_H


#define DEFAULT_NUM_THREADS 100
#define DEFAULT_MAX_LOOP 1000
#define DEFAULT_BATCH_SIZE 20
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8080
#define DEFAULT_PATH "/echo"
#define DEFAULT_EXPECTED_RESPONSE "Hello World!"
#define DEFAULT_TIMEOUT_SECONDS 10


#define REQUEST_PATTERN "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: HTTP-Client-C/1.0 (Cross-Platform)\r\nConnection: close\r\n\r\n"

#include "common.h"

// Платформозависимые заголовки
#ifdef _WIN32

#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

// #define SOCKET_TYPE SOCKET
// #define SOCKET_ERROR_CODE INVALID_SOCKET
// #define SOCKET_ERROR_RESULT SOCKET_ERROR
// #define CLOSE_SOCKET closesocket
// #define THREAD_RETURN unsigned __stdcall
// #define THREAD_CREATE _beginthreadex
// #define THREAD_HANDLE HANDLE
// #define THREAD_WAIT WaitForMultipleObjects
// #define THREAD_CLOSE CloseHandle
// #define SLEEP(x) Sleep(x)

#elifdef __linux__

#endif


// Структура для передачи данных в поток
typedef struct {
    int innerThreadId;
    int loopNumber;
#ifdef _WIN32
    HANDLE handleEvent;
#endif

    int success;

    long allTime;
    long minTime;
    long maxTime;
    long firstTime;
    int minTimeStep;
    int maxTimeStep;

    char* requestParameter[128];
    char* responseParameter[128];

    char errorMsg[128];
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
