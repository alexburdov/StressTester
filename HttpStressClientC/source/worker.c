//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/worker.h"

void printStatistics(unsigned int numberThread, ThreadData *data) {
    unsigned int allFailure = 0;
    unsigned int socketFailure = 0;
    unsigned int receiveFailure = 0;
    unsigned int sendFailure = 0;

    int numberFailureElement = sizeof(data[0].failure) / sizeof(data[0].failure[0]);
    for (int i = 0; i < numberThread; i++) {
        unsigned int failure = 0;
        for (int j = 0; j < numberFailureElement; j++) {
            allFailure += data[i].failure[j];
            failure += data[i].failure[j];
        }
        socketFailure += data[i].failure[SOCKET_FAILURE];
        sendFailure += data[i].failure[SEND_FAILURE];
        receiveFailure += data[i].failure[RECEIVE_FAILURE];


        printf(
            "Thread ID = %i SUCCESS = %lu FAILURE = %u Socket failure %lu Send failure = %lu Receive failure = %lu First = %i All = %i Min = %i [%lu] Max = %i [%lu] \n",
            data[i].innerThreadId, data->success, failure, data[i].failure[SOCKET_FAILURE], data[i].failure[SEND_FAILURE], data[i].failure[RECEIVE_FAILURE],
            data[i].firstTime, data[i].allTime, data[i].minTime, data->minTimeStep, data[i].maxTime,
            data->maxTimeStep);
    }
    free(data);
    printf("All failures = %i Socket failure = %i Send failure = %i Receive failure = %i \n", allFailure, socketFailure, sendFailure, receiveFailure);
}

// Запуск на Windows
#ifdef _WIN32
void startWindows(Configuration *config) {
    printf("Starting ...\n");

    unsigned int numberThread = config->howManyThreadRun;
    printf("Number threads to RUN = %i\n", numberThread);

    HANDLE *hEvent = malloc(sizeof(HANDLE) * numberThread);
    HANDLE *hThread = malloc(sizeof(HANDLE) * numberThread);
    ThreadData *data = malloc(sizeof(ThreadData) * numberThread);
    if (data) {
        for (int i = 0; i < numberThread; i++) {
            hEvent[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            if (hEvent[i]) {
                data[i].innerThreadId = i;
                data[i].loopNumber = config->numberOfCycles;
                data[i].handleEvent = hEvent[i];
                data[i].serverAddress = &config->serverAddress;

                hThread[i] = CreateThread(nullptr, 0, httpEndpoint, &data[i], 0, nullptr);
            }
        }

        for (int i = 0; i < numberThread; i++) {
            if (hEvent[i]) {
                DWORD waitResult = WaitForSingleObject(hEvent[i], INFINITE);
                printf("WaitForSingleObject returned %lu\n", waitResult);
                CloseHandle(hEvent[i]);
            }
            if (hThread[i]) {
                CloseHandle(hThread[i]);
            }
        }
        free(hThread);
        free(hEvent);

        printStatistics(numberThread, data);
    }
}

// Запуск на Linux
#elifdef __linux__
void startLinux(Configuration config) {
    //TO DO
}
#endif
