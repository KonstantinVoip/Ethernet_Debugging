#include <size_t.h>


#ifndef _PPC_TYPES_H
#define _PPC_TYPES_H


///перенесли из файла common.h
typedef unsigned char		uchar;
typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;



//#ifndef __ASSEMBLY__       //uncomment
typedef unsigned short umode_t;
typedef unsigned int  uint;
typedef unsigned long ulong;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;

/*                                              //uncomment 
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

typedef struct {
	__u32 u[4];
} __attribute__((aligned(16))) vector128;

//#ifdef __KERNEL__*/

 // These aren't exported outside the kernel to avoid name space clashes
 
 //typedef unsigned long  size_t;
 
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define BITS_PER_LONG 32

// DMA addresses are 32-bits wide 
typedef u32 dma_addr_t;
/*                                                                 //uncomment
#ifdef CONFIG_PHYS_64BIT
typedef unsigned long long phys_addr_t;
typedef unsigned long long phys_size_t;
#else
*/
typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;
/*                                                                 //uncomment
#endif 
#endif // __KERNEL__ //
#endif // __ASSEMBLY__//
*/
#endif
