//
// Created by Aleksandr Burdovitsin on 03.12.2025.
//

#include "../include/common.h"

// Удаление пробелов с левой стороны строки
char *ltrim(char *str) {
    while (isspace(*str)) str++;
    return str;
}

// Удаление пробелов с правой стороны строки
char *rtrim(char *str) {
    char *back = str + strlen(str);
    while (isspace(*--back)) {
    }
    *(back + 1) = '\0';
    return str;
}

// Удаление пробелов с обеих сторон строки
char *trim(char *str) {
    return rtrim(ltrim(str));
}

// Функция генерации случайной строки
char *generateRandomString(int length) {
    int possibleCharSize = strlen(POSSIBLE_CHARS);
    char *result = malloc(sizeof(char) * (length + 1));

    if (result != NULL) {
        for (int i = 0; i < length; i++) {
            result[i] = POSSIBLE_CHARS[rand() % possibleCharSize];
        }
        result[length] = '\0';
    }
    return result;
}

#ifdef __linux__
// Получить текущее время в миллисекундах
uint64_t getNowInMs() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return (uint64_t) now.tv_sec * 1000U + (uint64_t) now.tv_nsec / 1000000U;
}
#endif

// #ifdef _WIN32  TODO
// Получить разницу во времени в миллисекундах
unsigned long timeDiff(clock_t start, clock_t end) {
    unsigned long elapsed;
    if (end > start) {
        elapsed = end - start;
    } else {
        elapsed = start - end;
    }
    return elapsed;
}
// #endif  TODO
