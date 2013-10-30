//Глобальная структура spi_flash!!!!
 //static struct spi_flash *flash;
 //
//Прототипы функций нахождения min и max
/*
size_t min(size_t,unsigned int);
size_t max(size_t,unsigned int);

size_t min(size_t X,unsigned int Y)
{
 unsigned int __x;
 unsigned int __y;
 
  //typeof(X);
  __x=(unsigned int)X;
  __y=Y;

  return (__x < __y?__x : __y);
}

size_t max(size_t X,unsigned int Y)
{
 unsigned int __x;
 unsigned int __y;

 //Aeaeii io?ii ?oiau iu iiaee ii?aaaeyou eaeiai oeia ia?aiaiiay
 //typeof(X); 
  __x=(unsigned int)X;
  __y=Y;

  return (__x > __y? __x : __y);
	  
}
#define min(X, Y) ({ typeof (X) __x = (X), __y = (Y);(__x < __y) ? __x : __y; })
#define max(X, Y) ({ typeof (X) __x = (X), __y = (Y);(__x > __y) ? __x : __y; })
*/





/*
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
    
     // delay in 1msec units, example delay=25000msec=25sec 
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
  
     printf("Time for 1st (x1.0) timeout = %f sec \n\r", (delay/1000)*(1.0 / 2.5));
     printf("Time for 1st (x1.5) timeout = %f sec \n\r", (delay/1000)*(1.5 / 2.5));
     printf("Time for 2nd (x2.5) timeout = %d sec \n\r", delay/1000);
     printf("\n\r");
     
     
     
     // The debugger *.cfg file set the TBEN bit 
     // in the HID0 register. First disable that otherwise
     // the timebase will roll over
     
     GET_HID0(hid0_value);
     if(hid0_value & 0x00004000)
     	{
     	hid0_value &= ~0x00004000;
     	SET_HID0(hid0_value);
     	}
     	
	 
      //setting tcr register 
     GET_TCR(tcr_value);
     tcr_value &= ~(0xC0000000 | 0x001E0000);   // clear WP/WPEXT 
     tcr_value |= (period & 0x3) << 30 |
            (period & 0x3C) << 15;
	 SET_TCR(tcr_value);            

     // setting hid0 register 
     GET_HID0(hid0_value);
     hid0_value &= ~0x00002000;        		 
     SET_HID0(hid0_value);
     asm("isync");

      
       //Timeout exception options 
      // First timeout:
       //On a first timeout there is watchdog timer exception
       //is recorded in TSR[WIE] bit. If TCR[WIE] and MSR[CE] 
       //are set watchdog timer interrrupt (ivor 12) will be
       //taken
       
       //Second timeout:
       //In this case you choose not to take the interrupt on
       //a first timeout. Therefore there is no handler which 
       //could bring the state machine of the WD back to its
       //original state after the first timeout. So at some point
       //the second timeout will happen and the state machine for
       ///WD will roll over. Depending upon the setting of TCR[WRC]
       //on a second timeout following will occur
       //-If TCR[WRC]= 0b00, nothing will happen
       //-If TCR[WRC]= 0b01, machine check exception will occur  
       //-If TCR[WRC]= 0b10, checkstop should occur but broken
       //In this example code we are interested in machine
       //check exception only
      
     if ((FirstTimeout == 1) && (SecondTimeout == 0))
     	{
	     // set the WIE bit of TCR 
	    GET_TCR(tcr_value);
	    tcr_value |= TCR_WIE;
	    SET_TCR(tcr_value);

	     // set the CE bit of MSR 
		GET_MSR(msr_value);
		msr_value |= MSR_CE;
		SET_MSR(msr_value);
		
     	}

     if ((FirstTimeout == 0) && (SecondTimeout == 1))
     	{
		// set WRC to 0b01 of TCR 
		GET_TCR(tcr_value);
     	tcr_value &= ~0x30000000;
     	tcr_value |= 0x10000000;	     
     	SET_TCR(tcr_value);

		// set the ME bit of MSR 
		GET_MSR(msr_value);
		msr_value |= MSR_ME;
		SET_MSR(msr_value);

		// set the EMCP bit of HID0 
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

*/
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
/*
void WatchDogStart 
     (
     void
     )
     {
	 unsigned long hid0_value;
     unsigned ret; 
     unsigned *addr;
     // setting hid0 register 
     GET_HID0(hid0_value);
     hid0_value |= 0x00004000;              // enable the time base clock 
     
     // reset the timebase 
	 asm("lis		r16,0x0");
	 asm("ori		r16,r16,0x0");
	 asm ("mtspr 	284,r16   ");
	 asm("mtspr 	285,r16   ");
	 asm("mtspr 	284,r16   ");
	 asm("msync	");
      
	 while(1)
	 {
	 	start_time = (dtime() * 8) / 266000000;
	 }
	 //start_time = (dtime() * 8) / 266000000;	 

     
     SET_HID0(hid0_value);
     asm("isync");
     return;

     }

*/
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
/*
void WatchDogCancel (void)
{
	unsigned long tsr_value;
	unsigned long tcr_value;
	
	// clear the status in TSR register 
    GET_TSR(tsr_value);
    tsr_value |= TSR_ENW_WIS_WRS;
    SET_TSR(tsr_value);     	
	
	// clear the WPEXT and WP field of TCR register 
	GET_TCR(tcr_value);
    tcr_value &= ~(0xC0000000 | 0x001E0000);  
    SET_TCR(tcr_value);
	
	// reset the timebase registers 
	asm("lis		r16,0x0");
	asm("ori		r16,r16,0x0");
	asm ("mttbl 	r16   ");
	asm("mttbu 	r16   ");
	asm("mttbl 	r16   ");
	asm("msync	");
	return;
	
}
*/




























