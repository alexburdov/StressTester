/**
 *
 * Общий пакет
 *
 * Created by Aleksandr Burdovitsin on 03.12.2025.
**/
#ifndef HTTPSTRESSCLIENTC_COMMON_H
#define HTTPSTRESSCLIENTC_COMMON_H


#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_HOW_MANY_THREADS 500
#define DEFAULT_NUMBER_OF_CYCLES 1000000
#define DEFAULT_ADD_THREAD_BATCH_SIZE 50
#define DEFAULT_ADD_THREAD_TIME_IN_SECONDS 5
#define DEFAULT_MAX_ADD_COUNT 100

#define DEFAULT_FAILURE_THRESHOLD 10
#define DEFAULT_FAILURE_THRESHOLD_USE true

#define DEFAULT_BATCH_SIZE 20

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8080
#define DEFAULT_PATH "/echo"
#define DEFAULT_EXPECTED_RESPONSE "Hello World!"

#define DEFAULT_TIMEOUT_SECONDS 10

#define POSSIBLE_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

// #define DEBUG 0

/**
 * Удаление пробелов с левой стороны строки
 *
 * @param str - строка для обработки
 * @return - обработанная строка
 */
char *ltrim(char *str);

/**
 * Удаление пробелов с правой стороны строки
 *
 * @param str - строка для обработки
 * @return - обработанная строка
 */
char *rtrim(char *str);

/**
 * Удаление пробелов с обеих сторон строки
 *
 * @param str  - строка для обработки
 * @return - обработанная строка
 */
char *trim(char *str);

/**
 * Функция генерации случайной строки
 *
 * @param length - длина необходимой строки
 * @return - Строка из случайных символов
 */
char *generateRandomString(int length);

#ifdef __linux__
/**
 * Получить текущее время в миллисекундах
 *
 * @return - текущее время
 */
uint64_t getNowInMs();
#endif

#ifdef _WIN32
/**
 * Получить разницу во времени в миллисекундах
 *
 * @param start - начало периода
 * @param end - конец периода
 * @return  - раница в милисекундах
 */
unsigned long timeDiff(clock_t start, clock_t end);
#endif

#endif //HTTPSTRESSCLIENTC_COMMON_H
