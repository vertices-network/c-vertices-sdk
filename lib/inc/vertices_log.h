//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H
#define VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H

#if (__unix__ || __APPLE__)

#include <printf.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL 4 // default log level is to print everything
#endif

#if LOG_LEVEL >= 4

#define LOG_DEBUG(...)                             \
 do {                                                   \
    printf("🟣 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                                \
    printf("\r\n");                                     \
 } while(0)                                             \

#else

#define LOG_DEBUG(...)

#endif

#if LOG_LEVEL >= 3

#define LOG_INFO(...)                                   \
 do {                                                   \
    printf("🟢 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#else

#define LOG_INFO(...)

#endif

#if LOG_LEVEL >= 2

#define LOG_WARNING(...)                                   \
 do {                                                   \
    printf("🟠 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#else

#define LOG_WARNING(...)

#endif

#if LOG_LEVEL >= 1

#define LOG_ERROR(...)                                   \
 do {                                                   \
    printf("🔴 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#else

#define LOG_ERROR(...)

#endif

#endif

#endif //VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H
