//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H
#define VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H

#if (__unix__ || __APPLE__)

#include <printf.h>

#define LOG_DEBUG(...)                             \
 do {                                                   \
    printf("🟣 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                                \
    printf("\r\n");                                     \
 } while(0)                                             \

#define LOG_INFO(...)                                   \
 do {                                                   \
    printf("🟢 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#define LOG_WARNING(...)                                   \
 do {                                                   \
    printf("🟠 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#define LOG_ERROR(...)                                   \
 do {                                                   \
    printf("🔴 [%s:%d] ", __FILE__, __LINE__);       \
    printf(__VA_ARGS__);                            \
    printf("\r\n");                                 \
 } while(0)                                             \

#endif

#endif //VERTICES_EXAMPLES_UNIX_INC_VTC_LOG_H
