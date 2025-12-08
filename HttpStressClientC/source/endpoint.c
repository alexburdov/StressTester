//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/endpoint.h"

#include <stdio.h>
#include <time.h>

// Функция для создания HTTP GET запроса
void buildHttpRequest(char *buffer, size_t size,
                      const char *host, const char *path) {
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
    ZeroMemory(&data->failure, sizeof(data->failure));
    data->allTime = 0;
    data->minTime = INT_MAX;
    data->maxTime = 0;
    data->minTimeStep = 0;
    data->maxTimeStep = 0;

    clock_t startTime, endTime, startStepTime, endStepTime;

    char request[REQUEST_SIZE];
    char response[RESPONSE_SIZE];

    startTime = clock();

    HANDLE handleEvent = data->handleEvent;
    DWORD threadId = GetCurrentThreadId();
    data->processorId = GetCurrentProcessorNumber();
    printf("Thread ID: %lu Inner thread Id %i Processor Number %lu \n", threadId, data->innerThreadId,
           data->processorId);

    unsigned long iterations = 0;

    //for (int i = 0; i < data->maxIterationRun; i++) {
    do {
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
#ifdef _WIN32
                InterlockedIncrement(data->globalFailureCount);
#endif
                data->failure[SOCKET_FAILURE]++;
                break;
            }

            // Принимаем ответ
            ssize_t totalReceived = 0;
            ssize_t bytesReceived;
            Sleep(AFTER_CONNECT_SLEEP);
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
                        printf("%i Response INTERRUPT: %s \n", i, response);
#endif
                        break;
                    }
                } else if (bytesReceived == 0) {
#ifdef DEBUG
                    printf("%i Response : %s \n", i, response);
#endif
                    break;
                } else {
                    // Ошибка приема
#ifdef _WIN32
                    InterlockedIncrement(data->globalFailureCount);
#endif
                    data->failure[RECEIVE_FAILURE]++;
                    perror("Failed to receive response");
                    break;
                }
            }

            endStepTime = clock();

            unsigned long diffTime = timeDiff(startStepTime, endStepTime);

            if (iterations == 0) {
                data->minTime = diffTime;
                data->maxTime = diffTime;
                data->firstTime = diffTime;
            }
            if (diffTime < data->minTime) {
                data->minTime = diffTime;
                data->minTimeStep = iterations;
            }
            if (diffTime > data->maxTime) {
                data->maxTime = diffTime;
                data->maxTimeStep = iterations;
            }
            data->success++;
            closesocket(sockFd);
        } else {
            perror("Failed to connect to port");
#ifdef _WIN32
            InterlockedIncrement(data->globalFailureCount);
#endif
            data->failure[SEND_FAILURE]++;
        }
        if (*data->globalFailureCount > 1000) {
            printf("Fails = %lu \n", *data->globalFailureCount);
            printf("STOP BY FAILED COUNT Thread ID: %lu \n", threadId);
            break;
        }
    } while (iterations++ < data->maxIterationRun);

    endTime = clock();
    data->allTime = timeDiff(startTime, endTime);
    if (handleEvent) {
        SetEvent(handleEvent);
    }
#ifdef DEBUG
    printf("Event signaled by thread. %lu %i\n", threadId, data->innerThreadId);
#endif

#ifdef _WIN32
    return 0;
#elifdef __linux__
    return NULL;
#endif
}
