//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/worker.h"

void printStatistics(unsigned int numberThread, ThreadData *data) {
    unsigned int allFailure = 0;
    unsigned int socketFailure = 0;
    unsigned int receiveFailure = 0;

    int numberFailureElement = sizeof(data[0].failure) / sizeof(data[0].failure[0]);
    for (int i = 0; i < numberThread; i++) {
        unsigned int failure = 0;
        for (int j = 0; j < numberFailureElement; j++) {
            allFailure += data[i].failure[j];
            failure += data[i].failure[j];
        }
        socketFailure += data->failure[0];
        receiveFailure += data[i].failure[1];

        printf(
            "Thread ID = %i SUCCESS = %lu FAILURE = %u Socket failure %lu Receive failure = %lu First = %i All = %i Min = %i [%lu] Max = %i [%lu] \n",
            data[i].innerThreadId, data->success, failure, data[i].failure[0], data[i].failure[1],
            data[i].firstTime, data[i].allTime, data[i].minTime, data->minTimeStep, data[i].maxTime,
            data->maxTimeStep);
    }
    free(data);
    printf("All failures = %i Socket failure = %i receiveFailure = %i \n", allFailure, socketFailure, receiveFailure);
}

#ifdef _WIN32

void startWindows(Configuration *config) {
    printf("Starting ...\n");

    init_winsock();

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

#elifdef __linux__
void startLinux(Configuration config) {
    //TO DO
}
#endif
