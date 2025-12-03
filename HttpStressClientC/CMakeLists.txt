cmake_minimum_required(VERSION 4.0)
project(HttpStressClientC C)

set(CMAKE_C_STANDARD 23)

add_executable(HttpStressClientC main.c
        source/common.c
        include/common.h
        source/endpoint.c
        include/endpoint.h
        source/configuration.c
        include/configuration.h
        source/worker.c
        include/worker.h)

if (WIN32)
    target_link_libraries(HttpStressClientC ws2_32)
else ()
    target_link_libraries(HttpStressClientC pthread rt)
endif ()
