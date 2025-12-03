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

// Функция для получения IP-адреса хоста
int resolveHost(const char *hostname, struct sockaddr_in *addr) {
    struct hostent *host;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 0;
    }
#endif

    host = gethostbyname(hostname);
    if (host == NULL) {
        return 0;
    }

    addr->sin_family = AF_INET;
    addr->sin_port = htons(DEFAULT_PORT);
    memcpy(&addr->sin_addr, host->h_addr_list[0], host->h_length);

    return 1;
}

SOCKET getServerConnection(ThreadData *data) {
    SOCKET sockFd = INVALID_SOCKET;
    struct sockaddr_in serverAddress;

    // Получаем IP-адрес хоста
    if (!resolveHost(DEFAULT_HOST, &serverAddress)) {
        data->success = 0;
        snprintf(data->errorMsg, sizeof(*data->errorMsg), "Failed to resolve host: %s", DEFAULT_HOST);
        return sockFd;
    }

    // Создаем сокет
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == INVALID_SOCKET) {
        data->success = 0;
        snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to create socket");
        return sockFd;
    }

    // Устанавливаем таймаут
    if (!setSocketTimeout(sockFd, DEFAULT_TIMEOUT_SECONDS)) {
        data->success = 0;
        snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to set socket timeout");
        return sockFd;
    }

    // Подключаемся к серверу
    if (connect(sockFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        data->success = 0;
        snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to connect to %s:%d", DEFAULT_HOST, DEFAULT_PORT);
    }
    return sockFd;
}

/**
 * Выполнить шаг программы
 *
 * @param data - ThreadData
 */
void runStep(ThreadData *data) {
    char request[512];
    char response[2048];

    SOCKET sockfd = getServerConnection(data);

    // Строим HTTP запрос
    buildHttpRequest(request, sizeof(request), DEFAULT_HOST, DEFAULT_PATH);

    // Отправляем запрос
    ssize_t bytesSent;

#ifdef _WIN32
    bytesSent = send(sockfd, request, (int) strlen(request), 0);
#endif
#ifdef __linux__
    bytesSent = send(sockfd, request, strlen(request), 0);
#endif

    if (bytesSent == SOCKET_ERROR) {
        data->success = 0;
        snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to send request");
        if (sockfd != SOCKET_ERROR) {
            closesocket(sockfd);
        }
        return;
    }

    // Принимаем ответ
    ssize_t totalReceived = 0;
    ssize_t bytesReceived;

    while (1) {
#ifdef _WIN32
        bytesReceived = recv(sockfd, response + totalReceived, (int) (sizeof(response) - totalReceived - 1), 0);
#endif
#ifdef __linux__
        bytesReceived = recv(sockfd, response + totalReceived, sizeof(response) - totalReceived - 1, 0);
#endif

        if (bytesReceived > 0) {
            totalReceived += bytesReceived;
            if (totalReceived >= sizeof(response) - 1) {
                break;
            }
        } else if (bytesReceived == 0) {
            // Соединение закрыто
            break;
        } else {
            // Ошибка приема
            data->success = 0;
            snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to receive response");
            if (sockfd != SOCKET_ERROR) {
                closesocket(sockfd);
            }
            return;
        }
    }
    // printf("%i == %lu \n", i, threadId);
    //
    // Sleep(rand() / 10 + rand());
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

    data->allTime = -1L;
    data->minTime = 120 * 3600 * 1000;
    data->maxTime = -1L;
    data->minTimeStep = -1;
    data->maxTimeStep = -1;

    clock_t startTime, endTime, startStepTime, endStepTime;

    // runStep(data);
    char request[512];
    char response[2048];

    SOCKET sockfd = getServerConnection(data);
    // Строим HTTP запрос
    buildHttpRequest(request, sizeof(request), DEFAULT_HOST, DEFAULT_PATH);

    startTime = clock();
    HANDLE hEvent = data->handleEvent;
    DWORD threadId = GetCurrentThreadId();
    printf("Thread ID: %ul Inner thread Id %i\n", threadId, data->innerThreadId);

    for (int i = 0; i < data->loopNumber; i++) {
        startStepTime = clock();

        // Отправляем запрос
        ssize_t bytesSent;

#ifdef _WIN32
        bytesSent = send(sockfd, request, (int) strlen(request), 0);
#endif
#ifdef __linux__
        bytesSent = send(sockfd, request, strlen(request), 0);
#endif

        if (bytesSent == SOCKET_ERROR) {
            data->success = 0;
            snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to send request");
            // if (sockfd != SOCKET_ERROR) {
            //     closesocket(sockfd);
            // }
            // return 0;
        }

        // Принимаем ответ
        ssize_t totalReceived = 0;
        ssize_t bytesReceived;

        int isError = 0;

        while (1) {
#ifdef _WIN32
            bytesReceived = recv(sockfd, response + totalReceived, (int) (sizeof(response) - totalReceived - 1), 0);
#endif
#ifdef __linux__
            bytesReceived = recv(sockfd, response + totalReceived, sizeof(response) - totalReceived - 1, 0);
#endif

            if (bytesReceived > 0) {
                totalReceived += bytesReceived;
                if (totalReceived >= sizeof(response) - 1) {
                    break;
                }
            } else if (bytesReceived == 0) {
                printf("%i Response : %s \n", i, response);
                break;
            } else {
                // Ошибка приема
                data->success = 0;
                snprintf(data->errorMsg, sizeof(data->errorMsg), "Failed to receive response");
                isError = 1;
                break;
            }
        }

        endStepTime = clock();

        long diffTime = endStepTime - startStepTime;

        if (i == 0) {
            data->minTime = startStepTime;
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
        if (isError) {
            break;
        }
    }

    if (sockfd != SOCKET_ERROR) {
        closesocket(sockfd);
    }
    endTime = clock();
    data->allTime = (endTime - startTime); // / data->loopNumber;
    data->success = 1;

    SetEvent(hEvent);
    printf("Event signaled by thread. %ul %i\n", threadId, data->innerThreadId);

#ifdef _WIN32
    return 0;
#elifdef __linux__
    return NULL;
#endif
}
