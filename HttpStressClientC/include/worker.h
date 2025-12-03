//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#ifndef HTTPSTRESSCLIENTC_WORKER_H
#define HTTPSTRESSCLIENTC_WORKER_H

#include <stdio.h>

#include "configuration.h"
#include "endpoint.h"

#ifdef _WIN32
void startWindows(Configuration* config);
#elifdef __linux__
void startLinux(Configuration^ config);
#endif

#endif //HTTPSTRESSCLIENTC_WORKER_H
