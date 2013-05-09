/*
 * Copyright 2004, 2007 Freescale Semiconductor.
 * Copyright(c) 2003 Motorola Inc.
 */

#ifndef	__config_ethernet_H__
#define __config_ethernet_H__

#include "types.h"
#include <string.h>
#include <va_list.h>

//Prototype Function



void my_tsec_init();
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

/* TSEC and MDIO OFFSETS */
#ifdef CONFIG_TSECV2
#define CONFIG_SYS_TSEC1_OFFSET		0xB0000
#else
#define CONFIG_SYS_TSEC1_OFFSET		0x24000
#endif

#define TSEC_SIZE			0x01000
#define CONFIG_SYS_MDIO1_OFFSET		0x24000
#define MDIO_OFFSET			0x01000
#define TSEC_BASE_ADDR		(CONFIG_SYS_IMMR + CONFIG_SYS_TSEC1_OFFSET)
#define MDIO_BASE_ADDR		(CONFIG_SYS_IMMR + CONFIG_SYS_MDIO1_OFFSET)
#define CONFIG_SYS_MPC85xx_GPIO_ADDR (CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_GPIO_OFFSET)




#define CONFIG_TSEC_ENET		/* tsec ethernet support */

#if defined(CONFIG_TSEC_ENET)
#ifndef CONFIG_NET_MULTI
#define CONFIG_NET_MULTI	1
#endif
#define CONFIG_MII		1	/* MII PHY management */
#define CONFIG_MII_DEFAULT_TSEC	1	/* Allow unregistered phys */


  #define CONFIG_TSEC1	1
  #define CONFIG_TSEC1_NAME	"eTSEC1"
  #define CONFIG_TSEC2	1
  #define CONFIG_TSEC2_NAME	"eTSEC2"
  #define CONFIG_TSEC3	1
  #define CONFIG_TSEC3_NAME	"eTSEC3"




//MPC P2020 TSEC PHY _Address
  #define TSEC1_PHY_ADDR     1
  #define TSEC2_PHY_ADDR	 2
  #define TSEC3_PHY_ADDR	 3



//This P2020 RDB PHY Tsec Address
   /*
   #define TSEC1_PHY_ADDR		2
   #define TSEC2_PHY_ADDR		0
   #define TSEC3_PHY_ADDR		1
    */

 
  #define TSEC1_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)
  #define TSEC2_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)
  #define TSEC3_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)

  #define TSEC1_PHYIDX		0
  #define TSEC2_PHYIDX		0
 #define TSEC3_PHYIDX		0

/*Установка главного Ethernet */
  #define CONFIG_ETHPRIME		"eTSEC1"
//#define CONFIG_ETHPRIME		"eTSEC3"
 #define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */

#if defined(CONFIG_TSEC_ENET)
#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2
#endif


/* tsec */
typedef struct ccsr_tsec {
	u8	res1[16];
	u32	ievent;		/* IRQ Event */
	u32	imask;		/* IRQ Mask */
	u32	edis;		/* Error Disabled */
	u8	res2[4];
	u32	ecntrl;		/* Ethernet Control */
	u32	minflr;		/* Minimum Frame Len */
	u32	ptv;		/* Pause Time Value */
	u32	dmactrl;	/* DMA Control */
	u32	tbipa;		/* TBI PHY Addr */
	u8	res3[88];
	u32	fifo_tx_thr;		/* FIFO transmit threshold */
	u8	res4[8];
	u32	fifo_tx_starve;		/* FIFO transmit starve */
	u32	fifo_tx_starve_shutoff;	/* FIFO transmit starve shutoff */
	u8	res5[96];
	u32	tctrl;		/* TX Control */
	u32	tstat;		/* TX Status */
	u8	res6[4];
	u32	tbdlen;		/* TX Buffer Desc Data Len */
	u8	res7[16];
	u32	ctbptrh;	/* Current TX Buffer Desc Ptr High */
	u32	ctbptr;		/* Current TX Buffer Desc Ptr */
	u8	res8[88];
	u32	tbptrh;		/* TX Buffer Desc Ptr High */
	u32	tbptr;		/* TX Buffer Desc Ptr Low */
	u8	res9[120];
	u32	tbaseh;		/* TX Desc Base Addr High */
	u32	tbase;		/* TX Desc Base Addr */
	u8	res10[168];
	u32	ostbd;		/* Out-of-Sequence(OOS) TX Buffer Desc */
	u32	ostbdp;		/* OOS TX Data Buffer Ptr */
	u32	os32tbdp;	/* OOS 32 Bytes TX Data Buffer Ptr Low */
	u32	os32iptrh;	/* OOS 32 Bytes TX Insert Ptr High */
	u32	os32iptrl;	/* OOS 32 Bytes TX Insert Ptr Low */
	u32	os32tbdr;	/* OOS 32 Bytes TX Reserved */
	u32	os32iil;	/* OOS 32 Bytes TX Insert Idx/Len */
	u8	res11[52];
	u32	rctrl;		/* RX Control */
	u32	rstat;		/* RX Status */
	u8	res12[4];
	u32	rbdlen;		/* RxBD Data Len */
	u8	res13[16];
	u32	crbptrh;	/* Current RX Buffer Desc Ptr High */
	u32	crbptr;		/* Current RX Buffer Desc Ptr */
	u8	res14[24];
	u32	mrblr;		/* Maximum RX Buffer Len */
	u32	mrblr2r3;	/* Maximum RX Buffer Len R2R3 */
	u8	res15[56];
	u32	rbptrh;		/* RX Buffer Desc Ptr High 0 */
	u32	rbptr;		/* RX Buffer Desc Ptr */
	u32	rbptrh1;	/* RX Buffer Desc Ptr High 1 */
	u32	rbptrl1;	/* RX Buffer Desc Ptr Low 1 */
	u32	rbptrh2;	/* RX Buffer Desc Ptr High 2 */
	u32	rbptrl2;	/* RX Buffer Desc Ptr Low 2 */
	u32	rbptrh3;	/* RX Buffer Desc Ptr High 3 */
	u32	rbptrl3;	/* RX Buffer Desc Ptr Low 3 */
	u8	res16[96];
	u32	rbaseh;		/* RX Desc Base Addr High 0 */
	u32	rbase;		/* RX Desc Base Addr */
	u32	rbaseh1;	/* RX Desc Base Addr High 1 */
	u32	rbasel1;	/* RX Desc Base Addr Low 1 */
	u32	rbaseh2;	/* RX Desc Base Addr High 2 */
	u32	rbasel2;	/* RX Desc Base Addr Low 2 */
	u32	rbaseh3;	/* RX Desc Base Addr High 3 */
	u32	rbasel3;	/* RX Desc Base Addr Low 3 */
	u8	res17[224];
	u32	maccfg1;	/* MAC Configuration 1 */
	u32	maccfg2;	/* MAC Configuration 2 */
	u32	ipgifg;		/* Inter Packet Gap/Inter Frame Gap */
	u32	hafdup;		/* Half Duplex */
	u32	maxfrm;		/* Maximum Frame Len */
	u8	res18[12];
	u32	miimcfg;	/* MII Management Configuration */
	u32	miimcom;	/* MII Management Cmd */
	u32	miimadd;	/* MII Management Addr */
	u32	miimcon;	/* MII Management Control */
	u32	miimstat;	/* MII Management Status */
	u32	miimind;	/* MII Management Indicator */
	u8	res19[4];
	u32	ifstat;		/* Interface Status */
	u32	macstnaddr1;	/* Station Addr Part 1 */
	u32	macstnaddr2;	/* Station Addr Part 2 */
	u8	res20[312];
	u32	tr64;		/* TX & RX 64-byte Frame Counter */
	u32	tr127;		/* TX & RX 65-127 byte Frame Counter */
	u32	tr255;		/* TX & RX 128-255 byte Frame Counter */
	u32	tr511;		/* TX & RX 256-511 byte Frame Counter */
	u32	tr1k;		/* TX & RX 512-1023 byte Frame Counter */
	u32	trmax;		/* TX & RX 1024-1518 byte Frame Counter */
	u32	trmgv;		/* TX & RX 1519-1522 byte Good VLAN Frame */
	u32	rbyt;		/* RX Byte Counter */
	u32	rpkt;		/* RX Packet Counter */
	u32	rfcs;		/* RX FCS Error Counter */
	u32	rmca;		/* RX Multicast Packet Counter */
	u32	rbca;		/* RX Broadcast Packet Counter */
	u32	rxcf;		/* RX Control Frame Packet Counter */
	u32	rxpf;		/* RX Pause Frame Packet Counter */
	u32	rxuo;		/* RX Unknown OP Code Counter */
	u32	raln;		/* RX Alignment Error Counter */
	u32	rflr;		/* RX Frame Len Error Counter */
	u32	rcde;		/* RX Code Error Counter */
	u32	rcse;		/* RX Carrier Sense Error Counter */
	u32	rund;		/* RX Undersize Packet Counter */
	u32	rovr;		/* RX Oversize Packet Counter */
	u32	rfrg;		/* RX Fragments Counter */
	u32	rjbr;		/* RX Jabber Counter */
	u32	rdrp;		/* RX Drop Counter */
	u32	tbyt;		/* TX Byte Counter Counter */
	u32	tpkt;		/* TX Packet Counter */
	u32	tmca;		/* TX Multicast Packet Counter */
	u32	tbca;		/* TX Broadcast Packet Counter */
	u32	txpf;		/* TX Pause Control Frame Counter */
	u32	tdfr;		/* TX Deferral Packet Counter */
	u32	tedf;		/* TX Excessive Deferral Packet Counter */
	u32	tscl;		/* TX Single Collision Packet Counter */
	u32	tmcl;		/* TX Multiple Collision Packet Counter */
	u32	tlcl;		/* TX Late Collision Packet Counter */
	u32	txcl;		/* TX Excessive Collision Packet Counter */
	u32	tncl;		/* TX Total Collision Counter */
	u8	res21[4];
	u32	tdrp;		/* TX Drop Frame Counter */
	u32	tjbr;		/* TX Jabber Frame Counter */
	u32	tfcs;		/* TX FCS Error Counter */
	u32	txcf;		/* TX Control Frame Counter */
	u32	tovr;		/* TX Oversize Frame Counter */
	u32	tund;		/* TX Undersize Frame Counter */
	u32	tfrg;		/* TX Fragments Frame Counter */
	u32	car1;		/* Carry One */
	u32	car2;		/* Carry Two */
	u32	cam1;		/* Carry Mask One */
	u32	cam2;		/* Carry Mask Two */
	u8	res22[192];
	u32	iaddr0;		/* Indivdual addr 0 */
	u32	iaddr1;		/* Indivdual addr 1 */
	u32	iaddr2;		/* Indivdual addr 2 */
	u32	iaddr3;		/* Indivdual addr 3 */
	u32	iaddr4;		/* Indivdual addr 4 */
	u32	iaddr5;		/* Indivdual addr 5 */
	u32	iaddr6;		/* Indivdual addr 6 */
	u32	iaddr7;		/* Indivdual addr 7 */
	u8	res23[96];
	u32	gaddr0;		/* Global addr 0 */
	u32	gaddr1;		/* Global addr 1 */
	u32	gaddr2;		/* Global addr 2 */
	u32	gaddr3;		/* Global addr 3 */
	u32	gaddr4;		/* Global addr 4 */
	u32	gaddr5;		/* Global addr 5 */
	u32	gaddr6;		/* Global addr 6 */
	u32	gaddr7;		/* Global addr 7 */
	u8	res24[96];
	u32	pmd0;		/* Pattern Match Data */
	u8	res25[4];
	u32	pmask0;		/* Pattern Mask */
	u8	res26[4];
	u32	pcntrl0;	/* Pattern Match Control */
	u8	res27[4];
	u32	pattrb0;	/* Pattern Match Attrs */
	u32	pattrbeli0;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd1;		/* Pattern Match Data */
	u8	res28[4];
	u32	pmask1;		/* Pattern Mask */
	u8	res29[4];
	u32	pcntrl1;	/* Pattern Match Control */
	u8	res30[4];
	u32	pattrb1;	/* Pattern Match Attrs */
	u32	pattrbeli1;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd2;		/* Pattern Match Data */
	u8	res31[4];
	u32	pmask2;		/* Pattern Mask */
	u8	res32[4];
	u32	pcntrl2;	/* Pattern Match Control */
	u8	res33[4];
	u32	pattrb2;	/* Pattern Match Attrs */
	u32	pattrbeli2;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd3;		/* Pattern Match Data */
	u8	res34[4];
	u32	pmask3;		/* Pattern Mask */
	u8	res35[4];
	u32	pcntrl3;	/* Pattern Match Control */
	u8	res36[4];
	u32	pattrb3;	/* Pattern Match Attrs */
	u32	pattrbeli3;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd4;		/* Pattern Match Data */
	u8	res37[4];
	u32	pmask4;		/* Pattern Mask */
	u8	res38[4];
	u32	pcntrl4;	/* Pattern Match Control */
	u8	res39[4];
	u32	pattrb4;	/* Pattern Match Attrs */
	u32	pattrbeli4;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd5;		/* Pattern Match Data */
	u8	res40[4];
	u32	pmask5;		/* Pattern Mask */
	u8	res41[4];
	u32	pcntrl5;	/* Pattern Match Control */
	u8	res42[4];
	u32	pattrb5;	/* Pattern Match Attrs */
	u32	pattrbeli5;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd6;		/* Pattern Match Data */
	u8	res43[4];
	u32	pmask6;		/* Pattern Mask */
	u8	res44[4];
	u32	pcntrl6;	/* Pattern Match Control */
	u8	res45[4];
	u32	pattrb6;	/* Pattern Match Attrs */
	u32	pattrbeli6;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd7;		/* Pattern Match Data */
	u8	res46[4];
	u32	pmask7;		/* Pattern Mask */
	u8	res47[4];
	u32	pcntrl7;	/* Pattern Match Control */
	u8	res48[4];
	u32	pattrb7;	/* Pattern Match Attrs */
	u32	pattrbeli7;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd8;		/* Pattern Match Data */
	u8	res49[4];
	u32	pmask8;		/* Pattern Mask */
	u8	res50[4];
	u32	pcntrl8;	/* Pattern Match Control */
	u8	res51[4];
	u32	pattrb8;	/* Pattern Match Attrs */
	u32	pattrbeli8;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd9;		/* Pattern Match Data */
	u8	res52[4];
	u32	pmask9;		/* Pattern Mask */
	u8	res53[4];
	u32	pcntrl9;	/* Pattern Match Control */
	u8	res54[4];
	u32	pattrb9;	/* Pattern Match Attrs */
	u32	pattrbeli9;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd10;		/* Pattern Match Data */
	u8	res55[4];
	u32	pmask10;	/* Pattern Mask */
	u8	res56[4];
	u32	pcntrl10;	/* Pattern Match Control */
	u8	res57[4];
	u32	pattrb10;	/* Pattern Match Attrs */
	u32	pattrbeli10;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd11;		/* Pattern Match Data */
	u8	res58[4];
	u32	pmask11;	/* Pattern Mask */
	u8	res59[4];
	u32	pcntrl11;	/* Pattern Match Control */
	u8	res60[4];
	u32	pattrb11;	/* Pattern Match Attrs */
	u32	pattrbeli11;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd12;		/* Pattern Match Data */
	u8	res61[4];
	u32	pmask12;	/* Pattern Mask */
	u8	res62[4];
	u32	pcntrl12;	/* Pattern Match Control */
	u8	res63[4];
	u32	pattrb12;	/* Pattern Match Attrs */
	u32	pattrbeli12;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd13;		/* Pattern Match Data */
	u8	res64[4];
	u32	pmask13;	/* Pattern Mask */
	u8	res65[4];
	u32	pcntrl13;	/* Pattern Match Control */
	u8	res66[4];
	u32	pattrb13;	/* Pattern Match Attrs */
	u32	pattrbeli13;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd14;		/* Pattern Match Data */
	u8	res67[4];
	u32	pmask14;	/* Pattern Mask */
	u8	res68[4];
	u32	pcntrl14;	/* Pattern Match Control */
	u8	res69[4];
	u32	pattrb14;	/* Pattern Match Attrs */
	u32	pattrbeli14;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd15;		/* Pattern Match Data */
	u8	res70[4];
	u32	pmask15;	/* Pattern Mask */
	u8	res71[4];
	u32	pcntrl15;	/* Pattern Match Control */
	u8	res72[4];
	u32	pattrb15;	/* Pattern Match Attrs */
	u32	pattrbeli15;	/* Pattern Match Attrs Extract Len & Idx */
	u8	res73[248];
	u32	attr;		/* Attrs */
	u32	attreli;	/* Attrs Extract Len & Idx */
	u8	res74[1024];
} ccsr_tsec_t;

#endif	/* CONFIG_TSEC_ENET */
#endif	/* Config_ethernet_configuration file*/
