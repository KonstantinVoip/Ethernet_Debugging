/* tuning.h - macro definitions used by WDT */

/*  Copyright 2004-2005 Freescale Semiconductor */

/*
modification history
--------------------
01a,01dec04,ahk written
*/
#ifndef tuning_h
#define tuning_h

#define MSR_ME				0x00001000
#define TCR_WIE				0x08000000
#define MSR_CE				0x00020000
#define TSR_ENW_WIS_WRS		0xf0000000

/* MSR */
//#define GET_MSR(lval)   asm volatile ("mfmsr %0" : "=r" (value))
#define GET_MSR(lval)   asm volatile ("mfmsr %0" : "=r" (lval))
#define SET_MSR(value)  asm volatile ("mtmsr %0" : :"=r" (value))

/* TCR */
#define GET_TCR(lval)   asm volatile ("mfspr %0, 340" : "=r" (lval))
#define SET_TCR(value)  asm volatile ("mtspr 340, %0" : : "r" (value))

/* TSR */
#define GET_TSR(lval)   asm volatile ("mfspr %0, 336" : "=r" (lval))
#define SET_TSR(value)  asm volatile ("mtspr 336, %0" : : "r" (value))

/* HID0 */
#define GET_HID0(lval)   asm volatile ("mfspr %0, 1008" : "=r" (lval))
#define SET_HID0(value)  asm volatile ("mtspr 1008, %0" : : "r" (value))

#endif tuning_h