//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/worker.h"

#ifdef _WIN32
void startWindows(Configuration *config) {
    printf("Starting ...\n");

    init_winsock();

    int numberThread = config->howThreadRun;
    printf("numberThread = %i\n", numberThread);

    HANDLE *hEvent = malloc(sizeof(HANDLE) * numberThread);
    HANDLE *hThread = malloc(sizeof(HANDLE) * numberThread);
    ThreadData *data = malloc(sizeof(ThreadData) * numberThread);

    for (int i = 0; i < numberThread; i++) {
        hEvent[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (hEvent[i] != nullptr) {
            data[i].innerThreadId = i;
            data[i].loopNumber = config->maxLoopNumber;
            data[i].handleEvent = hEvent[i];

            hThread[i] = CreateThread(nullptr, 0, httpEndpoint, &data[i], 0, nullptr);
        }
    }

    for (int i = 0; i < numberThread; i++) {
        if (hEvent[i]) {
            DWORD waitResult = WaitForSingleObject(hEvent[i], INFINITE);
            printf("WaitForSingleObject returned %ul\n", waitResult);
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
        printf("Thread ID = %i First = %lu All = %lu Min = %lu [%i] Max = %lu [%i] \n", data[i].innerThreadId,
               data[i].firstTime, data[i].allTime, data[i].minTime, data->minTimeStep, data[i].maxTime,
               data->maxTimeStep);
    }
    free(data);
}

// int runMultiThread(HANDLE* hEvent, HANDLE* hThread, int howThread, long loopNumber) {
//     for (int i = 0; i < howThread; i++) {
//
//         if (hEvent[i] == NULL) {
//             char msg[256];
//             sprintf(msg, "Failed to create event handle. Error: %lu", GetLastError());
//             for (int j = 0; j < i; j++) {
//                 if (hEvent[j]) {
//                     CloseHandle(hEvent[j]);
//                 }
//             }
//             perror(msg);
//             free(hEvent);
//             return nullptr;
//         }
//     }
//
//     for (int i = 0; i < howThread; i++) {
//         hThread[i] = CreateThread(nullptr, 0, httpEndpoint, hEvent[i], 0, nullptr);
//         if (hThread[i] == NULL) {
//             char msg[256];
//             sprintf(msg, "Failed to create threads. Error: %lu", GetLastError());
//             perror(msg);
//             for (int j = 0; j < howThread; j++) {
//                 if (hEvent[j]) {
//                     CloseHandle(hEvent[j]);
//                 }
//             }
//             for (int j = 0; j < 1; j++) {
//                 if (hEvent[j]) {
//                     CloseHandle(hThread[j]);
//                 }
//             }
//             free(hThread);
//             free(hEvent);
//             return nullptr;
//         }
//     }
//     return hEvent;
// }
#elifdef __linux__
void startLinux(Configuration config) {
}
#endif
