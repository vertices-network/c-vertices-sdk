//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_PLATFORMS_COMPILERS_H
#define VERTICES_PLATFORMS_COMPILERS_H

#if defined ( __CC_ARM )

#ifndef __ASM
#define __ASM               __asm
#endif

#ifndef __INLINE
#define __INLINE            __inline
#endif

#ifndef __WEAK
#define __WEAK              __weak
#endif

#ifndef __ALIGN
#define __ALIGN(n)          __align(n)
#endif

#ifndef __PACKED
#define __PACKED            __packed
#endif

#define GET_SP()                __current_sp()

#elif defined   ( __GNUC__ )

#ifndef __ASM
#define __ASM               __asm
#endif

#ifndef __INLINE
#define __INLINE            inline
#endif

#ifndef __WEAK
#define __WEAK              __attribute__((weak))
#endif

#ifndef __ALIGN
#define __ALIGN(n)          __attribute__((aligned(n)))
#endif

#ifndef __PACKED
#define __PACKED           __attribute__((packed))
#endif

#endif

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
// It's a big-endian target architecture
#define IS_BIG_ENDIAN 1
#define IS_LITTLE_ENDIAN 0
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
// It's a little-endian target architecture
#define IS_BIG_ENDIAN 0
#define IS_LITTLE_ENDIAN 1
#else
#error "Unknown architecture with current compiler, consider adding a way to detect byte-order"
#endif

#define UNUSED_VARIABLE(X)  ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X) UNUSED_VARIABLE(X)

#endif //VERTICES_PLATFORMS_COMPILERS_H
