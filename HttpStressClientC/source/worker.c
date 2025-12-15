//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/worker.h"

void printStatistics(unsigned int threadCount, ThreadData *main) {
    unsigned int allFailure = 0;
    unsigned int socketFailure = 0;
    unsigned int receiveFailure = 0;
    unsigned int sendFailure = 0;

    int numberFailureElement = sizeof(main[0].failure) / sizeof(main[0].failure[0]);
    for (int i = 0; i <= threadCount; i++) {
        unsigned int failure = 0;
        for (int j = 0; j < numberFailureElement; j++) {
            allFailure += main[i].failure[j];
            failure += main[i].failure[j];
        }
        socketFailure += main[i].failure[SOCKET_FAILURE];
        sendFailure += main[i].failure[SEND_FAILURE];
        receiveFailure += main[i].failure[RECEIVE_FAILURE];


        printf(
            "Thread ID = %i SUCCESS = %lu FAILURE = %u Socket failure %lu Send failure = %lu Receive failure = %lu First = %i All = %i Min = %i [%lu] Max = %i [%lu] \n",
            main[i].innerThreadId, main->success, failure, main[i].failure[SOCKET_FAILURE],
            main[i].failure[SEND_FAILURE], main[i].failure[RECEIVE_FAILURE],
            main[i].firstTime, main[i].allTime, main[i].minTime, main->minTimeStep, main[i].maxTime,
            main->maxTimeStep);
    }
    free(main);
    printf("All failures = %i Socket failure = %i Send failure = %i Receive failure = %i \n", allFailure, socketFailure,
           sendFailure, receiveFailure);
    printf("ALL RUN %i THREAD!", threadCount);
}

// Запуск на Windows
#ifdef _WIN32
void startWindows(Configuration *config) {
    printf("Starting ...\n");

    unsigned int mainThread = config->howManyThreadRun;
    printf("Number threads to RUN = %i\n", mainThread);
    unsigned int additionsThread = config->maxAddCount * config->addThreadBatchSize;
    unsigned int allThread = mainThread + additionsThread;

    HANDLE *hEvent = malloc(sizeof(HANDLE) * (allThread));
    HANDLE *hThread = malloc(sizeof(HANDLE) * (allThread));
    ThreadData *data = malloc(sizeof(ThreadData) * (allThread));
    if (data) {
        for (unsigned int i = 0; i < mainThread; i++) {
            hEvent[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            if (hEvent[i]) {
                data[i].innerThreadId = i;
                data[i].maxIterationRun = config->maxIterationRun;
                data[i].handleEvent = hEvent[i];
                data[i].serverAddress = &config->serverAddress;
                data[i].failureThreshold = config->failureThreshold;
#ifdef _WIN32
                data[i].globalFailureCount = &config->globalFailureCount;
#endif

                hThread[i] = CreateThread(nullptr, 0, httpEndpoint, &data[i], 0, nullptr);
            }
        }
        unsigned int threadCount = mainThread;
        if (config->isFailureThresholdUse) {
            while (threadCount < allThread && config->failureThreshold > config->globalFailureCount) {
                for (unsigned int i = 0; i < config->addThreadBatchSize; i++) {
                    if (threadCount++ < allThread) {
                        hEvent[threadCount] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                        if (hEvent[threadCount]) {
                            data[threadCount].innerThreadId = threadCount;
                            data[threadCount].maxIterationRun = config->maxIterationRun;
                            data[threadCount].handleEvent = hEvent[threadCount];
                            data[threadCount].serverAddress = &config->serverAddress;
                            data[threadCount].failureThreshold = config->failureThreshold;
#ifdef _WIN32
                            data[threadCount].globalFailureCount = &config->globalFailureCount;
#endif

                            hThread[i] = CreateThread(nullptr, 0, httpEndpoint, &data[threadCount], 0, nullptr);
                        }
                    } else {
                        break;
                    }
                }
                Sleep(config->addThreadTimeInSecond * 1000);
            }
        }

        for (int i = 0; i <= threadCount; i++) {
            if (hEvent[i]) {
                DWORD waitResult = WaitForSingleObject(hEvent[i], INFINITE);
#ifdef DEBUG
                printf("WaitForSingleObject returned %lu\n", waitResult);
#endif
                CloseHandle(hEvent[i]);
            }
            if (hThread[i]) {
                CloseHandle(hThread[i]);
            }
        }
        free(hThread);
        free(hEvent);

        printStatistics(threadCount, data);
    }
}

// Запуск на Linux
#elifdef __linux__
void startLinux(Configuration *config) {
    //TO DO
}
#endif
