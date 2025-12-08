//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/worker.h"

void printStatistics(unsigned int numberThread, ThreadData *main, int addSize, ThreadData *add) {
    unsigned int allFailure = 0;
    unsigned int socketFailure = 0;
    unsigned int receiveFailure = 0;
    unsigned int sendFailure = 0;

    int numberFailureElement = sizeof(main[0].failure) / sizeof(main[0].failure[0]);
    for (int i = 0; i < numberThread; i++) {
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

    if (add && addSize > 0) {
        printf("\n\n ===================== ADD SEGMENT ==================\n\n");
        for (int i = 0; i < addSize; i++) {
            unsigned int failure = 0;
            for (int j = 0; j < numberFailureElement; j++) {
                allFailure += add[i].failure[j];
                failure += add[i].failure[j];
            }
            socketFailure += add[i].failure[SOCKET_FAILURE];
            sendFailure += add[i].failure[SEND_FAILURE];
            receiveFailure += add[i].failure[RECEIVE_FAILURE];


            printf(
                "Thread ID = %i SUCCESS = %lu FAILURE = %u Socket failure %lu Send failure = %lu Receive failure = %lu First = %i All = %i Min = %i [%lu] Max = %i [%lu] \n",
                add[i].innerThreadId, add->success, failure, add[i].failure[SOCKET_FAILURE],
                add[i].failure[SEND_FAILURE], add[i].failure[RECEIVE_FAILURE],
                add[i].firstTime, add[i].allTime, add[i].minTime, add->minTimeStep, add[i].maxTime,
                add->maxTimeStep);
        }
    }
    free(main);
    free(add);
    printf("All failures = %i Socket failure = %i Send failure = %i Receive failure = %i \n", allFailure, socketFailure,
           sendFailure, receiveFailure);
}

// Запуск на Windows
#ifdef _WIN32
void startWindows(Configuration *config) {
    printf("Starting ...\n");

    unsigned int numberThread = config->howManyThreadRun;
    printf("Number threads to RUN = %i\n", numberThread);

    HANDLE *mainEvent = malloc(sizeof(HANDLE) * numberThread);
    HANDLE *mainThread = malloc(sizeof(HANDLE) * numberThread);
    ThreadData *mainData = malloc(sizeof(ThreadData) * numberThread);

    HANDLE *addEvent = nullptr;
    HANDLE *addThread = nullptr;
    ThreadData *addData = nullptr;
    int addSize = 0;

    if (mainData) {
        for (int i = 0; i < numberThread; i++) {
            mainEvent[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            if (mainEvent[i]) {
                mainData[i].innerThreadId = i;
                mainData[i].maxIterationRun = config->maxIterationRun;
                mainData[i].handleEvent = mainEvent[i];
                mainData[i].serverAddress = &config->serverAddress;
#ifdef _WIN32
                mainData[i].globalFailureCount = &config->globalFailureCount;
#endif

                mainThread[i] = CreateThread(nullptr, 0, httpEndpoint, &mainData[i], 0, nullptr);
            }
        }
        if (config->isFailureThresholdUse) {
            // Добавочные потоки
            uint8_t addCount = 0;
            while (addCount < config->maxAddCount) {
                int oldAddSize = addSize;
                addSize += config->addThreadBatchSize;
                if (addThread) {
                    HANDLE *tmpEvent = malloc(sizeof(HANDLE) * addSize);
                    HANDLE *tmpThread = malloc(sizeof(HANDLE) * addSize);
                    ThreadData *tmpData = malloc(sizeof(ThreadData) * addSize);
                    memcpy(tmpEvent, addEvent, sizeof(HANDLE) * oldAddSize);
                    memcpy(tmpThread, addThread, sizeof(HANDLE) * oldAddSize);
                    memcpy(tmpData, addData, sizeof(ThreadData) * oldAddSize);
                    free(addEvent);
                    addEvent = tmpEvent;
                    free(addThread);
                    addThread = tmpThread;
                    free(tmpThread);
                    addData = tmpData;
                    free(tmpData);
                } else {
                    addEvent = malloc(sizeof(HANDLE) * addSize);
                    addThread = malloc(sizeof(HANDLE) * addSize);
                    addData = malloc(sizeof(ThreadData) * addSize);
                }
                for (int i = 0; i < config->addThreadBatchSize; i++) {
                    int step = oldAddSize + i;
                    addEvent[step] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                    addData[step].innerThreadId = numberThread + step;
                    addData[step].maxIterationRun = config->maxIterationRun;
                    addData[step].handleEvent = addEvent[step];
                    addData[step].serverAddress = &config->serverAddress;
#ifdef _WIN32
                    addData[step].globalFailureCount = &config->globalFailureCount;
#endif

                    addThread[step] = CreateThread(nullptr, 0, httpEndpoint, &addData[step], 0, nullptr);
                }
                Sleep(config->addThreadTimeInSecond * 1000);
            }
        }

        for (int i = 0; i < numberThread; i++) {
            if (mainEvent[i]) {
                DWORD waitResult = WaitForSingleObject(mainEvent[i], INFINITE);
                printf("WaitForSingleObject returned %lu\n", waitResult);
                CloseHandle(mainEvent[i]);
            }
            if (mainThread[i]) {
                CloseHandle(mainThread[i]);
            }
        }
        for (int i = 0; i < addSize; i++) {
            if (addThread[i]) {
                CloseHandle(addThread[i]);
                CloseHandle(addEvent[i]);
            }
        }

        free(mainThread);
        free(addThread);
        free(mainEvent);

        printStatistics(numberThread, mainData, addSize, addData);
        printf("\n\n GLOBAL = %lu \n\n", config->globalFailureCount);
    }
}

// Запуск на Linux
#elifdef __linux__
void startLinux(Configuration config) {
    //TO DO
}
#endif
