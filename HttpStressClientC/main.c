//
// Created by Aleksandr Burdovitsin on 01.12.2025.
//
#include <stdio.h>

#include "include/common.h"
#include "include/configuration.h"
#include "include/endpoint.h"
#include "include/worker.h"

int main(void) {
    Configuration configuration = getDefaultConfiguration();

#ifdef _WIN32
    startWindows(&configuration);
#elifdef __linux__
    startLinux(&configuration);
#endif

    return EXIT_SUCCESS;
}
