


//#define udelay(x);
//#define CONFIG_FSL_CORENET  0
/* Automatically generated - do not edit */
#define CONFIG_MK_P2020RDB 1
#define CONFIG_MK_SPIFLASH 1





#define CONFIG_BOOKE		1	/* BOOKE */
#define CONFIG_E500		1	/* BOOKE e500 family */
#define CONFIG_MPC85xx		1	/* MPC8540/60/55/41/48/P1020/P2020,etc*/
#define CONFIG_P2020      1
/*
 * eSPI - Enhanced SPI
 */
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SPANSION  1

#define CONFIG_HARD_SPI
#define CONFIG_FSL_ESPI          0

#define CONFIG_CMD_SF
#define CONFIG_SF_DEFAULT_SPEED 10000000
#define CONFIG_SF_DEFAULT_MODE 0
#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1ms ticks */

//#define CONFIG_SYS_CLK_FREQ	get_board_sys_clk(0) /*sysclk for P1_P2 RDB *

/* 
#include <configs/P1_P2_RDB.h>    uncomment  linux
 #include <asm\config.h>
*/
#include "asm\config.h"
