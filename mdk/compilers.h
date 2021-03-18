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

#endif //VERTICES_PLATFORMS_COMPILERS_H
