/**
 * Пакет запуска
 *
 * Created by Aleksandr Burdovitsin on 03.12.2025.
**/
#ifndef HTTPSTRESSCLIENTC_WORKER_H
#define HTTPSTRESSCLIENTC_WORKER_H

#include <stdio.h>

#include "configuration.h"
#include "endpoint.h"

#ifdef _WIN32
/**
 * Запуск приложения на Windows
 *
 * @param config - Конфигурация
 */
void startWindows(Configuration *config);
#elifdef __linux__

/**
 * Запуск приложения на Linux
 *
 * @param config - Конфигурация
 */

void startLinux(Configuration *config);
#endif

#endif //HTTPSTRESSCLIENTC_WORKER_H
