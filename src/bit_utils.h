/**********************************************************************************************************************
 * Common bit masks constants used by the project                                                                     *
 *********************************************************************************************************************/

#ifndef _BIT_UTILS_H_
#define _BIT_UTILS_H_

#include <inttypes.h>

#define EMPTY_BITMASK     UINT32_C(0x00000000)
#define BITMASK_1         UINT32_C(0x00000001)
#define BITMASK_2         UINT32_C(0x00000002)
#define BITMASK_3         UINT32_C(0x00000004)
#define BITMASK_4         UINT32_C(0x00000008)
#define BITMASK_5         UINT32_C(0x00000010)
#define BITMASK_6         UINT32_C(0x00000020)
#define BITMASK_7         UINT32_C(0x00000040)
#define BITMASK_8         UINT32_C(0x00000080)
#define BITMASK_9         UINT32_C(0x00000100)
#define BITMASK_10        UINT32_C(0x00000200)
#define BITMASK_11        UINT32_C(0x00000400)
#define BITMASK_12        UINT32_C(0x00000800)
#define BITMASK_13        UINT32_C(0x00001000)
#define BITMASK_14        UINT32_C(0x00002000)
#define BITMASK_15        UINT32_C(0x00004000)
#define BITMASK_16        UINT32_C(0x00008000)
#define BITMASK_17        UINT32_C(0x00010000)
#define BITMASK_18        UINT32_C(0x00020000)
#define BITMASK_19        UINT32_C(0x00040000)
#define BITMASK_20        UINT32_C(0x00080000)
#define BITMASK_21        UINT32_C(0x00100000)
#define BITMASK_22        UINT32_C(0x00200000)
#define BITMASK_23        UINT32_C(0x00400000)
#define BITMASK_24        UINT32_C(0x00800000)
#define BITMASK_25        UINT32_C(0x01000000)
#define BITMASK_26        UINT32_C(0x02000000)
#define BITMASK_27        UINT32_C(0x04000000)
#define BITMASK_28        UINT32_C(0x08000000)
#define BITMASK_29        UINT32_C(0x10000000)
#define BITMASK_30        UINT32_C(0x20000000)
#define BITMASK_31        UINT32_C(0x40000000)
#define BITMASK_32        UINT32_C(0x80000000)

#define BITMASK_4_TO_1    UINT32_C(0x0000000F)
#define BITMASK_8_TO_1    UINT32_C(0x000000FF)
#define BITMASK_24_TO_1   UINT32_C(0x00FFFFFF)
#define BITMASK_31_TO_1   UINT32_C(0x7FFFFFFF)
#define BITMASK_32_TO_13  UINT32_C(0xFFFFF000)
#define BITMASK_32_TO_17  UINT32_C(0xFFFF0000)
#define BITMASK_32_TO_29  UINT32_C(0xF0000000)

#define JMP_BITMASK       UINT32_C(0x00000000)
#define LD_BITMASK        UINT32_C(0x00000001)
#define PUT_BITMASK       UINT32_C(0x00000002)
#define SET_BITMASK       UINT32_C(0x00000003)
#define SETU_BITMASK      UINT32_C(0x00000004)
#define ADD_BITMASK       UINT32_C(0x00000005)
#define SUB_BITMASK       UINT32_C(0x00000006)
#define MUL_BITMASK       UINT32_C(0x00000007)
#define DIV_BITMASK       UINT32_C(0x00000008)
#define MOD_BITMASK       UINT32_C(0x00000009)
#define AND_BITMASK       UINT32_C(0x0000000A)
#define OR_BITMASK        UINT32_C(0x0000000B)
#define XOR_BITMASK       UINT32_C(0x0000000C)
#define BSR_BITMASK       UINT32_C(0x0000000D)
#define BSL_BITMASK       UINT32_C(0x0000000E)

#endif















