/////////////////////////
//	Project Stationery  //
//////////////////////////
//#include "spi.h"
//#include "linux\types.h"                                               
//#include "spi_flash_internal.h"
#include "asm\immap_85xx.h"
//#include "malloc.h"
//#include "config.h"
//# include <string.h>
//#include <stdarg.h>
//#include "common.h"
//#include "part.h"
//#include "flash.h"
//#include <image.h>
//Add malloc .h
//#include <time.h> 
//#include <assert.h>
//#include<stddef.h>
//#include <mman.h>
//#include "unistd.h"
//#include <windows.h>  nothing
//#include "fcntl.h"
//#include "unistd.h"
//#include "malloc.h"
//#include "spi_flash.h"
//#include "processor.h"
//#include "asm/ptrace.h"
//#include <time_api.h>
  #include "asm/io.h"
  #include <stdio.h>
 

  ///For Timer/////////

  #include "tuning.h"
  #include <math.h>
//#include "ppc_asm.tmpl"
//#include "malloc.h" 
//#include "alloc.h"
  
  
  //ƒополнительные утилиты
  //#include "poison.h"
  //#include "stddef.h"
   // #include "list.h"
  ///////!!!!!!!!!!!!!!!!!!!!!!!»нклуды дл€ Ethernet!!!!!!!!!!!!!!!!!!!!!////
      #include "config_ethernet.h"   //файл настройки дл€ ethernet
      #include "config_localbus.h"
  // #include "tsec.h"
 // #include "miiphy.h"
  // #include "net.h" 
  //#include "u-boot.h"
   // #include "eth.h"
  
//»нyклуды дл€ SPI  
/*#include "common.h"
  #include "spi_flash.h"
  #include "spiimage.h"*/
  //#include <stdio.h>

  //#include <generic.h>
   // #include "swab.h"
//#include "lgeneric.h"



 //¬от она велика€ правда :))) дл€ подачи первоначального ресета мы должны 
 //установить какие из портов GPIO будут у нас входами а какие выходами 
 //делаем мы это путем выставление 0 или 1 значение регистра GPIO_DIR
 //0 порт ->>signal input , 1 порт ->>signal output дл€ всех 15 GPIO
 //после этого запол€ем регистр GPIO_DAT и выставл€ем в нашем случае это
 //BOARD_PERI_RST_SET еденички там где они соответсвуют обозгаченным нами GPIO_DIR

/////////////////////////////–егистр gpdat
//нумераци€ //Ёто соответсвенно подаЄм сигнал на GPIO 6 и GPIO 14 √де у нас вис€т 
//VCS8221 и VSC8641 и это как раз 0x00020000 и 0x02000000
//биты с 16 по 31 ->>Reserved
//0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
//0 0 0 0 0 0 1 0 0 0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 

//“оже самое с выстовлением портов на вход и выход
///////////////////////////////////////MPC_P2020_GPIO_CONFIGURATION//////////////////////////////

 //GPIO set for P2020_mpc
 #define RMI1_RST   0x00010000            //gpio 13
 #define RMI2_RST   0x00020000            //gpio 14
 #define RMI3_RST   0x00040000            //gpio 15
 #define GPIO_DIR   0x00ff0000            //for p2020mpc  gpio 13,14,15
 #define BOARD_PERI_RST_SET	 RMI1_RST|RMI2_RST|RMI3_RST

///////////////////////////////////////RDB_P2020_GPIO_CONFIGURATION////////////////////////////// 
/*
 #ifdef RDB_P2020 
  // RESET  
  #define VSC7385_RST_SET		0x00080000
  //#define SLIC_RST_SET		0x00040000
  #define SGMII_PHY_RST_SET	0x00020000
  //#define PCIE_RST_SET		0x00010000
  #define RGMII_PHY_RST_SET	0x02000000
  #define USB2_PORT_OUT_EN	0x01000000
  //#define USB_RST_CLR		0x04000000
   //  GPDIR  0->signal_input  1->signal output 
  #define GPIO_DIR		0x060f0000
  #define BOARD_PERI_RST_SET	SGMII_PHY_RST_SET |RGMII_PHY_RST_SET|VSC7385_RST_SET	
#endif
*/
#ifndef CONFIG_WD_PERIOD
# define CONFIG_WD_PERIOD	(10 * 1000 * 1000)	/* 10 seconds default*/
#endif





#define DEBUG
asm void  get_lticks();
asm void  wait_lticks(unsigned long);

void WatchDogCreate (int delay, int FirstTimeout, int SecondTimeout);
void WatchDogStart  (void);
void WatchDogCancel  (void);
typedef void (IntHndlr)(long);
unsigned long usec2ticks(unsigned long usec);
extern unsigned long get_tbclk();
extern void InterruptHandler(long cause);
asm void system_call();
extern asm double dtime(void);



void Get_MPC_Clock(void);



//////////–азбираемс€ с системными CLOCK//////////////////
#define SYSCLK_66	66666666  //Input main clock =66.667mhz
#define sysClkTicksPerSecond  100	  /* default 60 ticks/second */

/* globals */

double start_time, end_time;



void Get_MPC_Clock(void)
{
#define GPIO_ADDR 0xE0000000
#define MPC85xx_GUTC  GPIO_ADDR+CONFIG_SYS_MPC85xx_GUTS_OFFSET //Ёто правильно	
#define MPC85xx_LBC_ADDR   GPIO_ADDR+CONFIG_SYS_MPC85xx_LBC_OFFSET 
#define LCRR_CLKDIV			0x0000001F
volatile ccsr_gur_t *gur = (void *)(MPC85xx_GUTC);
volatile ccsr_lbc_t *lbc = (void *)(MPC85xx_LBC_ADDR);

//ѕромежуточные значени€
uint l_plat_ratio;
//ѕараметры наших Clocks
uint l_freqProcessor0;
uint l_freqProcessor1;
uint l_freqSystemBus;
uint l_freqDDRBus;
uint l_freqLocalBus; 
uint l_half_freqSystemBus; 
uint l_e500_ratio_core0;
uint l_e500_ratio_core1;
uint l_lcrr_div;

unsigned long tbclk;	
unsigned long ticks;
unsigned long usec=2000;





	l_plat_ratio = (gur->porpllsr) & 0x0000003e;
	l_plat_ratio >>= 1;	
	l_freqSystemBus=l_plat_ratio*SYSCLK_66;
	l_half_freqSystemBus=l_freqSystemBus/2;
	
	l_e500_ratio_core0 = ((gur->porpllsr) >> (16)) & 0x3f;
    l_e500_ratio_core1 = ((gur->porpllsr) >> (8 + 16)) & 0x3f;
     	
	
	l_freqProcessor0 = l_e500_ratio_core0 * l_half_freqSystemBus;
	l_freqProcessor1 = l_e500_ratio_core1 * l_half_freqSystemBus;
	
	
	l_lcrr_div = (lbc->lcrr & LCRR_CLKDIV)*2;
	l_freqLocalBus =l_freqSystemBus / l_lcrr_div;
	
	
	printf("l_lcrr_div=%x\n\r",l_lcrr_div);
	printf("LocalBusFreq=%d MHz\n\r",l_freqLocalBus/1000000);
	
	
	printf("l_e500_ratio_core0=%d\n\r",l_e500_ratio_core0);
	printf("l_e500_ratio_core1=%d\n\r",l_e500_ratio_core1);
	
    printf("e500_core0=%d MHZ\n\r",l_freqProcessor0/1000000);
	printf("e500_core1=%d MHZ\n\r",l_freqProcessor1/1000000);
	
	printf("plat_Ratio=%x\n\r",l_plat_ratio);
	printf("CCB_system_bus=%d Mhz\n\r",l_freqSystemBus/1000000);
	printf("DDR in SYNC MODE =%x MT/s data rate",l_freqSystemBus/1000000);
    

    /*
    * Get timebase clock frequency
    */

    tbclk= (l_freqSystemBus+ 4UL)/8UL;   //in HZ
  
    
    if (usec < 1000) {
		ticks = ((usec * (tbclk/1000)) + 500) / 1000;
	} else {
		ticks = ((usec / 10) * (tbclk / 100000));
	}
    
   
    
    asm ("li 3,0 ; mttbu 3 ; mttbl 3 ;");
    

}





void sysClkInt (void)
    {
 
 int       decCountVal		= 10000000;	/* default dec value */
 int       sysClkRunning    = 0;
 void*          sysClkRoutine		= NULL;
 int	sysClkArg		= NULL; 
  
    /*
     * The PowerPC decrementer doesn't reload the value by itself. The reload
     * need to be performed in this handler. The reload value should be
     * adjusted each time because the time spent between the exception
     * generation and the moment the register is reloaded changes.
     * By reading the decrementer we obtain the time spent between the
     * two events and can adjust the value to reload. This is done in assembly
     * language in order to minimize time spent between reading and writing
     * to the decrementer register in order to maximize system clock accuracy.
     */

__asm__ ("
    mfdec   	3	
sysClkIntLoop:
    add. 	3, %0, 3
    ble		sysClkIntLoop		/* check if we missed tick(s) */
    mtdec	3"
    :					/* no output operands  */
    : "r" (decCountVal)			/* input operand, %0 = decCountVal */
    : "3", "cc"/* side-effects: r3 clobbered, 'condition code' is clobbered */
    );

    /* execute the system clock routine */

    if (sysClkRunning && (sysClkRoutine != NULL))
	sysClkArg=5;

   }














//For Example external Functions

//extern void delayNticks(short N);

asm void system_call()
{
	nofralloc
	sc
	blr
}




/*
 * This function is intended for SHORT delays only.
 * It will overflow at around 10 seconds @ 400MHz,
 * or 20 seconds @ 200MHz.
 */
unsigned long usec2ticks(unsigned long usec)
{
	ulong ticks;

	if (usec < 1000) {
		ticks = ((usec * (get_tbclk()/1000)) + 500) / 1000;
	} else {
		ticks = ((usec / 10) * (get_tbclk() / 100000));
	}

	return (ticks);
}


//





//



/*
 * unsigned long long get_ticks(void);
 *
 * read timebase as "long long"
 */
asm void  get_lticks()
{	
get_lticks:
1:  mfspr    r3,269	//mftbu	r3
	mfspr	 r4,268
	mfspr    r5,269                //mftbu	r5
	cmp	0,r3,r5
	bne	1b
blr
}



 // Delay for a number of ticks
asm void wait_lticks(unsigned long)
{

wait_ticks:
	mflr	r8		// save link register 
	mr	r7, r3		// save tick count 
	bl	get_lticks	// Get start time 

	// Calculate end time 
	addc    r7, r4, r7	// Compute end time lower 
	addze	r6, r3		//     and end time upper 

	//WATCHDOG_RESET		// Trigger watchdog, if needed 
1:	bl	get_lticks   	// Get current time 
	subfc	r4, r4, r7	// Subtract current time from end time 
	subfe.	r3, r3, r6
	bge	1b		       // Loop until time expired 

	mtlr	r8		// restore link register 
	blr
}




/*
 * We implement the delay by converting the delay (the number of
 * microseconds to wait) into a number of time base ticks; then we
 * watch the time base until it has incremented by that amount.
 */
void ludelay(unsigned long usec)
{

ulong ticks, kv;

	do {
		kv = usec > CONFIG_WD_PERIOD ? CONFIG_WD_PERIOD : usec;
		ticks = usec2ticks (kv);
		wait_lticks (ticks);
		usec -= kv;
	} while(usec);


}











void main()
{     
  
   unsigned long clk;
    
  	int i=0;
	/*
	Because interrupt handlers contain shared code, each core needs to register its own 
	InterruptHandeler routine
	*/
//	register IntHndlr* isr = InterruptHandler;
///	asm
//	{
//		mtspr SPRG0, isr
//	}
	
//	printf("Welcome to CodeWarrior!\r\n");

//	system_call(); // generate a system call exception to demonstrate the ISR
	
	
 //get_lticks();
	
//	get_tbclk();
	
    printf("\r\n");
	printf("===============================================\n\r");
	printf("Testing watchdog timer functionalities...\r\n");
	printf("===============================================\n\r");
	
    
    
    
    
  // Get_MPC_Clock();
    
    
  //  #if 0
   // 
     my_tsec_init();
  
     
    //#endif 
    
  // while(1)
  // {
   	
   //	ludelay(5000000);
//	ludelay(10000000);
//	start_time = (dtime() * 8) / 266000000;
//	printf("SUPER =%f\n\r",start_time);
	
   //	i++;
  // }
  
  
  
  
  
  //  WatchDogCreate(20000, 0, 1); //20,000msec
 //	WatchDogCreate(10000, 0, 1); //10,000msec
//	WatchDogCreate(5046, 1, 0);//5046msec

	
	
		/* start the watchdog timer */

/*	
	
	
	WatchDogStart ();	
    start_time = (dtime() * 8) / 266000000;
		
	
	do
	{
	
		
	end_time =(dtime() * 8) / 266000000;
//	ludelay(1000);
	
	printf("time lapsed %f\n\r",end_time - start_time);
	
//	printf( "%0*.*f", start_time);
	   
	   //WatchDogStart ();
	 //start_time = (dtime() * 8) / 266000000;
	// printf( "%f||", start_time);
//	ludelay(100);
	 
	
	
	 
	
	i++;  // loop forever
	
	
		
	}
		
	while (end_time >30.000000); 
*/	

 
    //WatchDogCancel();
    //printf("STOP\n\r");
  
  
  //ќтладка работы наших TSEC!!!!!
  // volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
  // pgpio->gpdir=GPIO_DIR;
  // pgpio->gpdat=BOARD_PERI_RST_SET; 
   
   
   
  
   
  //clrsetbits_be32(&pgpio->gpdat, USB_RST_CLR, BOARD_PERI_RST_SET); 
   
   
   // my_localbus_init();
   
   
    //my_tsec_init(); 
    //my_localbus_init();
    

}


void WatchDogCreate 
     (
     int delay,
     int FirstTimeout,
     int SecondTimeout
     )
     {
     unsigned long	msr_value;
     unsigned long	tcr_value;
     unsigned long	tsr_value;
     unsigned long	hid0_value;
     float			temp;		   
     unsigned long  period;
     unsigned long  period_fraction;
    
     /* delay in 1msec units, example delay=25000msec=25sec */
     temp = (float)(delay * (266000 / 8) * (1 / 2.5) ); //2^(63-p+1)
     temp = (float)(log(temp) / log(2));  //63-p+1 (L.H.S)
     temp = (float)(63.0 - temp + 1.0); //p, which is used by the TCR
	     
     period = (unsigned long)temp;
     period_fraction = (unsigned long)((temp - period)*100);
          
     if(period_fraction < 50)
     {
     	period = floor(temp);
         	
     }
     else if (period_fraction > 50)
     {
     	period = ceil(temp);
     }
  
    // printf("Time for 1st (x1.0) timeout = %f sec \n\r", (delay/1000)*(1.0 / 2.5));
    // printf("Time for 1st (x1.5) timeout = %f sec \n\r", (delay/1000)*(1.5 / 2.5));
    // printf("Time for 2nd (x2.5) timeout = %d sec \n\r", delay/1000);
    // printf("\n\r");
     
     
     /*
     * The debugger *.cfg file set the TBEN bit 
     * in the HID0 register. First disable that otherwise
     * the timebase will roll over
     */
     GET_HID0(hid0_value);
     if(hid0_value & 0x00004000)
     	{
     	hid0_value &= ~0x00004000;
     	SET_HID0(hid0_value);
     	}
     	
	 
     /* setting tcr register */
     GET_TCR(tcr_value);
     tcr_value &= ~(0xC0000000 | 0x001E0000);   /* clear WP/WPEXT */
     tcr_value |= (period & 0x3) << 30 |
            (period & 0x3C) << 15;
	 SET_TCR(tcr_value);            

     /* setting hid0 register */
     GET_HID0(hid0_value);
     hid0_value &= ~0x00002000;        		 
     SET_HID0(hid0_value);
     asm("isync");

     /* 
      * Timeout exception options 
      * First timeout:
      * On a first timeout there is watchdog timer exception
      * is recorded in TSR[WIE] bit. If TCR[WIE] and MSR[CE] 
      * are set watchdog timer interrrupt (ivor 12) will be
      * taken
      * 
      * Second timeout:
      * In this case you choose not to take the interrupt on
      * a first timeout. Therefore there is no handler which 
      * could bring the state machine of the WD back to its
      * original state after the first timeout. So at some point
      * the second timeout will happen and the state machine for
      * WD will roll over. Depending upon the setting of TCR[WRC]
      * on a second timeout following will occur
      * -If TCR[WRC]= 0b00, nothing will happen
      * -If TCR[WRC]= 0b01, machine check exception will occur  
      * -If TCR[WRC]= 0b10, checkstop should occur but broken
      * In this example code we are interested in machine
      * check exception only
      */
     if ((FirstTimeout == 1) && (SecondTimeout == 0))
     	{
	     /* set the WIE bit of TCR */
	    GET_TCR(tcr_value);
	    tcr_value |= TCR_WIE;
	    SET_TCR(tcr_value);

	     /* set the CE bit of MSR */
		GET_MSR(msr_value);
		msr_value |= MSR_CE;
		SET_MSR(msr_value);
		
     	}

     if ((FirstTimeout == 0) && (SecondTimeout == 1))
     	{
		/* set WRC to 0b01 of TCR */
		GET_TCR(tcr_value);
     	tcr_value &= ~0x30000000;
     	tcr_value |= 0x10000000;	     
     	SET_TCR(tcr_value);

		/* set the ME bit of MSR */
		GET_MSR(msr_value);
		msr_value |= MSR_ME;
		SET_MSR(msr_value);

		/* set the EMCP bit of HID0 */
		GET_HID0(hid0_value);
		hid0_value |= 0x80000000;                      
     	SET_HID0(hid0_value);
     	asm("isync");	
	
     	}  
     	
     	#ifdef DEBUG
     	GET_TCR(tcr_value);
     	GET_MSR(msr_value);
     	GET_HID0(hid0_value);
     	GET_TSR(tsr_value);
     	printf("TCR  = 0x%8.8x\n\r", tcr_value);
     	printf("MSR  = 0x%8.8x\n\r", msr_value);
     	printf("HID0 = 0x%8.8x\n\r", hid0_value);
     	printf("TSR  = 0x%8.8x\n\r", tsr_value);
     	#endif  
     	
     	
     	return; 

     }



/************************************************************
*
* WatchDogStart - start the watchdog timer
*
* This routine starts the WDT, assuming that the WDT has been
* created using WatchDogCreate() prior to calling this routine. 
*
* RETURNS
*
* ERRNO:
*
*
*/

void WatchDogStart 
     (
     void
     )
     {
	 unsigned long hid0_value;
    
     /* setting hid0 register */
     GET_HID0(hid0_value);
     hid0_value |= 0x00004000;              /* enable the time base clock */
     
     /* reset the timebase */
	 asm("lis		r16,0x0");
	 asm("ori		r16,r16,0x0");
	 asm ("mtspr 	284,r16   ");
	 asm("mtspr 	285,r16   ");
	 asm("mtspr 	284,r16   ");
	 asm("msync	");

	 start_time = (dtime() * 8) / 266000000;	 
     
     
     SET_HID0(hid0_value);
     asm("isync");
     return;

     }


/************************************************************
*
* WatchDogCancel - cancel the watchdog timer
*
* This routine
*
* RETURNS
*
* ERRNO:
*
*
*/

#if 0

void WatchDogCancel (void)
{
	unsigned long tsr_value;
	unsigned long tcr_value;
	
	/* clear the status in TSR register */
    GET_TSR(tsr_value);
    tsr_value |= TSR_ENW_WIS_WRS;
    SET_TSR(tsr_value);     	
	
	/* clear the WPEXT and WP field of TCR register */
	GET_TCR(tcr_value);
    tcr_value &= ~(0xC0000000 | 0x001E0000);  
    SET_TCR(tcr_value);
	
	/* reset the timebase registers */
	asm("lis		r16,0x0");
	asm("ori		r16,r16,0x0");
	asm ("mttbl 	r16   ");
	asm("mttbu 	r16   ");
	asm("mttbl 	r16   ");
	asm("msync	");
	return;
	
}

#endif












