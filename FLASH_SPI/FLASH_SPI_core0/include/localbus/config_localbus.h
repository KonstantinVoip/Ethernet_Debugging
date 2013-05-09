/*
 * Copyright 2004, 2007 Freescale Semiconductor.
 * Copyright(c) 2003 Motorola Inc.
 */

#ifndef	__config_localbus_H__
#define __config_localbus_H__

#include "types.h"
#include <string.h>
#include <va_list.h>

//Prototype Function


//void disable_tlb(u8 esel);
void my_localbus_init();
void	udelay	      (unsigned long);
int	vsprintf(char *buf, const char *fmt, va_list args);
int	sprintf(char * buf, const char *fmt, ...)__attribute__ ((format (__printf__, 2, 3)));
int	ctrlc (void);
ulong	simple_strtoul(const char *cp,char **endp,unsigned int base);
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

//int eth_register(struct eth_device* dev);/* Register network device */
//extern void eth_try_another(int first_restart);	/* Change the device */

//#define CONFIG_SYS_CCSRBAR	0xff700000      // CCSRBAR Default 
#define CONFIG_SYS_CCSRBAR	0xE0000000     //В соответсвии с файлом конфигурации CCSR перемещён -в 0xE000_0000
#define CONFIG_SYS_IMMR		CONFIG_SYS_CCSRBAR	/* PQII uses */
#define CONFIG_SYS_MPC85xx_GUTS_OFFSET		0xE0000
#define CONFIG_SYS_MPC85xx_GUTS_ADDR (CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_GUTS_OFFSET)
#define CONFIG_SYS_MPC85xx_GPIO_OFFSET		0xF000



#define CONFIG_SYS_FLASH_BASE		0xef000000	/* start of FLASH 16M */

#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_FLASH_BASE_PHYS	0xfef000000ull
#else
#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE
#endif

#define CONFIG_FLASH_BR_PRELIM	(BR_PHYS_ADDR(CONFIG_SYS_FLASH_BASE_PHYS) | \
					BR_PS_16 | BR_V)
#define CONFIG_FLASH_OR_PRELIM		0xff000ff7

#define CONFIG_SYS_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE_PHYS}
#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS 45 /* count down from 45/5: 9..1 */

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	128	/* sectors per device */
#undef	CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CONFIG_SYS_MONITOR_BASE	TEXT_BASE	/* start of monitor */




#endif	/* CONFIG_Local_Bus */
