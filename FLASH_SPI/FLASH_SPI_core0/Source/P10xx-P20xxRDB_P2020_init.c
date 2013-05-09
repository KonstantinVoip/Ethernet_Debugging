
#pragma section code_type ".init"

#ifdef __cplusplus
extern "C" {
#endif

asm void __reset(void);
asm void usr_init();
extern void __start();
extern unsigned long gInterruptVectorTable;
extern unsigned long gInterruptVectorTableEnd;

#ifdef __cplusplus
}
#endif

#ifdef ROM_VERSION
#define REGISTERSNO		23

const unsigned int address[REGISTERSNO + 1] = {
	0,
	0xe0002110, //DDR_SDRAM_CFG
	0xe0002000, //CS0_BNDS
	0xe0002080, //CS0_CONFIG
	0xe0002104, //TIMING_CFG_0
	0xe0002108, //TIMING_CFG_1
	0xe000210C, //TIMING_CFG_2
	0xe0002100, //TIMING_CFG_3
	0xe0002114, //DDR_SDRAM_CFG_2
	0xe0002118, //DDR_SDRAM_MODE
	0xe000211C, //DDR_SDRAM_MODE_2
	0xe0002120, //DDR_SDRAM_MD_CNTL
	0xe0002124, //DDR_SDRAM_INTERVAL
	0xe0002128, //DDR_DATA_INIT
	0xe0002130, //DDR_SDRAM_CLK_CNTL
	0xe0002160, //TIMING_CFG_4
	0xe0002164, //TIMING_CFG_5
	0xe0002170, //DDR_ZQ_CNTL
	0xe0002174, //DDR_WRLVL_CNTL
	0xe0002e48, //ERR_INT_EN
	0xe0002e58, //DDR_ERR_SBE
	0xe0002b28, //DDR_CDR1
	0xe0002b2c, //DDR_CDR2
	//sleep 500ms
	0xe0002110	//DDR_SDRAM_CFG
	//sleep 500ms
};
//ddr 512MB, 533MHz
const unsigned int val_revA[REGISTERSNO + 1] = {
	0,
	0x43000008,
	0x0000001F,
	0x80014202,
	0x00260802,
	0x4C47A432,
	0x04984CCE,
	0x00010000,
	0x24400000,
	0x00040642,
	0x00000000,
	0x00000000,
	0x08200100,
	0xDEADBEEF,
	0x03800000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	//sleep 500ms
	0xC3000008
	//sleep 500ms
};

//ddr 1GB, 666MHz
const unsigned int val_revB[REGISTERSNO + 1] = {
	0,
	0x43000000,
	0x0000003F,
	0x80014202,
	0x55770802,
	0x5f599543,
	0x0fa074d1,
	0x00030000,
	0x24401000,
	0x00040852,
	0x00000000,
	0x00000000,
	0x0a280100,
	0xDEADBEEF,
	0x02800000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	//sleep 500ms
	0xC3000000
	//sleep 500ms
};

#endif

asm void __reset(void)
{
	//
	//   Enable machine check exceptions, SPE, debug interrupts
	//
	lis		r3, 0x0200
	ori		r3, r3, 0x1200
	mtmsr	r3
		
	b		__start
}

asm void usr_init()
{
	nofralloc

#ifdef ROM_VERSION

	// init mmu
	////////////////////
	
	// 1MB TLB1 entry 1: 0xE0000000 - 0xE00FFFFF; for CCSR cache inhibited
	lis		r5, 0x1001
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0500
	mtspr	625, r5

	lis		r5, 0xe000
	ori		r5, r5, 0x000a 	
	mtspr	626, r5

	lis		r5, 0xe000
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync

	// 16MB TLB1 entry 2 - Local Bus : 0xFF000000 - 0xFFFFFFFF
	lis		r5, 0x1002
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0700
	mtspr	625, r5

	lis		r5, 0xff00
	ori		r5, r5, 0x000a
	mtspr	626, r5

	lis		r5, 0xff00
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync
	
	
	// 1GB TLB1 entry 3: 0x80000000 - 0xBFFFFFFF; PEX 1/2, cache inhibited
	lis		r5, 0x1003
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0a00
	mtspr	625, r5

	lis		r5, 0x8000
	ori		r5, r5, 0x000a
	mtspr	626, r5

	lis		r5, 0x8000
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync

	// 256KB TLB1 entry 6: 0xEFC00000 - 0xEFC3FFFF; for PCI Express I/O, cache inhibited
	lis		r5, 0x1006
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0400
	mtspr	625, r5

	lis		r5, 0xefc0
	ori		r5, r5, 0x000a 	
	mtspr	626, r5

	lis		r5, 0xefc0
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync
	
	
	// 1MB TLB1 entry 7: 0xF8000000 - 0xF80FFFFF; for NAND, cache inhibited
	lis		r5, 0x1007
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0500
	mtspr	625, r5

	lis		r5, 0xf800
	ori		r5, r5, 0x000a
	mtspr	626, r5

	lis		r5, 0xf800
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync

	// check board revision GPIO11
	lis		r4, 0xff70
	ori		r4, r4, 0xfc08
	lwz		r5, 0(r4)
	lis		r4, 0x0010
	ori		r4, r4, 0x0000
	and		r5, r5, r4
	cmpli   cr0,r5,0
	bne		TLBDDR_RevB
	
	// revA
	// 256MB TLB1 entry 9: 0x00000000 - 0x0FFFFFFF; DDR
	lis		r5, 0x1009
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0900
	mtspr	625, r5

	lis		r5, 0x0000
	ori		r5, r5, 0x0000
	mtspr	626, r5

	lis		r5, 0x0000
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync

	// 256MB TLB1 entry 10: 0x00000000 - 0x0FFFFFFF; DDR
	lis		r5, 0x100a
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0900
	mtspr	625, r5

	lis		r5, 0x1000
	ori		r5, r5, 0x0000
	mtspr	626, r5

	lis		r5, 0x1000
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync
	b init_CCSRBAR

TLBDDR_RevB:
	// rev B
	// 1GB TLB1 entry 9: 0x00000000 - 0x3FFFFFFF; DDR
	lis		r5, 0x1009
	ori		r5, r5, 0x0000
	mtspr	624, r5

	lis		r5, 0xC000
	ori		r5, r5, 0x0a00
	mtspr	625, r5

	lis		r5, 0x0000
	ori		r5, r5, 0x0000
	mtspr	626, r5

	lis		r5, 0x0000
	ori		r5, r5, 0x003f
	mtspr	627, r5

	tlbwe
	msync
	isync

init_CCSRBAR:
	//////////////////////////////////
	// relocate CCSRBAR at 0xe0000000	
	//////////////////////////////////
	// read current value (forces all accesses to configuration space to complete)
	lis		r5, 0xff70
	ori		r5, r5, 0x0000
	lwz		r4, 0(r5)
	isync
	
	//Write the new value to CCSRBAR
	lis		r5, 0x000e
	ori		r5, r5, 0x0000
	lis		r4, 0xFF700000@ha
	stw		r5, 0xFF700000@l(r4)

	//Perform a load of flash address	
	lis		r5, 0xffff
	ori		r5, r5, 0xfffc
	lwz		r4, 0(r5)
	isync

	// read new CCSRBAR value
	lis		r5, 0xe000
	ori		r5, r5, 0x0000
	lwz		r4, 0(r5)
	isync

	/////////////////////////////////
	// LAW Initialization
	/////////////////////////////////
	// 	0xFF000000	0xFFFFFFFF	LAW0	Local Bus NOR FLASH - 16M
	lis		r5, 0x000f
	ori		r5, r5, 0xf000	
	lis		r4, 0xe0000c08@ha
	stw		r5, 0xe0000c08@l(r4)
	
	lis		r5, 0x8040
	ori		r5, r5, 0x0017
	lis		r4, 0xe0000c10@ha
	stw		r5, 0xe0000c10@l(r4)

	//	0xC0000000  0xDFFFFFFF  LAW1    PEX1 - 512M
	lis		r5, 0x000a			
	ori		r5, r5, 0x0000
	lis		r4, 0xe0000c28@ha
	stw		r5, 0xe0000c28@l(r4)
	
	lis		r5, 0x8020
	ori		r5, r5, 0x001c
	lis		r4, 0xe0000c30@ha
	stw		r5, 0xe0000c30@l(r4)

	//	0xEFC20000  0xEFC2FFFF  LAW2    PEX1 - 64K
	lis		r5, 0x000e			
	ori		r5, r5, 0xfc20
	lis		r4, 0xe0000c48@ha
	stw		r5, 0xe0000c48@l(r4)
	
	lis		r5, 0x8020
	ori		r5, r5, 0x000f
	lis		r4, 0xe0000c50@ha
	stw		r5, 0xe0000c50@l(r4)
	
	//	0xA0000000  0xBFFFFFFF  LAW3    PEX2 - 512M
	lis		r5, 0x0008			
	ori		r5, r5, 0x0000
	lis		r4, 0xe0000c68@ha
	stw		r5, 0xe0000c68@l(r4)
	
	lis		r5, 0x8010
	ori		r5, r5, 0x001c
	lis		r4, 0xe0000c70@ha
	stw		r5, 0xe0000c70@l(r4)

	//	0xEFC10000  0xEFC1FFFF  LAW4    PEX2 - 64K
	lis		r5, 0x000e			
	ori		r5, r5, 0xfc10
	lis		r4, 0xe0000c88@ha
	stw		r5, 0xe0000c88@l(r4)
	
	lis		r5, 0x8010
	ori		r5, r5, 0x000f
	lis		r4, 0xe0000c90@ha
	stw		r5, 0xe0000c90@l(r4)
	
	
	//	0xF8000000  0xF8000000  LAW5    Local Bus NAND FLASH 1M
	lis		r5, 0x000f		
	ori		r5, r5, 0x8000
	lis		r4, 0xe0000ca8@ha
	stw		r5, 0xe0000ca8@l(r4)
	
	lis		r5, 0x8040
	ori		r5, r5, 0x0013
	lis		r4, 0xe0000cb0@ha
	stw		r5, 0xe0000cb0@l(r4)
	
	// check board revision GPIO11
	lis		r4, 0xe000
	ori		r4, r4, 0xfc08
	lwz		r5, 0(r4)
	lis		r4, 0x0010
	ori		r4, r4, 0x0000
	and		r5, r5, r4
	cmpli   cr0,r5,0
	bne		LAW_RevB

	// revA
	//	0x00000000	0x1FFFFFFF	LAW11	DDRC - 512MB
	lis		r5, 0x0000		
	ori		r5, r5, 0x0000
	lis		r4, 0xe0000d68@ha
	stw		r5, 0xe0000d68@l(r4)
	
	lis		r5, 0x80f0
	ori		r5, r5, 0x001c
	lis		r4, 0xe0000d70@ha
	stw		r5, 0xe0000d70@l(r4)
	b	init_LBC

LAW_RevB:
	// revB
	//	0x00000000	0x3FFFFFFF	LAW11	DDRC - 1GB
	lis		r5, 0x0000		
	ori		r5, r5, 0x0000
	lis		r4, 0xe0000d68@ha
	stw		r5, 0xe0000d68@l(r4)
	
	lis		r5, 0x80f0
	ori		r5, r5, 0x001d
	lis		r4, 0xe0000d70@ha
	stw		r5, 0xe0000d70@l(r4)

init_LBC:
	// configure local bus memory controller

	// CS0 - NOR Flash
	lis		r5, 0xFF00
	ori		r5, r5, 0x1001
	lis		r4, 0xe0005000@ha
	stw		r5, 0xe0005000@l(r4)	// BR0
	
	lis		r5, 0xFF00
	ori		r5, r5, 0x0FF7
	lis		r4, 0xe0005004@ha
	stw		r5, 0xe0005004@l(r4)	// OR0
	
	// CS1 - Promjet
	lis		r5, 0xF800
	ori		r5, r5, 0x0C21
	lis		r4, 0xe0005008@ha
	stw		r5, 0xe0005008@l(r4)	// BR1
	
	lis		r5, 0xFFF8
	ori		r5, r5, 0x0396
	lis		r4, 0xe000500c@ha
	stw		r5, 0xe000500c@l(r4)	// OR1


	//LBCR  
	lis		r5, 0x4000
	ori		r5, r5, 0x0000
	lis		r4, 0xe00050d0@ha
	stw		r5, 0xe00050d0@l(r4)

	//LCRR  
	lis		r5, 0x8000
	ori		r5, r5, 0x0008
	lis		r4, 0xe00050d4@ha
	stw		r5, 0xe00050d4@l(r4)
	
	
	//DDR CONFIG
	//	r3 - tmp
	//	r4 - index
	//	r5 - ptr data
	//	r6 - ptr address
	//	r8 - tmp
	//	r9 - length of configuration array (REGISTERSNO)
	sub		r4, r4, r4
	lis		r3, 0xe000
	ori		r3, r3, 0xfc08
	lwz		r8, 0(r3)
	lis		r3, 0x0010
	ori		r3, r3, 0x0000
	and		r8, r8, r3
	cmpli   cr0,r8,0
	beq		board_RevA
	// board rev B
	lis		r5, val_revB@h
	ori		r5, r5, val_revB@l
	b initDDR
board_RevA:
	lis		r5, val_revA@h
	ori		r5, r5, val_revA@l
initDDR:
	lis		r6, address@h
	ori		r6, r6, address@l
	addi	r9, r4, REGISTERSNO

loopDDR:
	cmpli	cr0, r4, (REGISTERSNO - 1)
	bne		no_delay
	lis		r3, 0x0000
	ori		r3, r3, 0x0fff
	mtspr CTR,r3
wait_delay1:
    bc    16,0,wait_delay1  // Decrement CTR, then branch if CTR is not 0

no_delay:
	
	lwzu	r3, 4(r5)
	lwzu	r8, 4(r6)
	stw		r3, 0(r8)	//save to conntroller
	addi	r4, r4, 1
	cmpl	cr0, r9, r4
	bne		loopDDR
	
	lis		r3, 0x0000
	ori		r3, r3, 0x0fff
	mtspr CTR,r3
wait_delay2:
    bc    16,0,wait_delay2  // Decrement CTR, then branch if CTR is not 0
	//END DDR CONFIG
	
	// interrupt vectors initialization
	lis		r4, 0x0000
	lis		r5, 0x0000
	ori		r5, r5, 0x0000
	mtspr	IVPR, r5

	ori		r4, r5, 0x0100
	mtspr	IVOR0, r4
	ori		r4, r5, 0x0200
	mtspr	IVOR1, r4
	ori		r4, r5, 0x0300
	mtspr	IVOR2, r4
	ori		r4, r5, 0x0400
	mtspr	IVOR3, r4
	ori		r4, r5, 0x0500
	mtspr	IVOR4, r4
	ori		r4, r5, 0x0600
	mtspr	IVOR5, r4
	ori		r4, r5, 0x0700
	mtspr	IVOR6, r4
	ori		r4, r5, 0x0c00
	mtspr	IVOR8, r4
	ori		r4, r5, 0x0900
	mtspr	IVOR10, r4
	ori		r4, r5, 0x0f00
	mtspr	IVOR11, r4
	ori		r4, r5, 0x0b00
	mtspr	IVOR12, r4
	ori		r4, r5, 0x1100
	mtspr	IVOR13, r4
	ori		r4, r5, 0x1000
	mtspr	IVOR14, r4
	ori		r4, r5, 0x1500
	mtspr	IVOR15, r4
	ori		r4, r5, 0x1600
	mtspr	IVOR32, r4
	ori		r4, r5, 0x1700
	mtspr	IVOR33, r4
	ori		r4, r5, 0x1800
	mtspr	IVOR34, r4
	ori		r4, r5, 0x1900
	mtspr	IVOR35, r4

	// activate debug interrupt and enable SPU
	lis		r4, 0x0200
	ori		r4, r4, 0x1200
	mtmsr	r4

	// time base enable
	lis		r4, 0x0000
	ori		r4, r4, 0x4000
	mtspr	1008, r4	
	
	// NAND Flash settings
	lis		r5, 0x0000
	ori		r5, r5, 0xF020
	lis		r4, 0xE00050E0@ha
	stw		r5, 0xE00050E0@l(r4)


	//enable trap debug event in DBCR0
	mfspr	r5, 308
	oris	r5, r5, 0x4100
	ori		r5, r5, 0x0000
	mtspr	308, r5
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copy the exception vectors from ROM to RAM
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	lis			r3, gInterruptVectorTable@h
	ori			r3, r3, gInterruptVectorTable@l
	subi		r3,r3,0x0004
	
	lis			r4, gInterruptVectorTableEnd@h
	ori			r4, r4, gInterruptVectorTableEnd@l
	
	lis			r5, 0xFFFF
	ori			r5,r5,0xFFFC
	
loop:
	lwzu		r6, 4(r3)
	stwu		r6, 4(r5)
	
	cmpw		r3,r4
	blt			loop
#endif  

#ifdef CACHE_VERSION
	mfspr	r3,L1CSR0	/* enable d-cache */
	ori		r3, r3, 0x0001
	mtspr	L1CSR0,r3

	mfspr	r3,L1CSR1	/* enable i-cache */
	ori		r3, r3, 0x0001
	mtspr	L1CSR1,r3
	
	mfspr	r3,BUCSR 	/* enable branch prediction in BUCSR */
	ori		r3, r3, 0x0001
	mtspr	BUCSR,r3 

	isync		
	msync		
#endif
  
   blr 
}
