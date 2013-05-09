/*
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.

 */

#include "fsl_lbc.h"
#include "config_localbus.h"
#include "immap_85xx.h"
#include "u-boot.h"
#include "asm/cache.h"
#include "asm/mmu.h"
#include "io.h"
//#include "ppu_intrinsics.h"
//#include "ppc_defs.h"
//#include "mpc85xx.h"
//#include "ppc_asm.tmpl"

//#include "config.h"
#include <stdio.h>
#include "cfi_flash.h"

#include "common.h"
#include "elbcTransnmitBuf.h"


u16 lbc_read_ok;


u16 lbc_mas[128];

extern void ludelay(unsigned long usec);

//u16 sendlocalBus_to_Tsec(u16 count,u16 *buf);

//u16 * get_lbcbuf();

void invalidate_icache();



u16 * get_lbc(u16 count)
{
	
count=lbc_read_ok;	
	
return lbc_mas;	
}





	
	
//Example of Asm  Cache functions.
asm void invalidate_icache()
{
invalidate_icache:
	mfspr	r0,L1CSR1
	ori	r0,r0,L1CSR1_ICFI
	msync
	isync

	mtspr	L1CSR1,r0
	isync
	blr				/* entire I cache */
}



///////////////////////////////////////Raw Read Functions///////////////////////////
static u8 __flash_read8(void *addr)
{
	return __raw_readb(addr);
}




/////////////////////Local Bus /Flash Function  ///////////////
static inline u16 __flash_read16(void *addr)
{
	
 return	*(volatile unsigned short *)PCI_FIX_ADDR(addr);
	
	//return __raw_readw(addr);
}



///////////////////////////////////////Raw Write Functions///////////////////////////
static inline void __flash_write16(u16 value, void *addr)
{
	__raw_writew(value, addr);
}



///////////////////////////////////////////////////////////////////////
static inline void * flash_map (u16 offset)
{
  unsigned int byte_offset = offset * 2;
  //puts("++++++++cfi_flash.c/flash_map+++++++\n\r");
	
  return (void *)(0xff000000 + byte_offset);
  //return (void *)(0xef000000 + byte_offset);     
}





void set_packet_size(volatile u16 p_aize)
{


ludelay(10000);
*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000dc)) = p_aize;	

}


/////////////////////////////////WRITE_TO_PLIS_SUCESSS//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void write_toPLIS_success2()
{
	
    //Channell2 =32*2=64 =0x40
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;	
	
	
}



void write_toPLIS_success3()
{
	
	//Channell3 =34*2=68 =0x44
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000044)) = 0x0003;
	
}



void write_toPLIS_success4()
{
	
	//Channell4 =36*2=72 =0x48	
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000048)) = 0x0003;
	
}

void write_toPLIS_success5()
{
	
	//Channell5 =38*2=76 =0x4C
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff00004C)) = 0x0003;
	
}


void write_toPLIS_success6()
{
	
	//Channell6 =40*2=80 =0x50
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000050)) = 0x0003;
	
}


void write_toPLIS_success7()
{
	
	//Channell7 =42*2=84 =0x54
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000054)) = 0x0003;
	
}


void write_toPLIS_success8()
{
	
	//Channell8 =44*2=88 =0x58
	*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000058)) = 0x0003;
	
}




void write_toPLIS_success()
{
u16 l_adress_write_success=30;
u16 data_write_success=0x0003;	
//u16 data_write_success=0xffff; 
//__flash_write16(data_write_success/*data_to_write*/ , flash_map(l_adress_write_success)); 	
  //__flash_write16(data_write_success/*data_to_write*/ , (void *)(0xff00003c));
   
   *(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff00003c)) = 0x0003;
   // __raw_writew(data_write_success, (void *)(0xff00003c)); 
  //(void *)(0xff00003c)
 //printf("Write_sucess Num=%d Waddress=0x%x -> Wdata= 0x%x|\n\r",l_adress_write_success,flash_map(l_adress_write_success),/*data_to_write*/ /*test_mas[m]*/data_write_success);	
//0xff00003c
}
	
	
	
	
///////////////////////////////////WRITE_TO_PLIS/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////	
void write_to_PLIS2(u16 *inpacket2_data,u16 data_legth2)
{
	
u16 l;//=0;

//u16 l_adress_write=100;   //int =20 ,hex =0x14    Read->120	
	
//cc ,=102 ,=204,->Write to 2 channel
	
 
 	for(l=0;l<data_legth2+1;l++)
 	{
  		*(volatile unsigned short *)(0xff000194) =inpacket2_data[l];
	
	}


 //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;	
	
	
}



void write_to_PLIS3(u16 *inpacket3_data,u16 data_legth3)
{
	
u16 l;//=0;


//d0 ,=104 ,=208,->Write to 3 channel	
 
 	for(l=0;l<data_legth3+1;l++)
 	{
  		*(volatile unsigned short *)(0xff000198) =inpacket3_data[l];
	
	}

		
}

/////////////////////////////////////////////////////////////////////
void write_to_PLIS4(u16 *inpacket4_data,u16 data_legth4)
{
	
u16 l;//=0;

//u16 l_adress_write=100;   //int =20 ,hex =0x14    Read->120	
	
//cc ,=102 ,=204,->Write to 2 channel
	
 
 	for(l=0;l<data_legth4+1;l++)
 	{
  		*(volatile unsigned short *)(0xff00019c) =inpacket4_data[l];
	
	}


 //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;	
	
	
}



void write_to_PLIS5(u16 *inpacket5_data,u16 data_legth5)
{
	
u16 l;//=0;

//u16 l_adress_write=100;   //int =20 ,hex =0x14    Read->120	
	
//cc ,=102 ,=204,->Write to 2 channel
	
 
 	for(l=0;l<data_legth5+1;l++)
 	{
  		*(volatile unsigned short *)(0xff0001a0) =inpacket5_data[l];
	
	}


 //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;	
	
	
}



void write_to_PLIS6(u16 *inpacket6_data,u16 data_legth6)
{
	
u16 l;//=0;

//u16 l_adress_write=100;   //int =20 ,hex =0x14    Read->120	
	
//cc ,=102 ,=204,->Write to 2 channel
	
 
 	for(l=0;l<data_legth6+1;l++)
 	{
  		*(volatile unsigned short *)(0xff0001a4) =inpacket6_data[l];
	
	}


 //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;	
	
	
}



void write_to_PLIS7(u16 *inpacket7_data,u16 data_legth7)
{
	
u16 l;//=0;

//u16 l_adress_write=100;   //int =20 ,hex =0x14    Read->120	
	
//cc ,=102 ,=204,->Write to 2 channel
	
 
 	for(l=0;l<data_legth7+1;l++)
 	{
  		*(volatile unsigned short *)(0xff0001a8) =inpacket7_data[l];
	
	}


 //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;	
	
	
}




void write_to_PLIS8(u16 *inpacket8_data,u16 data_legth8)
{
	
u16 l;//=0;
//u16 l_adress_write=100;   //int =20 ,hex =0x14    Read->120	
//cc ,=102 ,=204,->Write to 2 channel
 	for(l=0;l<data_legth8+1;l++)
 	{
  		*(volatile unsigned short *)(0xff0001ac) =inpacket8_data[l];
	
	}

 //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000040)) = 0x0003;		
}



////
void write_to_PLIS(u16 *inpacket_data,u16 data_legth)
{
//u16 data_to_write=0xaabb;
u16 m=0;
//u16 l_adress_write=100;   //int =20 ,hex =0x14 
 static u16 iteration;
u16 dannie30=1;
u16 dannie120=0;
u16 l_adress_read120=120;

u16 l_adress_read30=30;
int i;


//u16 pkt[508];

///ethernet_tsec2_send_packet(inpacket_data,data_legth);

/*

	for(i=0;i<508;i++)
	{
	pkt[i]=inpacket_data[i];
		
	}
*/	



//Printf_SIZE


 while(dannie30)
    {  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie30=__flash_read16(flash_map(l_adress_read30));
         //printf("Ctenie__30 =0x%x\n\r",dannie30);
///
     //
     //ludelay(10000);
    }

//dannie120=__flash_read16(flash_map(l_adress_read120));
//printf("Read_Iter_120_After30_Success , Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read120,flash_map(l_adress_read120),dannie120);







//u16 l_data_legth;



//l_data_legth=((data_legth/2)+1);

//u16 l_adress_write_success=30;
//u16 data_write_success=0x0003;
//u32 adress=0xff0000c8;

// printf("\n\r");
//printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
//printf("+++++++++++++++++Write_Iter->%d++++++++++++++++++++++++++++++++++++++\n\r",iteration++);
//__flash_write16(data_to_write , flash_map(l_adress_write));

//÷икл чтений из Cyclone3 PLIS по Local Bus
//unsigned short v;


// printf("length=%d\n\r",data_legth); 

 
 //#if 0
 
 	for(m=0;m<data_legth+1;m++)
 	{
   		//ludelay(100);
 	
  		//__flash_write16(inpacket_data[m] /*data_to_write*/ , flash_map(l_adress_write)); 

    	//__flash_write16(inpacket_data[m] /*data_to_write*/ , (void *)(0xff0000c8));
  	 

   		// __raw_writew(inpacket_data[m], (void *)(0xff0000c8));
  	//	*(volatile unsigned short *)(0xff000190) = test_full_packet_mas[m];//inpacket_data[m];
		*(volatile unsigned short *)(0xff000190) =inpacket_data[m];
		
		
		//*(volatile unsigned short *)(0xff0000c8)/*PCI_FIX_ADDR((void *)*/ = inpacket_data[m];
  		// __flash_read16((void *)(0xff0000c8));

 		// printf("0x%x|",inpacket_data[m]);  

	
	
	//	#if 0
 	//	if(m<4)
  //		{
  //	 	printf("Write_Iter =%d, Waddress=0x%x -> Wdata= 0x%04x|\n\r",m,0xff000190,inpacket_data[m] /*test_mas[m]*/ /*data_to_write*/);
  //		}
 
  	//	if(m>58)
  	//	{
 		// __flash_write16(test_mas[m]/*data_to_write*/ , flash_map(l_adress_write)); 
   	//	printf("Write_Iter =%d, Waddress=0x%x -> Wdata= 0x%04x|\n\r",m,0xff000190,inpacket_data[m] /*test_mas[m]*/ /*data_to_write*/);
    	// printf("0x%x|",inpacket_data[m]);
  	
      //  }
        //#endif
       
 
 	}
//	#endif


  
}









/*
u16 read_to_PLIS(u16  plis_mas[128])
{
 u16 t;
 u16 l_adress_read=102;    //int =22 ,hex =0x16	
 u16 status=0;
	
	for(t=0;t<128;t++)
 	{
 	udelay(10000);	
	plis_mas[t]=__flash_read16(flash_map(l_adress_read));
    printf("Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read,flash_map(l_adress_read),plis_mas[t]);
    udelay(10000);	
 	}
 status =1;	
	
	
	
return status;	
}
*/



void my_localbus_init()
{ 
u16 data=0x0000;
u16 offset=0x0;
u16 m=0;
u16 t=0;
u16 counter=0;
u16 memory_range_start=21;
u16 memory_range_end=128;
u32 addr=0xff000000;
u16 value =0x6666;
u16 data_to_write=0x3333;
u16 d=0;
//u16 out_mas[128];



#if 0
printf("############MONSTER###########MONSTER###################MONSTER#############MONSTER#######\n\r");


 for (t=0;t<4;t++)
 {
       
           if(t==0)
           {
            __flash_write16(test_mas[t], flash_map(t));	
           	printf("0x%x||",test_mas[t]); 
            
           }
           else
           {
           	__flash_write16(test_mas[t], flash_map(t));
            printf("0x%x||",test_mas[t]);	
           
           }
     
      
  }

#endif



//This element of 16bit (2bait) array. ot 0 from 100.
  
  u16 l_adress_write=100;   //int =20 ,hex =0x14 
  u16 l_adress_read=102;    //int =22 ,hex =0x16
///

printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!STRAT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\r");
//

//while(1)
//{
	
lbc_read_ok=0;


//÷икл чтений из Cyclone3 PLIS по Local Bus
 for(m=0;m<128;m++)
 {
  udelay(10000);
  __flash_write16(test_mas[m]/*data_to_write*/ , flash_map(l_adress_write)); 
  printf("Num=%d Waddress=0x%x -> Wdata= 0x%x|\n\r",l_adress_write,flash_map(l_adress_write),/*data_to_write*/ test_mas[m]);
  udelay(10000);
 }
 
 printf("############READ Iteration################################################################\n\r");
 //printf("############READ Iteration################################################################\n\r");
 //printf("############READ Iteration################################################################\n\r");
  
  

 for(t=0;t<128;t++)
 {
 udelay(10000);	
 //data= __flash_read16(flash_map(l_adress_read)); 
 // lbc_mas[t]=__flash_read16(flash_map(l_adress_read));
  	    
  	   //u16 sendlocalBus_to_Tsec(u16 count,u16 *buf);
  	     //get_lbcbuf();

  	 //Send_to_Ethernet_tsec buffer0x7e7e	
 printf("Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read,flash_map(l_adress_read),data);
 udelay(10000);	
 }
 lbc_read_ok=1;
  ////
//}
 
 ////
 
 // while(1)
 // {
  
#if 0  
  
  printf("+++++++++++++++++++++ %d Iteration+++++++++++++++++\n\r",d);
 // udelay(10000);
  __flash_write16(test_mas[t+d]/*data_to_write*/ , flash_map(l_adress_write)); 
  printf("Num=%d Waddress=0x%x -> Wdata= 0x%x|\n\r",l_adress_write,flash_map(l_adress_write),/*data_to_write*/ test_mas[t+d]);
  //udelay(10000);
  //data= __flash_read16(flash_map(l_adress_read)); 
  printf("Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read,flash_map(l_adress_read),data);
//  udelay(10000);
 
  
  if(d>=127)
  {
  	d=0;
  }
  d=d+1;
  
#endif
  
  
  

  
  
  
  
  
  
  
 
#if 0  
    
       printf("############WRITE %d Iteration###########WRITE###################WRITE#############WRITE#####################\n\r",counter);
      
       
       for (t=0;t<128;t++)
  	   {
        
        //__flash_write16(data_to_write, flash_map(20));
       // printf("0x%x|",data_to_write);
        
        __flash_write16(test_mas[t], flash_map(20));
  	    printf("0x%x|",test_mas[t]);
  	   }
       
       
   
       printf("\n\r");
       printf("############READ %d Iteration ###########Memory Start = 0x%x - EnD 0x%x ###################READ###############\n\r",counter,memory_range_start,memory_range_end); 
    // udelay(10000);
  	  
  	   
  	   for(memory_range_start=0;memory_range_start<memory_range_end;memory_range_start++)
       {
       //data= __flash_read16((void *)addr+m);
        
         data= __flash_read16(flash_map(22)); 
       
       //data= flash_read_word (offset+m);	
   	   //udelay(10000);
   	     printf("0x%x|",data);	 	
   	   
   	   /*if(memory_range_start==1)
   	   {
   	   	printf("\n\r");
   	   }*/
   	   
   	   }
   	   //udelay(10000);
   		memory_range_start=0; 
   	    t=0;
        counter=counter+1;
   		printf("################################################################################################################\n\r");
        printf("\n\r");
        
   
   
   
   #endif
   
  // }//end while


   

}//end local_bus_initialization