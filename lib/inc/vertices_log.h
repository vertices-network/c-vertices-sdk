//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H
#define VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H


#include <printf.h>
#include "vertices_config.h"

#ifndef VTC_LOG_LEVEL
#define LOG_LEVEL 0 // if variable has not been set, print nothing
#endif

#if VTC_LOG_LEVEL >= 4

#define LOG_DEBUG(...)                             \
 do {                                                   \
    printf("🟣 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                                \
    printf("\r\n");                                     \
 } while(0)                                             \

#else

#define LOG_DEBUG(...)

#endif

#if VTC_LOG_LEVEL >= 3

#define LOG_INFO(...)                                   \
 do {                                                   \
    printf("🟢 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#else

#define LOG_INFO(...)

#endif

#if VTC_LOG_LEVEL >= 2

#define LOG_WARNING(...)                                   \
 do {                                                   \
    printf("🟠 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#else

#define LOG_WARNING(...)

#endif

#if VTC_LOG_LEVEL >= 1

#define LOG_ERROR(...)                                   \
 do {                                                   \
    printf("🔴 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#else

#define LOG_ERROR(...)

#endif

#endif //VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H
