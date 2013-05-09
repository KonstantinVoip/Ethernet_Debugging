/*
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.

 */

#include "config_ethernet.h"
#include "tsec.h"
#include "net.h"
#include "miiphy.h"
#include "immap_85xx.h"
#include "u-boot.h"

static bd_t *bis;

void my_tsec_init()
{ 
 int num = 0;
 char *tmp;
 unsigned int vscfw_addr;
 struct tsec_info_struct tsec_info[4]; 
 volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
 
   


 //Здесь идет заполнение структуры для tsec определение расположения регистров и адресов !!
//где находить трансивер физического уровня
#ifdef CONFIG_TSEC1
	SET_STD_TSEC_INFO(tsec_info[num], 1);
	num++;
#endif
#ifdef CONFIG_TSEC2
	SET_STD_TSEC_INFO(tsec_info[num], 2);
	num++;
#endif
#ifdef CONFIG_TSEC3
	SET_STD_TSEC_INFO(tsec_info[num], 3);
	if (!(gur->pordevsr & MPC85xx_PORDEVSR_SGMII3_DIS))
		tsec_info[num].flags |= TSEC_SGMII;
	num++;
#endif
	if (!num) {
		//printf("No TSECs initialized\n");
		//return 0;
	}
	
	
//#ifdef CONFIG_VSC7385_ENET
// If a VSC7385 microcode image is present, then upload it. 
//	if ((tmp = getenv ("vscfw_addr")) != NULL) {
//		vscfw_addr = simple_strtoul (tmp, NULL, 16);
//		printf("uploading VSC7385 microcode from %x\n", vscfw_addr);
//		if (vsc7385_upload_firmware((void *) vscfw_addr,
//					CONFIG_VSC7385_IMAGE_SIZE))
//			puts("Failure uploading VSC7385 microcode.\n");
//	} else
//		puts("No address specified for VSC7385 microcode.\n");
//#endif




tsec_eth_init(bis, tsec_info, num);

}