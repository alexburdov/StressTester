//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/endpoint.h"

#include <stdio.h>
#include <time.h>

// Функция для инициализации сети (только для Windows)
#ifdef _WIN32
int init_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}
#endif

// Функция для создания HTTP GET запроса
void buildHttpRequest(char *buffer, size_t size,
                      const char *host, const char *path) {
    // size_t sz = strlen(REQUEST_PATTERN) + strlen(host) + strlen(path) - 3;
    snprintf(buffer, size, REQUEST_PATTERN, path, host);
}

// Функция для парсинга HTTP ответа
int parseHttpResponse(const char *response, char *body, size_t bodySize) {
    // Находим конец заголовков (двойной перевод строки)
    const char *bodyStart = strstr(response, "\r\n\r\n");
    if (!bodyStart) {
        bodyStart = strstr(response, "\n\n"); // Альтернативный вариант
        if (!bodyStart) {
            return 0;
        }
        bodyStart += 2;
    } else {
        bodyStart += 4; // Пропускаем \r\n\r\n
    }

    // Копируем тело ответа
    strncpy(body, bodyStart, bodySize - 1);
    body[bodySize - 1] = '\0';

    // Убираем пробелы и переводы строк из конца
    size_t len = strlen(body);
    while (len > 0 && (body[len - 1] == '\n' || body[len - 1] == '\r' ||
                       body[len - 1] == ' ' || body[len - 1] == '\t')) {
        body[--len] = '\0';
    }
    return 1;
}


// Функция для установки таймаута на сокет
int setSocketTimeout(SOCKET sockfd, int seconds) {
#ifdef _WIN32
    DWORD timeout = seconds * 1000; // Windows использует миллисекунды
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof(timeout)) != 0) {
        return 0;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char *) &timeout, sizeof(timeout)) != 0) {
        return 0;
    }
    return 1;
#endif
#ifdef __linux__
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        return 0;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        return 0;
    }
    return 1;
#endif
}

SOCKET setupServerSocket() {
    // Создаем сокет
    SOCKET sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == INVALID_SOCKET) {
        perror("Failed to create socket");
        return sockFd;
    }

    // Устанавливаем таймаут
    if (!setSocketTimeout(sockFd, DEFAULT_TIMEOUT_SECONDS)) {
        perror("Failed to set socket timeout");
    }

    return sockFd;
}

// Основная функция потока
#ifdef _WIN32
DWORD WINAPI httpEndpoint(LPVOID parameters)
#elifdef __linux__
void *httpEndpoint(void *parameters)
#endif
{
    ThreadData *data = parameters;
    data->success = 0;
    data->failure = 0;

    data->allTime = 0;
    data->minTime = INT_MAX;
    data->maxTime = 0;
    data->minTimeStep = 0;
    data->maxTimeStep = 0;

    clock_t startTime, endTime, startStepTime, endStepTime;

    char request[512];
    char response[2048];

    startTime = clock();

    HANDLE hEvent = data->handleEvent;
    DWORD threadId = GetCurrentThreadId();
    printf("Thread ID: %lu Inner thread Id %i\n", threadId, data->innerThreadId);

    for (int i = 0; i < data->loopNumber; i++) {
        SOCKET sockFd = setupServerSocket();
        // Подключаемся к серверу
        if (connect(sockFd, (struct sockaddr *) data->serverAddress, sizeof(struct sockaddr_in)) != SOCKET_ERROR) {
            // Строим HTTP запрос
            buildHttpRequest(request, sizeof(request), DEFAULT_HOST, DEFAULT_PATH);
            startStepTime = clock();

            // Отправляем запрос
            ssize_t bytesSent;

#ifdef _WIN32
            bytesSent = send(sockFd, request, (int) strlen(request), 0);
#endif
#ifdef __linux__
            bytesSent = send(sockFd, request, strlen(request), 0);
#endif

            if (bytesSent == SOCKET_ERROR) {
                perror("Failed to send request");
                data->failure++;
                break;
            }

            // Принимаем ответ
            ssize_t totalReceived = 0;
            ssize_t bytesReceived;
            Sleep(10);
            while (1) {
#ifdef _WIN32
                bytesReceived = recv(sockFd, response + totalReceived, (int) (sizeof(response) - totalReceived - 1), 0);
#endif
#ifdef __linux__
                bytesReceived = recv(sockFd, response + totalReceived, sizeof(response) - totalReceived - 1, 0);
#endif

                if (bytesReceived > 0) {
                    totalReceived += bytesReceived;
                    if (totalReceived >= sizeof(response) - 1) {
#ifdef DEBUG
                        //printf("%i Response INTERRUPT: %s \n", i, response);
#endif
                        break;
                    }
                } else if (bytesReceived == 0) {
#ifdef DEBUG
                    // printf("%i Response : %s \n", i, response);
#endif
                    break;
                } else {
                    // Ошибка приема
                    data->failure++;
                    perror("Failed to receive response");
                    break;
                }
            }

            endStepTime = clock();

            long diffTime = endStepTime - startStepTime;

            if (i == 0) {
                data->minTime = diffTime;
                data->maxTime = diffTime;
                data->firstTime = diffTime;
            }
            if (diffTime < data->minTime) {
                data->minTime = diffTime;
                data->minTimeStep = i;
            }
            if (diffTime > data->maxTime) {
                data->maxTime = diffTime;
                data->maxTimeStep = i;
            }
            data->success++;
            closesocket(sockFd);
        } else {
            perror("Failed to connect to port");
        }
    }


    endTime = clock();
    data->allTime = (endTime - startTime);

    SetEvent(hEvent);
    // printf("Event signaled by thread. %lu %i\n", threadId, data->innerThreadId);

#ifdef _WIN32
    return 0;
#elifdef __linux__
    return NULL;
#endif
}
