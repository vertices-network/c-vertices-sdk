//! @file
//!
//! Vertices Network
//! See License.txt for details
//!
//! Created by Cyril on 18/03/2021.

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

/*
 * General byte order swapping functions.
 */
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__)
#include <sys/endian.h>
#define XXH_swap64 bswap64
#define XXH_swap32 bswap32
#define XXH_swap16 bswap16
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define XXH_swap64 OSSwapInt64
#define XXH_swap32 OSSwapInt32
#define XXH_swap16 OSSwapInt16
#elif defined(WIN32)
#include <stdlib.h>
#define XXH_swap64 _byteswap_uin64
#define XXH_swap32 _byteswap_uin32
#define XXH_swap16 _byteswap_uin16
#elif defined(__linux__)
#include <byteswap.h>
#define XXH_swap64 bswap_64
#define XXH_swap32 bswap_32
#define XXH_swap16 bswap_16
#elif defined(__XTENSA__)
#include "endian.h"
#else
#error declare swapping function
#endif

#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) || \
    (defined(_BYTE_ORDER) && _BYTE_ORDER == _LITTLE_ENDIAN) || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)

// It's a big-endian target architecture

#define IS_BIG_ENDIAN 1
#define IS_LITTLE_ENDIAN 0

#ifndef htobe16
#define	htobe16(x)	((uint16_t)(x))
#endif

#ifndef htobe32
#define	htobe32(x)	((uint32_t)(x))
#endif

#ifndef htobe64
#define	htobe64(x)	((uint64_t)(x))
#endif
#ifndef htobe16
#define	htole16(x)	XXH_swap16((x))
#endif

#ifndef htole32
#define	htole32(x)	XXH_swap32((x))
#endif

#ifndef htole64
#define	htole64(x)	XXH_swap64((x))
#endif

#ifndef be16toh
#define	be16toh(x)	((uint16_t)(x))
#endif

#ifndef be32toh
#define	be32toh(x)	((uint32_t)(x))
#endif

#ifndef be64toh
#define	be64toh(x)	((uint64_t)(x))
#endif

#ifndef le16toh
#define	le16toh(x)	XXH_swap16((x))
#endif

#ifndef	le32toh
#define	le32toh(x)	XXH_swap32((x))
#endif

#ifndef le64toh
#define	le64toh(x)	XXH_swap64((x))
#endif

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    (defined(_BYTE_ORDER) && _BYTE_ORDER != _LITTLE_ENDIAN)  || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

// It's a little-endian target architecture

#define IS_BIG_ENDIAN 0
#define IS_LITTLE_ENDIAN 1

#ifndef htobe16
#define	htobe16(x)	XXH_swap16((x))
#endif

#ifndef htobe32
#define	htobe32(x)	XXH_swap32((x))
#endif

#ifndef htobe64
#define	htobe64(x)	XXH_swap64((x))
#endif

#ifndef htole16
#define	htole16(x)	((uint16_t)(x))
#endif

#ifndef htole32
#define	htole32(x)	((uint32_t)(x))
#endif

#ifndef htole64
#define	htole64(x)	((uint64_t)(x))
#endif

#ifndef be16toh
#define	be16toh(x)	XXH_swap16((x))
#endif

#ifndef be32toh
#define	be32toh(x)	XXH_swap32((x))
#endif

#ifndef be64toh
#define	be64toh(x)	XXH_swap64((x))
#endif

#ifndef le16toh
#define	le16toh(x)	((uint16_t)(x))
#endif

#ifndef le32toh
#define	le32toh(x)	((uint32_t)(x))
#endif

#ifndef le64toh
#define	le64toh(x)	((uint64_t)(x))
#endif

#else
#error "Unknown architecture with current compiler, consider adding a way to detect byte-order"
#endif

#define UNUSED_VARIABLE(X)  ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X) UNUSED_VARIABLE(X)

#endif //VERTICES_PLATFORMS_COMPILERS_H
