#include <stdio.h>
#include "tuning.h"

#ifdef __cplusplus
extern "C" {
#endif


void InterruptHandler(long cause);
asm unsigned int Get_ProcessorID();

#ifdef __cplusplus
}
#endif

#pragma e500v2_floatingpoint off

/* forward declarations */
void FreezeTimeBase (void);
void GetStateInfoWD (void);
void ResetTimeBase  (void);
extern asm double dtime(void);
extern double ticks_per_sec( unsigned int freq_MHz );

/* globals */
extern double start_time, end_time;

/* defines */

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


















#define clear_ce_msr \
			asm \
			{ \
			mfmsr	r8; \
			lis r7,0xfffd; \
			ori	r7,r7,0xffff; \
			and	r7,r7,r8; \
			mtmsr	r7; \
			isync; \
			}

#define clear_enw_wis_wrs_tsr \
			asm \
			{ \
			lis r7,0xd000; \
			ori	r7,r7,0x0000; \
			mttsr	r7; \
			isync; \
			}

#define clear_wis_tsr \
			asm \
			{ \
			lis r7,0x4000; \
			ori	r7,r7,0x0000; \
			mttsr	r7; \
			isync; \
			}			
			
#define clear_tcr \
			asm \
			{ \
			lis  r7,0x0000; \
			mtspr	340,r7; \
			isync; \
			}
			

#define read_tlb_entry_ccsrbar \
			asm \
			{    \
			lis  r7,0x1001; \
			mtspr	mas0,r7; \
			isync; \
			lis  	r7,0x4000; \
			ori	r7,r7,0x0008; \
			mtspr	mas2,r7; \
			isync ;\
			tlbre ;\
			isync ;\
			msync ;\
			}
/************************************************************
*
* InterruptHandler - handling the exceptions
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



			
void InterruptHandler
	(
	long exceptNumber
	)
	{
	volatile unsigned long 	*pReg;
	unsigned long 			tcr_value;
	unsigned long 			hid0_value;	

    /* pointer to MCPSUMR register */
	pReg = (volatile unsigned long *)0x400e0090;

	switch (exceptNumber)
		{
		case 0x100: 
		printf("EXEPTION 0x100\n\r");
		printf("Exc 0x%8.8x : critical interrupt...\r\n",exceptNumber);
		
		break;
		case 0x200:
			/*
			* check to see whether this is cause
			* by the watchdog second timeout
			*/
			printf("EXEPTION 0x200\n\r");	
			if(*pReg & 0x00000004)
				{
				printf("pReg & 0x00000004\n\r");
			
				end_time = (dtime() * 8)/266000000;
			
				/* freeze the time base */
				FreezeTimeBase ();
				printf("2nd tm\n\r");
				printf("-------\n\r");
  				printf("time lapsed %f\n\r",end_time - start_time); 								

				/* 
				* Print out the state of the state machine 
				* of the watchdog timer
				*/
				printf("Watchdog state before...\n\r");
				GetStateInfoWD();				
				
				clear_enw_wis_wrs_tsr
				*pReg |= 0x00000004; //clearing WRS of MCPSUMR*/
								
				 
				// Print out the state of the state machine 
				// of the watchdog timer
				//
				printf("Watchdog state after...\n\r");			
				GetStateInfoWD();
				printf("\n\r");
			
				
				// programming the tcr again so that we
			// can get second timeout again.Only WRC
				// field of TCR needs to be set to 0b01
				
				GET_TCR(tcr_value);
				tcr_value &= ~0x30000000;
     			tcr_value |= 0x10000000;	     
     			SET_TCR(tcr_value);
    			
				 //enable the timebase again 
     	    	GET_HID0(hid0_value);
     			hid0_value |= 0x00004000;              
 
     			// clear the timebase 
     			ResetTimeBase();
				
				SET_HID0(hid0_value);
     			start_time = (dtime() * 8) / 266000000;     		     			

				}

		break;	

		case 0x300: 
		printf("EXEPTION 0x300\n\r");
		printf("Exc 0x%x : data storage...\r\n",exceptNumber);
		break;	
		case 0x500:
		printf("EXEPTION 0x500\n\r");
		printf("Exc 0x%x : external interrupt...\r\n",exceptNumber);break;	
		case 0xB00: 
		
		printf("EXEPTION 0xB00\n\r");
		
			// freeze the time base 
			FreezeTimeBase ();
			printf("1st tm\n\r");
			printf("-------\n\r");			
		
			
			//Print out the state of the state machine 
			// of the watchdog timer
			
			printf("Watchdog state before...\n\r");
			GetStateInfoWD();
		
			
			// take the watchdog state machine to initial state 
			clear_enw_wis_wrs_tsr;			
		
			 
			// Print out the state of the state machine 
		// of the watchdog timer
			
			printf("Watchdog state after...\n\r");			
			GetStateInfoWD();
			printf("\n\r");

			// enable the timebase again 
     	    GET_HID0(hid0_value);
     		hid0_value |= 0x00004000;              
 
     		/// clear the timebase 
     		ResetTimeBase();

     		SET_HID0(hid0_value);
			
			
		break;	
		default: 
		printf("EXEPTION DEfault\n\r");
		printf("Exc: lumped...\r\n");break;
		}

	}

#endif


/************************************************************
*
* FreezeTimeBase - stops incrementing the timebase registers. 
*
* This routine clears the TBEN bit in the HID0 register. The 
* consequence of this is that the timebase registers are not
* supplied with the Platform/CCB clocks. In other words, the 
* timebase registers don't increment after every 8 CCB clock
* cycles. 
* 
*
* RETURNS: NONE
*
* ERRNO: NONE
*
*
*/
void FreezeTimeBase (void)
{
	unsigned long hid0_value;
	GET_HID0(hid0_value);
    hid0_value &= ~0x00004000;              
    SET_HID0(hid0_value);
	
}

/************************************************************
*
* GetStateInfoWD - provides the status of the WDT.  
*
* This routine provides the status of the WDT by reading the 
* TSR register. 
* 
*
* RETURNS: NONE
*
* ERRNO: NONE
*
*
*/
void GetStateInfoWD (void)
{
	unsigned long tsr_value;
	
	GET_TSR(tsr_value);
	if ((tsr_value & 0xc0000000) == 0xc0000000)
		{
		printf("TSR[ENW,WIS] = 0b11 \n\r");
		}

	if ((tsr_value & 0xc0000000) == 0x00000000)
		{
		printf("TSR[ENW,WIS] = 0b00 \n\r");
		}			
	if ((tsr_value & 0xc0000000) == 0x80000000)
		{
		printf("TSR[ENW,WIS] = 0b10 \n\r");
		}				
	
}

void ResetTimeBase (void)
{	 		
	asm("lis		r16,0x0");
	asm("ori		r16,r16,0x0");
	asm ("mtspr 	284,r16   ");
	asm("mtspr 	285,r16   ");
	asm("mtspr 	284,r16   ");
	asm("isync	");
	
}




















































void InterruptHandler(long cause)
{
	
	register long aMSR;
	unsigned long 			hid0_value;
	#pragma unused (aMSR) // Do not warn - aMSR used in asm code 
	
	asm
	{
		//If we are using SPE,  we need to make sure to enable the SPE bit in the  MSR
		#if defined(__PPCZen__) || defined(__PPCe500__) || defined(__PPCe500v2__)
			mfmsr  aMSR
			oris   aMSR,aMSR,0x0200
			mtmsr  aMSR
		#endif	
	}



 switch(cause)
 {
 	case 0xC00:
 	printf("Core #%d: system call exception handler\r\n", Get_ProcessorID());
 	break;
 	
 	
 	case 0xB00:
 	// freeze the time base 
			FreezeTimeBase ();
			printf("1st tm\n\r");
			printf("-------\n\r");			
		
			
			//Print out the state of the state machine 
			// of the watchdog timer
			
		 printf("Watchdog state before...\n\r");
			GetStateInfoWD();
		
			
			// take the watchdog state machine to initial state 
			clear_enw_wis_wrs_tsr;			
		
			 
			// Print out the state of the state machine 
		// of the watchdog timer
			
			printf("Watchdog state after...\n\r");			
			GetStateInfoWD();
			printf("\n\r");

			// enable the timebase again 
     	  GET_HID0(hid0_value);
     		hid0_value |= 0x00004000;              
 
     		/// clear the timebase 
     		ResetTimeBase();

     		SET_HID0(hid0_value);
			
//	WatchDogCancel();

 	break;
 	
 	default:
    //printf("Exc: lumped..%x.\r\n",cause);
 	break;



 	
 }







}

asm unsigned int Get_ProcessorID()
{
	mfspr r3, 286
}


















#pragma e500v2_floatingpoint reset
