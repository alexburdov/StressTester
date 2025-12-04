//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/worker.h"

#ifdef _WIN32
void startWindows(Configuration *config) {
    printf("Starting ...\n");

    init_winsock();

    int numberThread = config->howManyThreadRun;
    printf("numberThread = %i\n", numberThread);

    HANDLE *hEvent = malloc(sizeof(HANDLE) * numberThread);
    HANDLE *hThread = malloc(sizeof(HANDLE) * numberThread);
    ThreadData *data = malloc(sizeof(ThreadData) * numberThread);

    for (int i = 0; i < numberThread; i++) {
        hEvent[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (hEvent[i] != nullptr) {
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
            // printf("WaitForSingleObject returned %ul\n", waitResult);
        }
    }


    for (int i = 0; i < numberThread; i++) {
        if (hEvent[i] != nullptr) {
            CloseHandle(hEvent[i]);
        }
    }
    for (int i = 0; i < numberThread; i++) {
        if (hThread[i] != nullptr) {
            CloseHandle(hThread[i]);
        }
    }
    free(hThread);
    free(hEvent);

    for (int i = 0; i < numberThread; i++) {
        printf("Thread ID = %i SUCCESS = %i FAILURE = %i First = %lu All = %lu Min = %lu [%i] Max = %lu [%i] \n",
               data[i].innerThreadId, data->success, data->failure,
               data[i].firstTime, data[i].allTime, data[i].minTime, data->minTimeStep, data[i].maxTime,
               data->maxTimeStep);
    }
    free(data);
}
#elifdef __linux__
void startLinux(Configuration config) {
}
#endif
