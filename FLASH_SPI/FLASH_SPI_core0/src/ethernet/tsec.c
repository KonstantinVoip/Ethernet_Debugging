/*
 * Freescale Three Speed Ethernet Controller driver
 *
 * This software may be used and distributed according to the
 * terms of the GNU Public License, Version 2, incorporated
 * herein by reference.
 *
 * Copyright 2004-2010 Freescale Semiconductor, Inc.
 * (C) Copyright 2003, Motorola, Inc.
 * author Andy Fleming
 *
 */
  #include <stdio.h>
  #include "malloc.h"
  #include "net.h"
  #include "tsec.h"
  #include "errno.h"
  #include "miiphy.h"
  #include "eth.h"
  #include "io.h"
  #include "elbcTransnmitBuf.h"
  




/////////////////////////Ethernet Configuration Parameter//////////////
////Ethernet Header
//7 element = 14 bait
                       /*Destination address*//*Source Address */ /*TYPE */
u16 ethernet_header[7]={0x20CF,0x30A8,0xF699,0x0011,0x2233,0x4455,0x0800};	//Ethernet Destination Address
//Ip Destination Address
//IPaddr_t  ip_dst=0xC0A8821C;   //192.168.130.28
int       udp_src_port=0x1001; // 4097
int       udp_dst_port=0x1000; // 4096



//u16 volatile plis_raw_data_mas[758];
/***************************************************************/
/////////////////////TSEC Configuration Parameters///////////////
/***************************************************************/


#define  RX_buffer_descriptors_per_ring  128 // RX_RING_SIZE
#define  TX_buffer_descriptors_per_ring  128 // TX_RING_SIZE                                                 



static uint tsec1_rxIdx;		/* index of the current RX buffer_TSEC1 */
static uint tsec1_txIdx;		/* index of the current TX buffer_TSEC1 */

static uint tsec2_rxIdx;		/* index of the current RX buffer_TSEC2 */
static uint tsec2_txIdx;		/* index of the current TX buffer_TSEC2 */


static uint tsec3_rxIdx;        /* index of the current RX buffer_TSEC3 */
static uint tsec3_txIdx;        /* index of the current TX buffer_TSEC3 */


typedef volatile struct tsec1 
{
	txbd8_t txbd[TX_buffer_descriptors_per_ring];
	rxbd8_t rxbd[RX_buffer_descriptors_per_ring];
} TSEC1;


typedef volatile struct tsec2 
{
	txbd8_t txbd[TX_buffer_descriptors_per_ring];
	rxbd8_t rxbd[RX_buffer_descriptors_per_ring];
} TSEC2;




typedef volatile struct tsec3 
{
	txbd8_t txbd[TX_buffer_descriptors_per_ring];
	rxbd8_t rxbd[RX_buffer_descriptors_per_ring];
} TSEC3;



static TSEC1 tsec1 __attribute__ ((aligned(64)));
static TSEC2 tsec2 __attribute__ ((aligned(64)));
static TSEC3 tsec3 __attribute__ ((aligned(64)));




#define MAXCONTROLLERS	(8)
static struct tsec_private *privlist[MAXCONTROLLERS];




//extern volatile uchar *NetTxPacket;
 // extern volatile u16 *NetTxPacket;      //Transmit packet        
//  volatile uint *RxPacet[RX_buffer_descriptors_per_ring];/* Receive packets		*/





/////////////////*EXTERN_FUNCTION_DEFENITION***********////////////
extern void ludelay(unsigned long usec);


//
//#ifdef __GNUC__
//static TSEC1 rtx __attribute__ ((aligned(16)));


/////////////////////Local Bus /Flash Function  ///////////////


static inline u16 __flash_read16(void *addr)
{
	return __raw_readw(addr);
}


static inline void * flash_map (u16 offset)
{
  unsigned int byte_offset = offset * 2;
  //puts("++++++++cfi_flash.c/flash_map+++++++\n\r");
	
  return (void *)(0xff000000 + byte_offset);
  //return (void *)(0xef000000 + byte_offset);     
}




//#else
//#error "rtx must be 64-bit aligned"
//#endif

//void tsec_send(struct eth_device *dev,volatile void *packet, int length);
   //struct eth_device dev[3];
  // struct tsec_private priv[3];
//
/*u16*///void assmble_packet(u16 length);




void tsec_send(struct eth_device *dev,struct eth_device *dev2);  
void tsec_recv(struct eth_device *dev,struct eth_device *dev2,struct eth_device *dev3);
static int tsec_init(struct eth_device *dev);
static void tsec_halt(struct eth_device *dev);
static void init_registers(volatile tsec_t * regs);
static void startup_tsec(struct eth_device *dev);
static int init_phy(struct eth_device *dev);
void write_phy_reg(struct tsec_private *priv, uint regnum, uint value);
uint read_phy_reg(struct tsec_private *priv, uint regnum);
struct phy_info *get_phy_info(struct eth_device *dev);
void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd);
static void adjust_link(struct eth_device *dev);
#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) \
	&& !defined(BITBANGMII)
static int tsec_miiphy_write(char *devname, unsigned char addr,unsigned char reg, unsigned short value);
static int tsec_miiphy_read(char *devname, unsigned char addr,unsigned char reg, unsigned short *value);
#endif
#ifdef CONFIG_MCAST_TFTP
static int tsec_mcast_addr (struct eth_device *dev, u8 mcast_mac, u8 set);
#endif




// Default initializations for TSEC controllers.
static struct tsec_info_struct tsec_info[] = {
#ifdef CONFIG_TSEC1
{(tsec_t *)(TSEC_BASE_ADDR),(tsec_mdio_t *)(MDIO_BASE_ADDR),(tsec_mdio_t *)(MDIO_BASE_ADDR),CONFIG_TSEC1_NAME,TSEC1_PHY_ADDR,TSEC1_FLAGS}, 
#endif
#ifdef CONFIG_TSEC2
{(tsec_t *)(TSEC_BASE_ADDR + TSEC_SIZE),(tsec_mdio_t *)(MDIO_BASE_ADDR),(tsec_mdio_t *)(MDIO_BASE_ADDR + MDIO_OFFSET),CONFIG_TSEC2_NAME,TSEC2_PHY_ADDR,TSEC2_FLAGS},
#endif
#ifdef CONFIG_TSEC3
{(tsec_t *)(TSEC_BASE_ADDR + 2 * TSEC_SIZE),(tsec_mdio_t *)(MDIO_BASE_ADDR),(tsec_mdio_t *)(MDIO_BASE_ADDR + 2*MDIO_OFFSET),CONFIG_TSEC3_NAME,TSEC3_PHY_ADDR,TSEC3_FLAGS},
#endif
#ifdef CONFIG_TSEC4
{  }
#endif
};






#define TEST_DATA_SIZE          0x200
u32 mas[TEST_DATA_SIZE] =
{
    0x02608c87, 0x65400000, 0x00006364, 0x01eeffff, 0x11111111, 0xffffffff,
    0x22222222, 0xffffffff, 0x33333333, 0xffffffff, 0x44444444, 0xffffffff,
    0x55555555, 0xffffffff, 0x66666666, 0xffffffff, 0x77777777, 0xffffffff,
    0x88888888, 0xffffffff, 0x99999999, 0xffffffff, 0xaaaaaaaa, 0xffffffff,
    0xbbbbbbbb, 0xffffffff, 0xcccccccc, 0xffffffff, 0xdddddddd, 0xffffffff,
 
    0xbbbbbbbb, 0xffffffff, 0xcccccccc, 0xffffffff, 0xdddddddd, 0xffffffff,
    0xbbbbbbbb, 0xffffffff, 0xcccccccc, 0xffffffff, 0xdddddddd, 0xffffffff,
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x11223344,
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x11223344,
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x11223344,
 
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x11223344,
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x11223344,
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x99999999,
    0x02608c87, 0x65400000, 0x00006364, 0x012affff, 0x11111111, 0xffffffff,
    0x22222222, 0xffffffff, 0x33333333, 0xffffffff, 0x44444444, 0xffffffff,
 
    0x55555555, 0xffffffff, 0x66666666, 0xffffffff, 0x77777777, 0xffffffff,
    0x88888888, 0xffffffff, 0x99999999, 0xffffffff, 0xaaaaaaaa, 0xffffffff,
    0xbbbbbbbb, 0xffffffff, 0xcccccccc, 0xffffffff, 0xdddddddd, 0xffffffff,
    0xbbbbbbbb, 0xffffffff, 0xcccccccc, 0xffffffff, 0xdddddddd, 0xffffffff,
    0xbbbbbbbb, 0xffffffff, 0xcccccccc, 0xffffffff, 0xdddddddd, 0xffffffff,
 
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x12345678, 0x11223344,
    0x11111111 };


struct tsec_private priv[3];




///////////////////////////////////TSEC_INITIALIZE_FUNCTIONS/////////////////////////////	
int tsec_eth_init(bd_t *bis, struct tsec_info_struct *tsecs, int num)
{
   int i;
   struct eth_device dev[3];
   //struct tsec_private priv[3];
   unsigned long event;
   int error = 0;
   memset(&dev, 0, sizeof(dev));
   memset(&priv, 0, sizeof(priv));
   
  u16 out_packet=0;
  u16 length=0; 


   for(i=1;i<4;i++)
   {
   	privlist[i] = &priv[i];
	priv[i].regs =          tsecs[i-1].regs;
	priv[i].phyregs =       tsecs[i-1].miiregs;
	priv[i].phyregs_sgmii = tsecs[i-1].miiregs_sgmii;
    priv[i].phyaddr =       tsecs[i-1].phyaddr;
    
	switch(i-1)
	    {case 0:puts("###############INITIALIZATION_TSEC1-->");break;
	     case 1:puts("###############INITIALIZATION_TSEC2-->");break;
	     case 2:puts("###############INITIALIZATION_TSEC3-->");break;
	     case 3:puts("*************?????****************");break;	
		default:break;}
	
	
	
	switch(priv[i].phyaddr)
	    {case 0:puts("MDIO PHYADDR=0###########\n\r");break;
	     case 1:puts("MDIO PHYADDR=1###########\n\r");break;
	     case 2:puts("MDIO PHYADDR=2###########\n\r");break;
	     case 3:puts("MDIO PHYADDR=3###########\n\r");break;	
		default:break;}
	
	
	
    priv[i].flags = tsecs[i-1].flags;
	dev[i].iobase = 0;
	dev[i].priv = &priv[i];
	dev[i].init = tsec_init;
	dev[i].halt = tsec_halt;
//	dev[i].send = tsec_send;
	dev[i].recv = tsec_recv;
	
	// Reset the MAC 
	priv[i].regs->maccfg1 |= MACCFG1_SOFT_RESET;
	//udelay(2);  // Soft Reset must be asserted for 3 TX clocks 
	ludelay(3); 
	priv[i].regs->maccfg1 &= ~(MACCFG1_SOFT_RESET);
	
	     if(0==init_phy(&dev[i]))
	     {
	        tsec_halt(&dev[i]); 
			puts("0==init_phy(&dev[i]) error init phy no match\n");
	        puts("STOP TSEC DEVICE\n");
	     }
	     else
	     {
	        if(tsec_init(&dev[i])<0)
			{
			   
			tsec_halt(&dev[i]);  //Stop tsec! if no Link fo external Device PHY. 
			puts("STOP TSEC DEVICE\n");
			}
			 
    
	    }
   
    }




//if (regs->ievent & IEVENT_BSY) 
//	          {regs->ievent = IEVENT_BSY;regs->rstat = RSTAT_CLEAR_RHALT;} 



///////dev1 ->TSEC1->2eth
//////dev2 ->TSEC2->3eth	
//////dev3 ->TSEC3->4eth	
	
//    struct tsec_private *priv = (struct tsec_private *)dev[1]->priv;
//	volatile tsec_t *regs = priv->regs;
	
//	struct tsec_private *priv2 = (struct tsec_private *)dev[2]->priv;
//	volatile tsec_t *regs2 = priv2->regs;
	
//	struct tsec_private *priv3 = (struct tsec_private *)dev[3]->priv;
//	volatile tsec_t *regs3 = priv3->regs;	
	
	///////////////////////////////////////////////MAIN NET LOOP/////////////////////////////////
    puts("##########################Start Ethernet CYCLE##############################\n\r");
	//assmble_packet(out_packet,length);
	//u16 ethernet_send_packet(volatile void *packet, int length);
	 	 
	 //while(1)
	 //{
	// tsec_send(&dev[2],&dev[1]);
	   tsec_recv(&dev[1],&dev[2],&dev[3]);	
	 //}
	 
	 
	 /*
	 for(;;)
	 {
	 tsec_recv(&dev[1],&dev[2]);
	 //tsec_send(&dev[2]);
	 }
	 */
	//tsec_recv(&dev[2]);
	 /*
    	  for(;;)
		  {
	       tsec_send(&dev[3], (void*)mas, 0x200);    
		  } 
	 */  
	   
	   
	   
return 0;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/*Send Packet to Tsec'1 */
void  ethernet_tsec1_send_packet(volatile unsigned int *packet1, u16 length1)
{
	
    tsec1.txbd[tsec1_txIdx].bufPtr = (unsigned int)packet1;
	tsec1.txbd[tsec1_txIdx].length = length1; 
    tsec1.txbd[tsec1_txIdx].status |=(TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);
    asm volatile ("msync");
     // Tell the DMA to go 
	priv[1].regs->tstat = TSTAT_CLEAR_THALT;  
	tsec1_txIdx = (tsec1_txIdx + 1) % TX_buffer_descriptors_per_ring; 	
	
}


/*Send Packet to Tsec'2 */
void ethernet_tsec2_send_packet(volatile u16 *packet2,int length2)
{
    tsec2.txbd[tsec2_txIdx].bufPtr = (unsigned int)packet2;
	tsec2.txbd[tsec2_txIdx].length = length2; 
    tsec2.txbd[tsec2_txIdx].status |=(TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);
    asm volatile ("msync");
     // Tell the DMA to go 
	priv[2].regs->tstat = TSTAT_CLEAR_THALT;  
	tsec2_txIdx = (tsec2_txIdx + 1) % TX_buffer_descriptors_per_ring; 	
	
}


/*Send Packet to Tsec3*/
void ethernet_tsec3_send_packet(u16 *packet3, int length3)
{
    tsec3.txbd[tsec3_txIdx].bufPtr = (unsigned int)packet3;
	tsec3.txbd[tsec3_txIdx].length = length3; 
    tsec3.txbd[tsec3_txIdx].status |=(TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);
    asm volatile ("msync");
     // Tell the DMA to go 
	priv[3].regs->tstat = TSTAT_CLEAR_THALT;  
	tsec3_txIdx = (tsec3_txIdx + 1) % TX_buffer_descriptors_per_ring; 	
	
}





////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////ASSEMBLE PACKET///////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void assmble_packet2(u16 length2)
{
	u16 plis_raw_data_mas2[length2];	
	u16 u=0;

	
	write_toPLIS_success2();
	
      do
      {  	 
           plis_raw_data_mas2[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff000324);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length2+1);
     
      ethernet_tsec2_send_packet(plis_raw_data_mas2,length2*2);
    
     // ethernet_tsec3_send_packet(plis_raw_data_mas2,length2*2);
    //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}


void assmble_packet3(u16 length3)
{
	u16 plis_raw_data_mas3[length3];	
	u16 u=0;

	
	write_toPLIS_success3();
	
      do
      {  	 
           plis_raw_data_mas3[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff000328);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length3+1);
     
     // ethernet_tsec1_send_packet(plis_raw_data_mas3,length3*2);
   // ethernet_tsec2_send_packet(plis_raw_data_mas3,length3*2);
       ethernet_tsec3_send_packet(plis_raw_data_mas3,length3*2);
      //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}


void assmble_packet4(u16 length4)
{
	u16 plis_raw_data_mas4[length4];	
	u16 u=0;

	
	write_toPLIS_success4();
	
      do
      {  	 
           plis_raw_data_mas4[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff00032c);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length4+1);
     
     // ethernet_tsec1_send_packet(plis_raw_data_mas3,length3*2);
   // ethernet_tsec2_send_packet(plis_raw_data_mas4,length4*2);
      ethernet_tsec3_send_packet(plis_raw_data_mas4,length4*2);
      //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}


void assmble_packet5(u16 length5)
{
	u16 plis_raw_data_mas5[length5];	
	u16 u=0;

	
	write_toPLIS_success5();
	
      do
      {  	 
           plis_raw_data_mas5[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff000330);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length5+1);
     
     // ethernet_tsec1_send_packet(plis_raw_data_mas3,length3*2);
   // ethernet_tsec2_send_packet(plis_raw_data_mas5,length5*2);
      ethernet_tsec3_send_packet(plis_raw_data_mas5,length5*2);
      //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}


void assmble_packet6(u16 length6)
{
	u16 plis_raw_data_mas6[length6];	
	u16 u=0;

	
	write_toPLIS_success6();
	
      do
      {  	 
           plis_raw_data_mas6[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff000334);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length6+1);
     
     // ethernet_tsec1_send_packet(plis_raw_data_mas3,length3*2);
    //ethernet_tsec2_send_packet(plis_raw_data_mas6,length6*2);
      ethernet_tsec3_send_packet(plis_raw_data_mas6,length6*2);
      //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}






void assmble_packet7(u16 length7)
{
	u16 plis_raw_data_mas7[length7];	
	u16 u=0;

	
	write_toPLIS_success7();
	
      do
      {  	 
           plis_raw_data_mas7[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff000338);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length7+1);
     
     // ethernet_tsec1_send_packet(plis_raw_data_mas3,length3*2);
   // ethernet_tsec2_send_packet(plis_raw_data_mas7,length7*2);
      ethernet_tsec3_send_packet(plis_raw_data_mas7,length7*2);
      //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}









void assmble_packet8(u16 length8)
{
	u16 plis_raw_data_mas8[length8];	
	u16 u=0;

	
	write_toPLIS_success8();
	
      do
      {  	 
           plis_raw_data_mas8[u]=*(volatile unsigned short *)/*PCI_FIX_ADDR((void *)*/(0xff00033c);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
   
           u++;
         //iteration=t;
        
       } while(u<length8+1);
     
     // ethernet_tsec1_send_packet(plis_raw_data_mas3,length3*2);
    //ethernet_tsec2_send_packet(plis_raw_data_mas8,length8*2);
      ethernet_tsec3_send_packet(plis_raw_data_mas8,length8*2);
      //ethiteration=ernet_tsec3_send_packet(plis_raw_data_mas2,1518);
	
}








//////////////////////////////////////////////////////////////////////////////////
void assmble_packet(u16 length)
{
  	
  u16 plis_raw_data_mas[length]; //massive_count = 0 to 759  element =760 word = 1520 bait
  u16 dannie0=0;
  u16 dannie2=0;
  u16 dannie4=0;
  u16 dannie6=0;
  u16 dannie8=0;
  u16 dannie10=0;
  u16 dannie12=0; 
  u16 dannie30=1;
  u16 dannie400=0;
  u16 dannie100=0;
  u16 dannie120=0;
  u16 dannie1200=0;
  
  //u16 l_adress_read12=12;
  static u16 iteration=0;

  u16 l_adress_read0=0;
  u16 l_adress_read2=2;
  u16 l_adress_read4=4;
  u16 l_adress_read6=6;
  u16 l_adress_read8=8;
  u16 l_adress_read10=10;
  u16 l_adress_read12=12;
  u16 l_adress_read30=30;
  u16 l_adress_read400=400;
  u16 l_adress_read100=100;
  u16 l_adress_read120=120;
  u16 l_adress_read1200=1200;
  
  
  u16 t=0;
  u16 l_adress_read800=800;
  u16 dannie800=0;
 
 
  u16 l_adress_read1000=1000;
  u16 dannie1000=0;
 
  //memset(&plis_raw_data_mas, 0xaaaa, sizeof(plis_raw_data_mas));
  
  
//  u16 plis_raw_data_mas1[length];
//  u16 plis_raw_data_mas2[length];
//  u16 plis_raw_data_mas4[length];
 
 //u16 plis_raw_data_mas;



/*
*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000000)) = 0x5555;


 
 dannie0=__flash_read16(flash_map(l_adress_read0));
 printf("Read_Iter_0, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read0,flash_map(l_adress_read0),dannie0);
    //memset(&plis_raw_data_mas, 0x0000, sizeof(plis_raw_data_mas));

 dannie2=__flash_read16(flash_map(l_adress_read2));
 printf("Read_Iter_2, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read2,flash_map(l_adress_read2),dannie2);
    //memset(&plis_raw_data_mas, 0x0000, sizeof(plis_raw_data_mas));
 

 
//////Read 44 element///////////////
 dannie4=__flash_read16(flash_map(l_adress_read4));
 printf("Read_Iter_4, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read4,flash_map(l_adress_read4),dannie4);


 dannie6=__flash_read16(flash_map(l_adress_read6));
 printf("Read_Iter_6, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read6,flash_map(l_adress_read6),dannie6);


 dannie8=__flash_read16(flash_map(l_adress_read8));
 printf("Read_Iter_8, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read8,flash_map(l_adress_read8),dannie8);


 dannie10=__flash_read16(flash_map(l_adress_read10));
 printf("Read_Iter_10, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read10,flash_map(l_adress_read10),dannie10);


//write_to_PLIS(test_full_packet_mas,759);


printf("Writw_Iter_12, ->Rdata=0x3322|\n\r");

*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000018)) = 0xffff;

 dannie12=__flash_read16(flash_map(l_adress_read12));
 printf("Read_Iter_12, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read12,flash_map(l_adress_read12),dannie12);

 
 */
 
/*  *(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff00003c)) = 0xffff; 
   printf("30---->1\n\r");*/
////write_to_PLIS(test_full_packet_mas,500);  
//write_toPLIS_success();
  
  
  



/*
u16 raw_data_from_plis_element=0;
u16 raw_data_from_plis_size=0;
//u16 t=0;
static u16 iteration=0;
u16 read_data=0x0000;

u16 read_enable=0x0000;
u16 l_adress_read=102;
////
u16 l_adress_read30=30;
u16 l_addres_read40=40;
u16 l_adress_read42=42; 	
u16 l_addres_read44=44;
u16 l_addres_read46=46;
u16 l_addres_read52=52;
u16 l_addres_read54=54;
u16 l_addres_read64=64;
u16 l_addres_read68=68;
/////
u16 dannie44=0;
u16 dannie40=0;
u16 dannie30=0;
u16 dannie42=0;
u16 dannie46=0;
u16 dannie52=0;
u16 dannie54=0;
u16 dannie64=0;
u16 dannie68=0;
*/




//printf("in_length =%d\n\r",length);


 // memset(&plis_raw_data_mas, 0xffff, sizeof(plis_raw_data_mas));
//memset(&ip_packet, 0, sizeof(ip_packet));





//	for(t=0;t<128;t++)
 //   {
  //  NetTxPacket[t]=0xaabb;//__flash_read16(flash_map(l_addres_read52));
   // }

//int NetSetEther(volatile uchar * xet, uchar * addr, uint prot)
//int NetSendUDPPacket(uchar *ether, IPaddr_t dest, int dport, int sport, int len)
 //NetSendUDPPacket((uchar*)et_dest, ip_dst, 1000, 1001, raw_data_from_plis_size);



//#if 0


  //printf("\n\r");

//memcpy (&super_raw_data_mas[0], et_dest,14);
//printf("+++++++++++++++++Iteration %d+++++++++++++++Iteration +++++++++++++++++++Iteration +++++++++++++++\n\r",iteration++);
  //printf("!!!!!!!!!!!!!!!!!Iteration %d!!!!!!!!!!!!!!!Iteration !!!!!!!!!!!!!!!!!!!\n\r",iteration++);




//////Read 30 element//////////////

 //dannie30=__flash_read16(flash_map(l_adress_read30));
 //printf("Read_Iter_30_do_write100, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read30,flash_map(l_adress_read30),dannie30);
 
 
 //////Read 40 element//////////////
// dannie40=__flash_read16(flash_map(l_addres_read40));
// printf("Read_Iter_40_do_write100, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read40,flash_map(l_addres_read40),dannie40);
  

 //////Read 42 element//////////////
// dannie42=__flash_read16(flash_map(l_adress_read42));
 //printf("Read_Iter_42_do_write100, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read42,flash_map(l_adress_read42),dannie42);
  
 
//////Read 44 element///////////////
 //dannie44=__flash_read16(flash_map(l_addres_read44));
 //printf("Read_Iter_44_do_write100,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read44,flash_map(l_addres_read44),dannie44);

//Read 46 element
 //dannie46=__flash_read16(flash_map(l_addres_read46));
// printf("Read_Iter_46_do_write100,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read46,flash_map(l_addres_read46),dannie46);




//printf("\n\r");
/*
printf("Write_Iter_12,->>data=0xffff|\n\r");
*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000018)) = 0xffff;


dannie12=__flash_read16(flash_map(l_adress_read12));
 printf("Read_Iter_12, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read12,flash_map(l_adress_read12),dannie12);


ludelay(100);
printf("Write_Iter_12,->>data=0x0000|\n\r");
*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff000018)) = 0x0000;


dannie12=__flash_read16(flash_map(l_adress_read12));
printf("Read_Iter_12, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read12,flash_map(l_adress_read12),dannie12);
*/


/*
dannie12=__flash_read16(flash_map(l_adress_read12));
printf("Read_Iter_12, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read12,flash_map(l_adress_read12),dannie12);
*/



//dannie100=__flash_read16(flash_map(l_adress_read100));
//printf("Read_Iter_100_BeforeWrite , Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read100,flash_map(l_adress_read100),dannie100);


/*

 while(dannie30)
    {  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie30=__flash_read16(flash_map(l_adress_read30));
         printf("Ctenie__30 =0x%x\n\r",dannie30);

     //
     //ludelay(10000);
    }
*/


//dannie120=__flash_read16(flash_map(l_adress_read120));
//printf("Read_Iter_120_AfterWrite , Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read120,flash_map(l_adress_read120),dannie120);
  

//write_to_PLIS(test_full_packet_mas,64);
//write_to_PLIS(test_full_packet_mas,256);  
//

//write_toPLIS_success();



//printf("--------------------READ_Iter=%d----------------------------------\n\r",iteration++);


//dannie100=__flash_read16(flash_map(l_adress_read100));
//printf("Read_Iter_100_AfterWrite , Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read100,flash_map(l_adress_read100),dannie100);



	




    while(!dannie1000)
     {  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie1000=__flash_read16(flash_map(l_adress_read1000));
         
         /*if(dannie1000==0xabc0)
         {
         	
         	dannie1000=0;
         }*/
        
        // dannie800=__flash_read16(flash_map(l_adress_read800));       
          //printf("WHILE_Ctenie__1000 =0x%x\n\r",dannie1000);
         // printf("WHILE_Ctenie__800 =0x%x\n\r",dannie800);  
    
     }





     dannie800=__flash_read16(flash_map(l_adress_read800)); 
    // printf("Ctenie__800 =0x%x\n\r",dannie800);
   
 /* dannie1200=__flash_read16(flash_map(l_adress_read1200));
  printf("Razmer_in1200, Num=%d Raddress =0x%x ->Rdata=0x%04x|\n\r",l_adress_read1200,flash_map(l_adress_read1200),dannie1200);
 */

   //while(!dannie800)
    //{  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         //dannie800=__flash_read16(flash_map(l_adress_read800));
         //  printf("Second_ctenie_posle _800 =0x%x\n\r",dannie800);
          // printf("Ctenie__1000 =0x%x\n\r",dannie1000); 
        // printf("Visim_while 800 =0x%x\n\r",dannie800);
    //}







/*
dannie120=__flash_read16(flash_map(l_adress_read120));
printf("Read_Iter_120_After30_Success , Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read120,flash_map(l_adress_read120),dannie120);
*/




//dannie1200=__flash_read16(flash_map(l_adress_read1200));
//printf("Razmer_in1200, Num=%d Raddress =0x%x ->Rdata=0x%04x|\n\r",l_adress_read1200,flash_map(l_adress_read1200),dannie1200);
  
 //ludelay(1000);   
 
 
 //getchar();
//printf("+\n\r");      
//printf("--------------------READ_Iter=%d----------------------------------\n\r",iteration++);      
//#if 0      
       do
       {
        
       // plis_raw_data_mas[t]=__flash_read16((void *)(0xff0000cc));
        
           //plis_raw_data_mas[t]=*(volatile unsigned short *)(0xff0000cc);//__flash_read16((void *)(0xff0000cc));
         //NetTxPacket[t]=*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff0000cc));
       
       plis_raw_data_mas[t]=*(volatile unsigned short *)(0xff000320);
         
      // #if 0
       
       
       // if(t<4)
         //{
          //  printf("\n+");	
         // printf("Read_Iter =%d, Raddress=0x%x -> Rdata= 0x%04x|\n\r",t,0xff000320,plis_raw_data_mas[t] /*test_mas[m]*/ /*data_to_write*/);
           	
         //}
        
         
         // if(t>59)
         // {
         //  	
         // printf("Read_Iter =%d, Raddress=0x%x -> Rdata= 0x%04x|\n\r",t,0xff000320,plis_raw_data_mas[t] /*test_mas[m]*/ /*data_to_write*/);
           	
         // }
        
       // #endif
        
        
        
        
       // #if 0
        //   if(t<4)
        //   {
           	
        //  printf("Read_Iter =%d,Raddress=0x%x -> Rdata= 0x%04x|\n\r",t,0xff000320,plis_raw_data_mas[t] /*test_mas[m]*/ /*data_to_write*/);
  		 //   }
          
          
            
          // printf("\n");
           
          // if(t>123)
          // {
           	
         // printf("Read_Iter =%d, Raddress=0x%x -> Rdata= 0x%x|\n\r",t,0xff000320,plis_raw_data_mas[t] /*test_mas[m]*/ /*data_to_write*/);
           	
           //}
           
       //  #endif  
           
           t++;
         //iteration=t;
        
        } while(t<length+1);

// printf("--------------------After Read---------------------------------\n\r");

ethernet_tsec2_send_packet(plis_raw_data_mas,length*2);
//ethernet_tsec3_send_packet(plis_raw_data_mas,length*2);

  
/*

     while(!dannie800)
    {  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie800=__flash_read16(flash_map(l_adress_read800));
         printf("Second_ctenie_posle _800 =0x%x\n\r",dannie800);

      //printf("Visim_while 800 =0x%x\n\r",dannie800);
    }

*/








/*
dannie400=__flash_read16(flash_map(l_adress_read400));
printf("Read_Iter_400, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read400,flash_map(l_adress_read400),dannie400);
*/







/*
dannie12=__flash_read16(flash_map(l_adress_read12));
printf("Read_Iter_12, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read12,flash_map(l_adress_read12),dannie12);
*/









/*
dannie800=__flash_read16(flash_map(l_adress_read800));
printf("Read_Iter_800_1, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read800,flash_map(l_adress_read800),dannie800);
*/


/*
dannie1000=__flash_read16(flash_map(l_adress_read1000));
printf("Read_Iter_1000_1, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read1000,flash_map(l_adress_read1000),dannie1000);
*/






/*
dannie800=__flash_read16(flash_map(l_adress_read800));
printf("Read_Iter_800_2, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read800,flash_map(l_adress_read800),dannie800);


dannie800=__flash_read16(flash_map(l_adress_read800));
printf("Read_Iter_800_3, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read800,flash_map(l_adress_read800),dannie800);


dannie800=__flash_read16(flash_map(l_adress_read800));
printf("Read_Iter_800_4, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read800,flash_map(l_adress_read800),dannie800);
*/



 //////Read 30 element//////////////

/*
dannie30=__flash_read16(flash_map(l_adress_read30));
printf("Read_Iter_30, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read30,flash_map(l_adress_read30),dannie30);
*/ 
 
 
  //////Read 30 element//////////////
// dannie800=__flash_read16(flash_map(l_adress_read800));
// printf("Read_Iter_800, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read800,flash_map(l_adress_read800),dannie800);
 
 
 
 //////Read 40 element//////////////
 //dannie40=__flash_read16(flash_map(l_addres_read40));
 //printf("Read_Iter_40_After_write100, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read40,flash_map(l_addres_read40),dannie40);
  

 //////Read 42 element//////////////



/*
 dannie42=__flash_read16(flash_map(l_adress_read42));
 printf("Read_Iter_42_do_write30, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read42,flash_map(l_adress_read42),dannie42);
  
 
//////Read 44 element///////////////
dannie44=__flash_read16(flash_map(l_addres_read44));
printf("Read_Iter_44_do_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read44,flash_map(l_addres_read44),dannie44);


//Read 46 element
 dannie46=__flash_read16(flash_map(l_addres_read46));
 printf("Read_Iter_46_do_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read46,flash_map(l_addres_read46),dannie46);


 //Read 52 element
 dannie52=__flash_read16(flash_map(l_addres_read52));
 printf("Read_Iter_52_do_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read52,flash_map(l_addres_read52),dannie52);


  //Read 54 element
 dannie54=__flash_read16(flash_map(l_addres_read54));
  printf("Read_Iter_54_do_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read54,flash_map(l_addres_read54),dannie54);

*/




//////Read 30 element//////////////
// dannie30=__flash_read16(flash_map(l_adress_read30));
 //printf("NO_Read_Iter_30_after_write30, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read30,flash_map(l_adress_read30),dannie30);
 
 //////Read 40 element//////////////
 //dannie40=__flash_read16(flash_map(l_addres_read40));
 //printf("Read_Iter_40_After_write100, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read40,flash_map(l_addres_read40),dannie40);
  
  
 
 //////Read 42 element//////////////
 //dannie42=__flash_read16(flash_map(l_adress_read42));
 //printf("Read_Iter_42_after_write30, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read42,flash_map(l_adress_read42),dannie42);
  
 //
//////Read 44 element/////////////
/*

 dannie44=__flash_read16(flash_map(l_addres_read44));
 printf("Read_Iter_44_do_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read44,flash_map(l_addres_read44),dannie44);


//Read 46 element
 //dannie46=__flash_read16(flash_map(l_addres_read46));
 //printf("Read_Iter_46_after_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read46,flash_map(l_addres_read46),dannie46);

*/
 //printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
 //Read 52 element
 //dannie52=__flash_read16(flash_map(l_addres_read52));
 //printf("Read_Iter_52_do_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read52,flash_map(l_addres_read52),dannie52);


 
   //Read 54 element
// dannie54=__flash_read16(flash_map(l_addres_read54));
// printf("Read_Iter_54_after_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read54,flash_map(l_addres_read54),dannie54);




//printf("\n\r");

//ludelay(10);

 //ludelay(100000); 

// write_to_PLIS(test_full_packet_mas,759);
//commment poka
// write_toPLIS_success();

//write_toPLIS_success();

  //write_to_PLIS(test_full_packet_mas,760);
 // write_toPLIS_success();



  //write_to_PLIS(test_full_packet_mas,759);
 // write_toPLIS_success();
 //Read 64 element
 //dannie64=__flash_read16(flash_map(l_addres_read64));
// printf("Read_Iter_64_Init_State,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read64,flash_map(l_addres_read64),dannie64);


 //Read 68 element
 //dannie68=__flash_read16(flash_map(l_addres_read68));
 //printf("Read_Iter_68_Init_State,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read68,flash_map(l_addres_read68),dannie68);


 /*
if(iteration==1)
{
	



 write_to_PLIS(test_full_packet_mas,759);


 //Read 64 element
// dannie64=__flash_read16(flash_map(l_addres_read64));
 //printf("Read_Iter_64_After_Write,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read64,flash_map(l_addres_read64),dannie64);


 //Read 68 element

 dannie68=__flash_read16(flash_map(l_addres_read68));
 printf("Read_Iter_68_After_Write,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read68,flash_map(l_addres_read68),dannie68);


 write_toPLIS_success();
}
*/

 //Read 64 element
// dannie64=__flash_read16(flash_map(l_addres_read64));
// printf("Read_Iter_64_After_set30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read64,flash_map(l_addres_read64),dannie64);

 //Read 68 element
  //dannie68=__flash_read16(flash_map(l_addres_read68));
  //printf("Read_Iter_68_After_set30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read68,flash_map(l_addres_read68),dannie68);
 
  

//ludelay(100000);
/*
{

//write_to_PLIS(test_mas,128);
write_toPLIS_success();
read_enable=__flash_read16(flash_map(l_adress_read42));	


	
}while(!read_enable);*/


/*
while(read_enable==0x4)
{

  //ludelay(10000000);
  read_enable=__flash_read16(flash_map(l_adress_read42));
  //if(read_enable==0x00)
  //{
 // printf("NOT_READ_Operation=%d, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",t=t++,l_adress_read42,flash_map(l_adress_read42),read_enable);	
  //}


}*/
    
 //////Read 44 element///////////////

/*
 dannie44=__flash_read16(flash_map(l_addres_read44));
 printf("Read_Iter_44_After_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read44,flash_map(l_addres_read44),dannie44);   
 */   
    
    //Read 52 element
 //dannie52=__flash_read16(flash_map(l_addres_read52));
 //printf("Read_Iter_52_After_write30,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read52,flash_map(l_addres_read52),dannie52);


   //Read 54 element
 //dannie54=__flash_read16(flash_map(l_addres_read54));
// printf("Read_Iter_54_before_Read,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read54,flash_map(l_addres_read54),dannie54);



//printf("StartREAD_Operation 42 ->to 1, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read42,flash_map(l_adress_read42),read_enable);

//#if 0
  
  //  printf("StartREAD_Operation_ON_PLIS->>>>>>>>>>>>>>>>>  42 ->to 1,|\n\r");


/*
while(t<760)
{
	
	plis_raw_data_mas[t++]=__flash_read16((void *)(0xff0000cc));
	
}
*/




//#if 0

//	for(t=0;t<=759;t++)
   // {
  	    
  	    //ludelay(1000);
  	  //ludelay(1000000);
  	 // plis_raw_data_mas[t]=__flash_read16(flash_map(l_adress_read));
        
        //
        
        //NetTxPacket[t]=__flash_read16(flash_map(l_adress_read));
        //read_data=__flash_read16(flash_map(l_adress_read));
      //NetTxPacket[t]=0xaabb;//__flash_read16(flash_map(l_addres_read52));
        //NetTxPacket[t]=__flash_read16(flash_map(l_adress_read));
        // printf("Read_Iter=%d, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",t,l_adress_read,flash_map(l_adress_read),NetTxPacket[t]/*plis_raw_data_mas[t]*/);
      
        //plis_raw_data_mas[t]=__flash_read16(flash_map(l_adress_read));
      
      
      //memset(&plis_raw_data_mas,0xaabb,759);
       // __flash_read16((void *)(0xff0000cc)
     //ludelay(10);
      
      //memcpy(&plis_raw_data_mas,test_full_packet_mas,1518);
   //ludelay(1000);
  //printf("\r");
   
   
   // printf("Output Packet =%d ,length=%d\r\n",iteration,length); 
 //printf("\n");    



//ludelay(10000); 
 
/*
 while(!dannie1000)
    {  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie1000=__flash_read16(flash_map(l_adress_read1000));
         printf("1000_Register_1_Read ->>>0x%x\n\r",dannie1000);
      //printf("Visim_while 800 =0x%x\n\r",dannie800);
        ludelay(100000); 
    }    
 */ 
 

   /*   
  dannie1000=*(volatile unsigned short *)(0xff0007D0);
  
  
       // dannie1000=__flash_read16(flash_map(l_adress_read1000));
   printf("1000_Register_2_Read ->>>0x%x\n\r",dannie1000); 
  
  
        
     //   dannie800=__flash_read16(flash_map(l_adress_read800));
     //   printf("800_Register_1_Read ->>>0x%x\n\r",dannie800);
  
  
        dannie1000=__flash_read16(flash_map(l_adress_read1000));
        printf("1000_Register_3_Read ->>>0x%x\n\r",dannie1000); 
  
  */
  
  
      //printf("!!!!!1000_Register_SUCCESS =0x%x!!!!!!\n\r",dannie1000);
  
  /*

       dannie800=__flash_read16(flash_map(l_adress_read800));
       printf("800_Register_1_Read ->>>0x%x\n\r",dannie800);


       ludelay(100);

         
       dannie800=__flash_read16(flash_map(l_adress_read800));
       printf("800_Register_2_Read ->>>0x%x\n\r",dannie800); 
  */



/*
     while(!dannie800)
    {  
      
        //*(volatile unsigned short *)PCI_FIX_ADDR((void *)(0xff00003c)) = 0xffff; 
      
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie800=__flash_read16(flash_map(l_adress_read800));
         printf("Visim_while_0 800 =0x%x\n\r",dannie800);
         if(dannie800==0xfffe)
         {
         //	printf("Visim_while 800 =0x%x\n\r",dannie800);
         	dannie800=0;
            
         }
      //printf("Visim_while 800 =0x%x\n\r",dannie800);
     // ludelay(100000); 
    }    
  
*/










   //printf("!!!!!!!!!!!!!!!!!800_Register_OK!!!!!!!!!!!!!\n\r");










	


// printf("--------------------After Read---------------------------------\n\r");

// ludelay(10);  
   
   /*
   
       while(dannie800)
    {  
       //data_in_memory800=*(volatile unsigned short *)(0xff000640);
         dannie800=__flash_read16(flash_map(l_adress_read800));
         printf("Second_ctenie_posle _800 =0x%x\n\r",dannie800);

      //printf("Visim_while 800 =0x%x\n\r",dannie800);
    }
 */



   
 
   ////////////////////Second_Ctenie_Iz_Regosters


  

 //printf("--------------------800=OK_Sbros=%x----------------------------------\n\r",dannie800); 
   


   
       
//#endif
       
      // printf("\n\r");
       // t=0;
      // #if 0 
     // #endif
      //ludelay(10000);//l_address_readl_adress_read
      //ludelay(1000000);
      //out_packet[t]=__flash_read16(flash_map(l_adress_read));
    //}

//#endif  
  
  /*
   dannie44=__flash_read16(flash_map(l_addres_read44));
   printf("Read_Iter_44_after_Read102, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read44,flash_map(l_addres_read44),dannie44);
  */
    //ludelay(1000);
  
// ethernet_send_packet(NetTxPacket,1518);
//ethernet_send_packet(plis_raw_data_mas,length);
//ethernet_tsec2_send_packet(*(volatile unsigned short *)(0xff0000cc),length*2);  
  
     //  ethernet_tsec2_send_packet(plis_raw_data_mas,length*2);


         //ethernet_tsec3_send_packet(plis_raw_data_mas,length*2); 	
      //printf("tr_event=%x\n",regs2->ievent)
   
   //iteration++;
   //printf("+++++++++++++++++++++++++++\n\r"); 
    /*
    tsec2.txbd[tsec2_txIdx].bufPtr = (unsigned int)plis_raw_data_mas;
	tsec2.txbd[tsec2_txIdx].length = 1518; 
    tsec2.txbd[tsec2_txIdx].status |=(TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);
    asm volatile ("msync");
     // Tell the DMA to go 
	priv[2].regs->tstat = TSTAT_CLEAR_THALT;  
	tsec2_txIdx = (tsec2_txIdx + 1) % TX_buffer_descriptors_per_ring; 
  */
  
    
  
   //Read 68 element
 // dannie68=__flash_read16(flash_map(l_addres_read68));
 // printf("Read_Iter_68_After_Read,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read68,flash_map(l_addres_read68),dannie68);
  
  
   // ethernet_send_packet(plis_raw_data_mas, 1518); 
   
       //printf("tr_event=%x\n",regs2->ievent);

  
   // printf("END_READ_Operation_ON_PLIS->>>>>>>>>>>>>>>>>  42 ->to 1,|\n\r");
   //NetSendUDPPacket((uchar*)ethernet_header, ip_dst, udp_dst_port, udp_src_port, length);
  //ludelay(10000);
  
//#endif
  
  
  
//write_toPLIS_success();


 //////Read 30 element//////////////
 //dannie30=__flash_read16(flash_map(l_adress_read30));
 //printf("NO_Read_Iter_30__After_read102, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read30,flash_map(l_adress_read30),dannie30);


#if 0
 
 //////Read 40 element//////////////
 dannie40=__flash_read16(flash_map(l_addres_read40));
 printf("Read_Iter_40__After_read102, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read40,flash_map(l_addres_read40),dannie40);
  

 //////Read 42 element//////////////

 dannie42=__flash_read16(flash_map(l_adress_read42));
 printf("Read_Iter_42__After_read102, Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_adress_read42,flash_map(l_adress_read42),dannie42);
  
 
//////Read 44 element///////////////
 dannie44=__flash_read16(flash_map(l_addres_read44));
 printf("Read_Iter_44__After_read102,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read44,flash_map(l_addres_read44),dannie44);


 dannie46=__flash_read16(flash_map(l_addres_read46));
 printf("Read_Iter_46___After_read102,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read46,flash_map(l_addres_read46),dannie46);
 
  //Read 52 element
 dannie52=__flash_read16(flash_map(l_addres_read52));
 printf("Read_Iter_52_After_read102,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read52,flash_map(l_addres_read52),dannie52);
    //Read 54 element
 dannie54=__flash_read16(flash_map(l_addres_read54));
 printf("Read_Iter_54_After_read102,Num=%d Raddress =0x%x ->Rdata=0x%x|\n\r",l_addres_read54,flash_map(l_addres_read54),dannie54);

#endif

//#endif  /////////////////////end MAIN Endif Cicle///////////////////////
	
//return 1;	
}



/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!TSEC RECIEVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
void tsec_recv(struct eth_device *dev,struct eth_device *dev2,struct eth_device *dev3)
{
	
  
   volatile u16 size_packet_rxtsec1=0;
   volatile u16 size_packet_rxtsec2=0;
   volatile u16 size_packet_rxtsec3=0;
   u16 iteration=0;  
   u16 i;
  // assmble_packet(500);
/*
  assmble_packet(756);
  assmble_packet(756);
  assmble_packet(756);
  assmble_packet(756); 
 */
 
 
 /*
  assmble_packet(64);
  assmble_packet(64);
  assmble_packet(64);
  assmble_packet(64);
 */ 
  
  
  
  /*
  
  for(i=0;i<=10;i++)
  {
  	
  
  
  
   write_to_PLIS(tsec1.rxbd[tsec1_rxIdx].bufPtr,64);
   assmble_packet(64);         
  }
  
  */
  
  
  
  for(;;)
  {
  	  // assmble_packet(42); 
  	 assmble_packet(256);
  	   
  	  //assmble_packet(128);
  	 //  assmble_packet(super_mas,759); 
    
  	 
  	 
  #if 0	 
  	 	while (!(tsec1.rxbd[tsec1_rxIdx].status & RXBD_EMPTY))
	 	{	
        
          
             size_packet_rxtsec1=((tsec1.rxbd[tsec1_rxIdx].length-4)/2);
             //  printf("+\n\r");
        
           // if(tsec1.rxbd[tsec1_rxIdx].length-4==128)
           // {     
            
            
             // ethernet_tsec2_send_packet(tsec1.rxbd[tsec1_rxIdx].bufPtr,tsec1.rxbd[tsec1_rxIdx].length-4);
             // ethernet_tsec3_send_packet(tsec1.rxbd[tsec1_rxIdx].bufPtr,tsec1.rxbd[tsec1_rxIdx].length-4);
            
            
            
             //printf("--------------------Packet recieve=%d----------------------------------\n\r",iteration++);     
            
               //printf("+++++++++++++++packet recieve+++++++++\n\r");
            
            
             write_to_PLIS(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	         assmble_packet(size_packet_rxtsec1);   //go_to_tsec2
           
              /*
                write_to_PLIS2(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	           	assmble_packet2(size_packet_rxtsec1);   //go_to_tsec2
                
              
              
               	write_to_PLIS3(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	           	assmble_packet3(size_packet_rxtsec1);   //go_to_tsec2
                                    
            
               	write_to_PLIS4(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	           	assmble_packet4(size_packet_rxtsec1);   //go_to_tsec2
                
               	write_to_PLIS5(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	        	assmble_packet5(size_packet_rxtsec1);   //go_to_tsec2
            
            
            
                write_to_PLIS6(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	        	assmble_packet6(size_packet_rxtsec1);   //go_to_tsec2
            
                     
                write_to_PLIS7(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	        	assmble_packet7(size_packet_rxtsec1);   //go_to_tsec2
            
            
            	write_to_PLIS8(tsec1.rxbd[tsec1_rxIdx].bufPtr,size_packet_rxtsec1);
	        	assmble_packet8(size_packet_rxtsec1);   //go_to_tsec2
              */
           
          //  ludelay(1000);
            //}		
			//printf("event_tsec=%x\n",regs->ievent);	        				 
	        //NetReceive(tsec1.rxbd[tsec1_rxIdx].bufPtr, tsec1.rxbd[tsec1_rxIdx].length-4);
	  	    eieio();
	  	    tsec1.rxbd[tsec1_rxIdx].status = RXBD_EMPTY | (((tsec1_rxIdx + 1) == RX_buffer_descriptors_per_ring) ? RXBD_WRAP : 0);
 	   	    tsec1_rxIdx = (tsec1_rxIdx + 1) % RX_buffer_descriptors_per_ring;
	  	
	  	}
   
   #endif
   
   #if 0
   
  
	  	
	  	
	  	 //TSEC2 Recieve and Transmit  PLIS  ->TSEC1
	  	
         while (!(tsec2.rxbd[tsec2_rxIdx].status & RXBD_EMPTY))
         {
         
            
            size_packet_rxtsec2=(tsec2.rxbd[tsec2_rxIdx].length-4)/2;
            printf("+++++++++++++++packet eth2 recieve+++++++++\n\r");
           
           
           // write_to_PLIS2(tsec2.rxbd[tsec2_rxIdx].bufPtr,size_packet_rxtsec2); 
	        //assmble_packet2(size_packet_rxtsec2);   //go_to_tsec2
                      
            //ethernet_tsec1_send_packet(tsec2.rxbd[tsec2_rxIdx].bufPtr,tsec2.rxbd[tsec2_rxIdx].length-4);
           
            //if(tsec1.rxbd[tsec1_rxIdx].length>1500)
            //{ 
              //write_to_PLIS2(tsec2.rxbd[tsec2_rxIdx].bufPtr,(tsec2.rxbd[tsec2_rxIdx].length)/2);
             // assmble_packet2(tr,(tsec1.rxbd[tsec1_rxIdx].length)/2);
             ///}
         	
         	
         	eieio();
            tsec2.rxbd[tsec2_rxIdx].status = RXBD_EMPTY | (((tsec2_rxIdx + 1) == RX_buffer_descriptors_per_ring) ? RXBD_WRAP : 0);
 	   	    tsec2_rxIdx = (tsec2_rxIdx + 1) % RX_buffer_descriptors_per_ring;
               
         }
       
       
   //#if 0    
        while (!(tsec3.rxbd[tsec3_rxIdx].status & RXBD_EMPTY))
	 	{	
        
          
             size_packet_rxtsec3=(tsec3.rxbd[tsec3_rxIdx].length-4)/2;
             // printf("++++++++++++++++\n\r");
        
           // if(tsec1.rxbd[tsec1_rxIdx].length-4==64)
            //{     
            	write_to_PLIS3(tsec3.rxbd[tsec3_rxIdx].bufPtr,size_packet_rxtsec3);
	        	assmble_packet3(size_packet_rxtsec3);   //go_to_tsec2
            //}
			
			
			
			//printf("event_tsec=%x\n",regs->ievent);	        				 
	        //NetReceive(tsec1.rxbd[tsec1_rxIdx].bufPtr, tsec1.rxbd[tsec1_rxIdx].length-4);
	  	    eieio();
	  	    tsec3.rxbd[tsec3_rxIdx].status = RXBD_EMPTY | (((tsec3_rxIdx + 1) == RX_buffer_descriptors_per_ring) ? RXBD_WRAP : 0);
 	   	    tsec3_rxIdx = (tsec3_rxIdx + 1) % RX_buffer_descriptors_per_ring;
	  	
	  	}
      #endif  
        
         
    //#endif  
    
   }
}






int tsec_standard_init(bd_t *bis)
{
	return tsec_eth_init(bis, tsec_info, ARRAY_SIZE(tsec_info));
}


/* Initializes data structures and registers for the controller,
 * and brings the interface up.	 Returns the link status, meaning
 * that it returns success if the link is up, failure otherwise.
 * This allows u-boot to find the first active controller.
 */
int tsec_init(struct eth_device *dev)
{
	uint tempval;
	char tmpbuf[MAC_ADDR_LEN];
	int i;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	 
	puts("+++++++++tsec.c/tsec_init+++++++\n");
	ludelay(100);
	/* Make sure the controller is stopped */
	tsec_halt(dev);

	/*Init MACCFG2.Defaults to GMII */
	//regs->maccfg2 = MACCFG2_INIT_SETTINGS;  // p.733 
      regs->maccfg2 = MACCFG2_INIT_SETTINGS_MPC;  /*Init MACCFG2.Defaults to RMII */
	  
	
	/* Init ECNTRL */
	//regs->ecntrl = ECNTRL_INIT_SETTINGS;
      regs->ecntrl = ECNTRL_INIT_SETTINGS_MPC;   //p.693
	/* Copy the station address into the address registers.
	 * Backwards, because little endian MACS are dumb */
	
	/*
	for (i = 0; i < MAC_ADDR_LEN; i++) 
	{
		tmpbuf[MAC_ADDR_LEN - 1 - i] = dev->enetaddr[i];
	}
	tempval = (tmpbuf[0] << 24) | (tmpbuf[1] << 16) | (tmpbuf[2] << 8) |tmpbuf[3];
	regs->macstnaddr1 = tempval;
	tempval = *((uint *) (tmpbuf + 4));
	regs->macstnaddr2 = tempval;
    */
	//MAC address 0x12345678ABCD
	
	regs->macstnaddr1=0xCDAB7856;   //p.741 MAC stantion address
	regs->macstnaddr2=0x34120000;
	/* reset the indices to zero */
	tsec1_rxIdx = 0;
	tsec1_txIdx = 0;

	tsec2_rxIdx = 0;
	tsec2_txIdx = 0;
	
	tsec3_rxIdx = 0;
	tsec3_txIdx = 0;
	
	
	/* Clear out (for the most part) the other registers */
	init_registers(regs);

	/* Ready the device for tx/rx */
	startup_tsec(dev);

	/* If there's no link, fail */
	return (priv->link ? 0 : -1);
}
/* Initialized required registers to appropriate values, zeroing
 * those we don't care about (unless zero is bad, in which case,
 * choose a more appropriate value)
 */
static void init_registers(volatile tsec_t * regs)
{
	/* Clear IEVENT */
	regs->ievent = IEVENT_INIT_CLEAR;

	regs->imask = IMASK_INIT_CLEAR;

	regs->hash.iaddr0 = 0;
	regs->hash.iaddr1 = 0;
	regs->hash.iaddr2 = 0;
	regs->hash.iaddr3 = 0;
	regs->hash.iaddr4 = 0;
	regs->hash.iaddr5 = 0;
	regs->hash.iaddr6 = 0;
	regs->hash.iaddr7 = 0;

	regs->hash.gaddr0 = 0;
	regs->hash.gaddr1 = 0;
	regs->hash.gaddr2 = 0;
	regs->hash.gaddr3 = 0;
	regs->hash.gaddr4 = 0;
	regs->hash.gaddr5 = 0;
	regs->hash.gaddr6 = 0;
	regs->hash.gaddr7 = 0;

	//RCTRL_regs///////////////
	 //regs->rctrl = 0x00000000;               /////////////p.713
     regs->rctrl = RCTRL_PromiscEN;
	 puts("++++++++PromiscMode_Enable+++++\n");
	
	/* Init RMON mib registers */
	memset((void *)&(regs->rmon), 0, sizeof(rmon_mib_t));

	regs->rmon.cam1 = 0xffffffff;
	regs->rmon.cam2 = 0xffffffff;

	
	
	regs->mrblr =  MRBLR_INIT_SETTINGS;   //Maximum Receive Buffer Length Register (MRBLR) p.725
	regs->minflr = MINFLR_INIT_SETTINGS;

	regs->attr    = ATTR_INIT_SETTINGS_MPC;  //Allocate in L2 cache_memory
	
	
//	regs->attr    = ATTR_INIT_SETTINGS;       //Attribute Register (ATTR)  p.773
	regs->attreli = ATTRELI_INIT_SETTINGS; //Attribute Extract Length and Extract Index Register (ATTRELI) p.774

}



/* Writes the given phy's reg with value, using the specified MDIO regs */
static void tsec_local_mdio_write(volatile tsec_mdio_t *phyregs, uint addr,uint reg, uint value)
{
	int timeout = 1000000;

	phyregs->miimadd = (addr << 8) | reg;
	phyregs->miimcon = value;
	//asm("sync");
    asm("msync");

	timeout = 1000000;
	while ((phyregs->miimind & MIIMIND_BUSY) && timeout--) ;
}



/* Provide the default behavior of writing the PHY of this ethernet device */
#define write_phy_reg(priv, regnum, value) tsec_local_mdio_write(priv->phyregs,priv->phyaddr,regnum,value)

/* Reads register regnum on the device's PHY through the
 * specified registers.	 It lowers and raises the read
 * command, and waits for the data to become valid (miimind
 * notvalid bit cleared), and the bus to cease activity (miimind
 * busy bit cleared), and then returns the value
 */
uint tsec_local_mdio_read(volatile tsec_mdio_t *phyregs, uint phyid, uint regnum)
{
	uint value;
    ludelay(1000);
	/* Put the address of the phy, and the register
	 * number into MIIMADD */
	phyregs->miimadd = (phyid << 8) | regnum;

	/* Clear the command register, and wait */
	phyregs->miimcom = 0;
	asm("msync");

	/* Initiate a read command, and wait */
	phyregs->miimcom = MIIM_READ_COMMAND;
	asm("msync");

	/* Wait for the the indication that the read is done */
	while ((phyregs->miimind & (MIIMIND_NOTVALID | MIIMIND_BUSY))) ;

	/* Grab the value read from the PHY */
	value = phyregs->miimstat;

	return value;
}

/* #define to provide old read_phy_reg functionality without duplicating code */
#define read_phy_reg(priv,regnum) tsec_local_mdio_read(priv->phyregs,priv->phyaddr,regnum)

#define TBIANA_SETTINGS ( \
		TBIANA_ASYMMETRIC_PAUSE \
		| TBIANA_SYMMETRIC_PAUSE \
		| TBIANA_FULL_DUPLEX \
		)

#define TBICR_SETTINGS ( \
		TBICR_PHY_RESET \
		| TBICR_ANEG_ENABLE \
		| TBICR_FULL_DUPLEX \
		| TBICR_SPEED1_SET \
		)
/* Configure the TBI for SGMII operation */
static void tsec_configure_serdes(struct tsec_private *priv)
{
	/* Access TBI PHY registers at given TSEC register offset as opposed to the
	 * register offset used for external PHY accesses */
	tsec_local_mdio_write(priv->phyregs_sgmii, priv->regs->tbipa, TBI_ANA,TBIANA_SETTINGS);
	tsec_local_mdio_write(priv->phyregs_sgmii, priv->regs->tbipa, TBI_TBICON,TBICON_CLK_SELECT);
	tsec_local_mdio_write(priv->phyregs_sgmii, priv->regs->tbipa, TBI_CR,TBICR_SETTINGS);
}

/* Discover which PHY is attached to the device, and configure it
 * properly.  If the PHY is not recognized, then return 0
 * (failure).  Otherwise, return 1
 */
static int init_phy(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	struct phy_info *curphy;
	volatile tsec_mdio_t *phyregs = priv->phyregs;
	volatile tsec_t *regs = priv->regs;
    ludelay(1000);
	/* Assign a Physical address to the TBI */
	regs->tbipa = CONFIG_SYS_TBIPA_VALUE;
	asm("msync");
    //printf("+++++++++tsec.c/init_phy+++++++\n\r");
	puts("+++++++++tsec.c/init_phy+++++++\n\r");
	/* Reset MII (due to new addresses) */
	priv->phyregs->miimcfg = MIIMCFG_RESET;
	asm("msync");
	priv->phyregs->miimcfg = MIIMCFG_INIT_VALUE;
	asm("msync");
	while (priv->phyregs->miimind & MIIMIND_BUSY) ;

	
	
	/* Get the cmd structure corresponding to the attached
	 * PHY */
	curphy = get_phy_info(dev);
    
	if (curphy == NULL) 
	{
		priv->phyinfo = NULL;
		//printf("%s: No PHY found\n", dev->name);
		puts("No PHY found\n\r");
		return 0;
	}

	/*
	if (regs->ecntrl & ECNTRL_SGMII_MODE)
		tsec_configure_serdes(priv);*/

	priv->phyinfo = curphy;
	phy_run_commands(priv, priv->phyinfo->config);
   
	return 1;
}

/*
 * Returns which value to write to the control register.
 * For 10/100, the value is slightly different
 */
uint mii_cr_init(uint mii_reg, struct tsec_private * priv)
{
	if (priv->flags & TSEC_GIGABIT)
		return MIIM_CONTROL_INIT;
	else
		return MIIM_CR_INIT;
}

/*
 * Wait for auto-negotiation to complete, then determine link
 */
uint mii_parse_sr(uint mii_reg, struct tsec_private * priv)
{
	/*
	 * Wait if the link is up, and autonegotiation is in progress
	 * (ie - we're capable and it's not done)
	 */
	
	ludelay(10000);
	
	//asm("stop");
	mii_reg = read_phy_reg(priv, MIIM_STATUS);
	//printf ("+++++++++tsec.c/mii_parse_sr+++++++++\n\r"); 
	  puts ("+++++++++tsec.c/mii_parse_sr+++++++++\n\r"); 
	
	
	if ((mii_reg & PHY_BMSR_AUTN_ABLE) && !(mii_reg & PHY_BMSR_AUTN_COMP)) 
	{
		int i = 0;   
		//puts("Waiting for PHY auto negotiation to complete");
		//printf("Waiting for PHY auto negotiation to complete");  
		  //puts("+++++++++tsec.c/mii_parse_sr+++++++++\n\r"); 
		
		while (!(mii_reg & PHY_BMSR_AUTN_COMP)) {
			/*
			 * Timeout reached ?
			 */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				  puts(" TIMEOUT !\n\r");
				//printf(" TIMEOUT !\n");  
				priv->link = 0;
				return 0;
			}


        //27.11.2012 comment_kosta
           /*
			if (ctrlc())
			{
				 puts("user interrupt!\n\r");
				//printf("user interrupt!\n");
				priv->link = 0;
				return -EINTR;
			}
            */
           

			if ((i++ % 1000) == 0)
			{
				//printf('.');
				  puts(".");
			 
			}
			ludelay(1000);
			//udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_STATUS);
		}
		    puts("done\n\r");
         // printf(" done\n");
		
		/* Link status bit is latched low, read it again */
		mii_reg = read_phy_reg(priv, MIIM_STATUS);
        ludelay(500000);

	//	udelay(500000);	/* another 500 ms (results in faster booting) */
	}

	priv->link = mii_reg & MIIM_STATUS_LINK ? 1 : 0;

	return 0;
}

/* Generic function which updates the speed and duplex.  If
 * autonegotiation is enabled, it uses the AND of the link
 * partner's advertised capabilities and our advertised
 * capabilities.  If autonegotiation is disabled, we use the
 * appropriate bits in the control register.
 *
 * Stolen from Linux's mii.c and phy_device.c
 */
uint mii_parse_link(uint mii_reg, struct tsec_private *priv)
{
	/* We're using autonegotiation */
	if (mii_reg & PHY_BMSR_AUTN_ABLE) {
		uint lpa = 0;
		uint gblpa = 0;

		/* Check for gigabit capability */
		if (mii_reg & PHY_BMSR_EXT) {
			/* We want a list of states supported by
			 * both PHYs in the link
			 */
			gblpa = read_phy_reg(priv, PHY_1000BTSR);
			gblpa &= read_phy_reg(priv, PHY_1000BTCR) << 2;
		}

		/* Set the baseline so we only have to set them
		 * if they're different
		 */
		priv->speed = 10;
		priv->duplexity = 0;

		/* Check the gigabit fields */
		if (gblpa & (PHY_1000BTSR_1000FD | PHY_1000BTSR_1000HD)) {
			priv->speed = 1000;

			if (gblpa & PHY_1000BTSR_1000FD)
				priv->duplexity = 1;

			/* We're done! */
			return 0;
		}

		lpa = read_phy_reg(priv, PHY_ANAR);
		lpa &= read_phy_reg(priv, PHY_ANLPAR);

		if (lpa & (PHY_ANLPAR_TXFD | PHY_ANLPAR_TX)) {
			priv->speed = 100;

			if (lpa & PHY_ANLPAR_TXFD)
				priv->duplexity = 1;

		} else if (lpa & PHY_ANLPAR_10FD)
			priv->duplexity = 1;
	} else {
		uint bmcr = read_phy_reg(priv, PHY_BMCR);

		priv->speed = 10;
		priv->duplexity = 0;

		if (bmcr & PHY_BMCR_DPLX)
			priv->duplexity = 1;

		if (bmcr & PHY_BMCR_1000_MBPS)
			priv->speed = 1000;
		else if (bmcr & PHY_BMCR_100_MBPS)
			priv->speed = 100;
	}

	return 0;
}






/* Configure maccfg2 based on negotiated speed and duplex
 * reported by PHY handling code
 */
static void adjust_link(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	//printf("+++++++++tsec.c/adjust_link+++++++++\n\r"); 
	puts("+++++++++tsec.c/adjust_link+++++++++\n");
	//eth_get_dev_index();
	if (priv->link) 
	{
		if (priv->duplexity != 0)
			regs->maccfg2 |= MACCFG2_FULL_DUPLEX;
		else
			regs->maccfg2 &= ~(MACCFG2_FULL_DUPLEX);

		switch (priv->speed)
		{
		    case 1000:
			         regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))| MACCFG2_GMII);
			break;
		
		
		    case 100:
		    case 10:
			       regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))| MACCFG2_MII);
				   puts ("case 10: regs->maccfg2 \n\r");
                   /* Set R100 bit in all modes although
			        * it is only used in RGMII mode
			        */
			       if (priv->speed == 100)
				   regs->ecntrl |= ECNTRL_R100; 
				   else
				   regs->ecntrl &= ~(ECNTRL_R100);
			       break;
		
		    default:
			//printf("%s: Speed was bad\n", dev->name);
			  puts ("Tsec_Speed was bad\n");
			 break;
		}

		//printf("Speed: %d, %s duplex\n", priv->speed,(priv->duplexity) ? "full" : "half");
        //puts ("Speed:\n\r");
	} 
	else 
	{
		//printf("%s: No link.\n", dev->name);
	      puts ("Tsec_Link is down\n");
	};
}

/* Set up the buffers and their descriptors, and bring up the
 * interface
 */
static void startup_tsec(struct eth_device *dev)
{
	int i,in_phyaddr;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	puts("+++++++++tsec.c/startup_tsec+++++++\n");
    in_phyaddr=priv->phyaddr;
	

	
	if(in_phyaddr==1)
	{
	     puts("buffer_descriptor_point_TSEC1!!!!!!!!\n");
	     // Point to the buffer descriptors 
	     regs->tbase = (unsigned int)(&tsec1.txbd[tsec1_txIdx]);
	     regs->rbase = (unsigned int)(&tsec1.rxbd[tsec1_rxIdx]);

	     //Initialize the Rx Buffer descriptors
	     for (i = 0; i < RX_buffer_descriptors_per_ring; i++)
	     {
		 tsec1.rxbd[i].status = RXBD_EMPTY;
		 tsec1.rxbd[i].length = 0;
		 tsec1.rxbd[i].bufPtr =0;//RxPacet[i];
	     }
	     tsec1.rxbd[RX_buffer_descriptors_per_ring - 1].status |= RXBD_WRAP;

	
	    //Initialize the TX Buffer Descriptors
	    for (i = 0; i < TX_buffer_descriptors_per_ring; i++) 
	    {
		tsec1.txbd[i].status = 0;
		tsec1.txbd[i].length = 0;
		tsec1.txbd[i].bufPtr = 0;
	    }
	    tsec1.txbd[TX_buffer_descriptors_per_ring - 1].status |= TXBD_WRAP;

	}
	
	if(in_phyaddr==2)
	{
	     puts("buffer_descriptor_point_TSEC2!!!!!!!!\n");
	     // Point to the buffer descriptors 
	     regs->tbase = (unsigned int)(&tsec2.txbd[tsec2_txIdx]);
	     regs->rbase = (unsigned int)(&tsec2.rxbd[tsec2_rxIdx]);

	     //Initialize the Rx Buffer descriptors
	     for (i = 0; i < RX_buffer_descriptors_per_ring; i++)
	     {
		 tsec2.rxbd[i].status = RXBD_EMPTY;
		 tsec2.rxbd[i].length = 0;
		 tsec2.rxbd[i].bufPtr = 0;
	     }
	     tsec2.rxbd[RX_buffer_descriptors_per_ring - 1].status |= RXBD_WRAP;

	
	    //Initialize the TX Buffer Descriptors
	    for (i = 0; i < TX_buffer_descriptors_per_ring; i++) 
	    {
		tsec2.txbd[i].status = 0;
		tsec2.txbd[i].length = 0;
		tsec2.txbd[i].bufPtr = 0;
	    }
	    tsec2.txbd[TX_buffer_descriptors_per_ring - 1].status |= TXBD_WRAP;

	}
	
	
	if(in_phyaddr==3)
	{
	     puts("buffer_descriptor_point_TSEC3!!!!!!!!\n");
	     // Point to the buffer descriptors 
	     regs->tbase = (unsigned int)(&tsec3.txbd[tsec3_txIdx]);
	     regs->rbase = (unsigned int)(&tsec3.rxbd[tsec3_rxIdx]);

	     //Initialize the Rx Buffer descriptors
	     for (i = 0; i < RX_buffer_descriptors_per_ring; i++)
	     {
		 tsec3.rxbd[i].status = RXBD_EMPTY;
		 tsec3.rxbd[i].length = 0;
		 tsec3.rxbd[i].bufPtr = 0;
	     }
	     tsec3.rxbd[RX_buffer_descriptors_per_ring - 1].status |= RXBD_WRAP;

	
	    //Initialize the TX Buffer Descriptors
	    for (i = 0; i < TX_buffer_descriptors_per_ring; i++) 
	    {
		tsec3.txbd[i].status = 0;
		tsec3.txbd[i].length = 0;
		tsec3.txbd[i].bufPtr = 0;
	    }
	    tsec3.txbd[TX_buffer_descriptors_per_ring - 1].status |= TXBD_WRAP;

	}
		
	
	// Start up the PHY
	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->startup);

	adjust_link(dev);

	// Enable Transmit and Receive 
	regs->maccfg1 |= (MACCFG1_RX_EN | MACCFG1_TX_EN);
     
	  //Set Internal Tsec Loopback Mode    tx->rx
	 // regs->maccfg1 |= (MACCFG1_RX_EN | MACCFG1_TX_EN|MACCFG1_LOOPBACK);
	
	// Tell the DMA it is clear to go
	regs->dmactrl |= DMACTRL_INIT_SETTINGS;
	regs->tstat = TSTAT_CLEAR_THALT;
	regs->rstat = RSTAT_CLEAR_RHALT;
	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
}

/* This returns the status bits of the device.	The return value
 * is never checked, and this is what the 8260 driver did, so we
 * do the same.	 Presumably, this would be zero if there were no
 * errors
 */



/*
void tsec_send(struct eth_device *dev2,struct eth_device *dev1)
//void tsec_send(struct eth_device *dev, volatile void *packet, int length)
{

    u16 super_mas[128];
    
    
    ludelay(100000);
    //volatile tsec_t *regs2=priv[2].regs;
    
  
    //printf("tr_event=%x\n",regs2->ievent);
    tsec2.txbd[tsec2_txIdx].bufPtr = (unsigned int)mas;
	tsec2.txbd[tsec2_txIdx].length = 0x80; 
    tsec2.txbd[tsec2_txIdx].status |=(TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);
    asm volatile ("msync");
	priv[2].regs->tstat = TSTAT_CLEAR_THALT;  
	tsec2_txIdx = (tsec2_txIdx + 1) % TX_buffer_descriptors_per_ring; 
		


}
*/









/*//////////////// Stop the eTSEC interface ////////////////////*/
static void tsec_halt(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
 
	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
	regs->dmactrl |= (DMACTRL_GRS | DMACTRL_GTS);
     //printf("+++++++++tsec.c/tsec_halt+++++++\n\r"); 
	 puts("+++++++++tsec.c/tsec_halt+++++++\n\r");
	while ((regs->ievent & (IEVENT_GRSC | IEVENT_GTSC))
			!= (IEVENT_GRSC | IEVENT_GTSC)) ;

	regs->maccfg1 &= ~(MACCFG1_TX_EN | MACCFG1_RX_EN);

	/* Shut down the PHY, as needed */
	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->shutdown);
}









/* Parse the vsc8244's status register for speed and duplex
 * information
 */
uint mii_parse_vsc8244(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	if (mii_reg & MIIM_VSC8244_AUXCONSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = mii_reg & MIIM_VSC8244_AUXCONSTAT_SPEED;
	switch (speed) 
	{
	case MIIM_VSC8244_AUXCONSTAT_GBIT:
		priv->speed = 1000;
		puts("vsc8244_SPEED=1000M/bit\n\r");
		break;
	case MIIM_VSC8244_AUXCONSTAT_100:
		priv->speed = 100;
		puts("vsc8244_SPEED=100M/bit\n\r");
		break;
	default:
		priv->speed = 10;
		puts("vsc8244_SPEED=10M/bit\n\r");
		break;
	}

	return 0;
}


struct phy_info phy_info_VSC8641 = {
	0x7043,
	"Vitesse VSC8641",
	4,
	(struct phy_cmd[]){	/* config */
			   /* Configure some basic stuff */
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Read the Status (2x to make sure link is right) */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_VSC8244_AUX_CONSTAT, miim_read,
			    &mii_parse_vsc8244},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

struct phy_info phy_info_VSC8221 = {
	0xfc55,
	"Vitesse VSC8221",
	4,
	(struct phy_cmd[]){	/* config */
			   /* Configure some basic stuff */
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Read the Status (2x to make sure link is right) */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_VSC8244_AUX_CONSTAT, miim_read,
			    &mii_parse_vsc8244},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};



/* a generic flavor.  */
struct phy_info phy_info_generic =  {
	0,
	"Unknown/Generic PHY",
	32,
	(struct phy_cmd[]) { /* config */
		{PHY_BMCR, PHY_BMCR_RESET, NULL},
		{PHY_BMCR, PHY_BMCR_AUTON|PHY_BMCR_RST_NEG, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		{PHY_BMSR, miim_read, NULL},
		{PHY_BMSR, miim_read, &mii_parse_sr},
		{PHY_BMSR, miim_read, &mii_parse_link},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	}
};


/* Parse the DP83848's link and auto-neg status register for speed and duplex
 * information
 */
uint mii_parse_dp83848_lanr(uint mii_reg, struct tsec_private *priv)
{
  //printf("DP83848 PHY_STATUS_REGISTER= %x\n",mii_reg);
  //printf("+++++++++tsec.c/mii_parse_dp83848_lanr+++++++\n\r");
    puts ("+++++++++tsec.c/mii_parse_dp83848_lanr+++++++\n\r");
	////////////////////////////Link/////////////////////////////////
	if (mii_reg & MIIM_DP83848_LINK_MASK_MASK)
	{
	puts("dp83848_Link is UP\n\r"); 
	}
	else	
	{
	puts("dp83848_Link is down\n\r");  
	return 0;
	}
	
    ////////////////////////////////////Speed////////////////////////////////////////////
	switch (mii_reg & MIIM_DP83848_SPD_MASK ) {
	case MIIM_DP83848_SPD_100:
		priv->speed = 100;
		puts("dp83848_SPEED=100M/bit\n\r");
		break;

	case MIIM_DP83848_SPD_10:
		priv->speed = 10;
		puts("dp83848_SPEED=10M/bit\n\r");
		break;

	default:
		priv->speed = 10;
		puts("dp83848_Error SPEED=default\n\r");
		break;

	}

    ///////////////////////////////DUplex//////////////////////////
	if (mii_reg & MIIM_DP83848_DPX_MASK)
	{
		priv->duplexity = 1; //FULL DUPLEX
	    puts("dp83848_FULL DUPLEX\n\r"); 
	}
	else
	{
		priv->duplexity = 0; //HALF DUPLEX
        puts("dp83848_HALF DUPLEX\n\r"); 
    }

	    ////////////////////////////LoopBaCK MODE/////////////////////////////////
	if (mii_reg & MIIM_DP83848_LOOPBACK_MASK)
	{puts("dp83848_Loopback enabled.\n\r"); }
	else
	{puts("dp83848_Normal operation.\n\r"); }
 
 return 0;
}

struct phy_info phy_info_dp83848 = {
	0x20005c90,
	"NatSemi DP83848",
	0,
	(struct phy_cmd[]){	// config 
			   {MIIM_CONTROL, MIIM_DP83848_CR_INIT, NULL},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	// startup
			   // Status is read once to clear old link state 
			   {MIIM_STATUS, miim_read, NULL},
			   // Auto-negotiate 
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   // Read the link and auto-neg status 
			   {PHY_DP83848_STATUS_REGISTER , miim_read,&mii_parse_dp83848_lanr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	// shutdown 
			   {miim_end,}
			   },
};




/* Parse the DP83865's link and auto-neg status register for speed and duplex
 * information
 */
uint mii_parse_dp83865_lanr(uint mii_reg, struct tsec_private *priv)
{
	switch (mii_reg & MIIM_DP83865_SPD_MASK) {

	case MIIM_DP83865_SPD_1000:
		priv->speed = 1000;
		break;

	case MIIM_DP83865_SPD_100:
		priv->speed = 100;
		break;

	default:
		priv->speed = 10;
		break;

	}

	if (mii_reg & MIIM_DP83865_DPX_FULL)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	return 0;
}

struct phy_info phy_info_dp83865 = {
	0x20005c7,
	"NatSemi DP83865",
	4,
	(struct phy_cmd[]){	/* config */
			   {MIIM_CONTROL, MIIM_DP83865_CR_INIT, NULL},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the link and auto-neg status */
			   {MIIM_DP83865_LANR, miim_read,
			    &mii_parse_dp83865_lanr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};


struct phy_info *phy_info[] = {
	&phy_info_VSC8641,
	&phy_info_VSC8221,
	&phy_info_dp83865,
	&phy_info_dp83848,
	&phy_info_generic,	/* must be last; has ID 0 and 32 bit mask */
	NULL
};



/* Grab the identifier of the device's PHY, and search through
 * all of the known PHYs to see if one matches.	 If so, return
 * it, if not, return NULL
 */
struct phy_info *get_phy_info(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	uint phy_reg, phy_ID;
	int i;
	struct phy_info *theInfo = NULL;
    //printf("+++++++++tsec.c/get_phy_info+++++++\n\r");
	  puts("+++++++++tsec.c/get_phy_info+++++++\n\r");
	/* Grab the bits from PHYIR1, and put them in the upper half */
	phy_reg = read_phy_reg(priv, MIIM_PHYIR1);
	phy_ID = (phy_reg & 0xffff) << 16;
    //printf("PHY_ID_REGISTER1=%x\n",phy_reg);
	/* Grab the bits from PHYIR2, and put them in the lower half */
	phy_reg = read_phy_reg(priv, MIIM_PHYIR2);
	phy_ID |= (phy_reg & 0xffff);
    //printf("PHY_ID_REGISTER2=%x\n",phy_reg);
	/* loop through all the known PHY types, and find one that */
	/* matches the ID we read from the PHY. */
	for (i = 0; phy_info[i]; i++) 
	{
		if (phy_info[i]->id == (phy_ID >> phy_info[i]->shift)) 
		{
			theInfo = phy_info[i];
			break;
		}
	}

	if (theInfo == &phy_info_generic) 
	{
		//printf("%s: No support for PHY id %x; assuming generic\n", dev->name, phy_ID);
	    puts("No support for PHY id\n\r"); 
	} else 
	{
		//debug("%s: PHY is %s (%x)\n", dev->name, theInfo->name, phy_ID);
	      //printf("%s: PHY is %s (%x)\n", dev->name, theInfo->name, phy_ID);
	     puts("----------------------PHY is Complete initialize------------------\n\r");
	}

	return theInfo;
}

/* Execute the given series of commands on the given device's
 * PHY, running functions as necessary
 */
void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd)
{
	int i;
	uint result;
	volatile tsec_mdio_t *phyregs = priv->phyregs;

	phyregs->miimcfg = MIIMCFG_RESET;
	phyregs->miimcfg = MIIMCFG_INIT_VALUE;

	puts("++++++++tsec.c/phy_run_commands+++++++++++\n\r"); 
	 
	while (phyregs->miimind & MIIMIND_BUSY) ;

	for (i = 0; cmd->mii_reg != miim_end; i++) {
		if (cmd->mii_data == miim_read) {
			result = read_phy_reg(priv, cmd->mii_reg);

			if (cmd->funct != NULL)
				(*(cmd->funct)) (result, priv);

		} else {
			if (cmd->funct != NULL)
				result = (*(cmd->funct)) (cmd->mii_reg, priv);
			else
				result = cmd->mii_data;

			write_phy_reg(priv, cmd->mii_reg, result);

		}
		cmd++;
	}
}

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) \
	&& !defined(BITBANGMII)

/*
 * Read a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_read(char *devname, unsigned char addr,unsigned char reg, unsigned short *value)
{
	unsigned short ret;
	struct tsec_private *priv = privlist[0];
    //printf("+++++++++tsec.c/tsec_miiphy_read+++++++\n\r");  
	  puts("+++++++++tsec.c/tsec_miiphy_read+++++++\n\r");
	if (NULL == priv) {
		//printf("Can't read PHY at address %d\n", addr);
		  puts("Can't read PHY at address\n\r");
		return -1;
	}

	ret = (unsigned short)tsec_local_mdio_read(priv->phyregs, addr, reg);
	*value = ret;

	return 0;
}

/*
 * Write a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_write(char *devname, unsigned char addr,unsigned char reg, unsigned short value)
{
	struct tsec_private *priv = privlist[0];
    //printf("+++++++++tsec.c/tsec_miiphy_write+++++++\n\r"); 
	  puts("+++++++++tsec.c/tsec_miiphy_write+++++++\n\r");
	if (NULL == priv) {
		//printf("Can't write PHY at address %d\n", addr);
		   puts("Can't write PHY at address\n\r");
		return -1;
	}

	tsec_local_mdio_write(priv->phyregs, addr, reg, value);

	return 0;
}

#endif

#ifdef CONFIG_MCAST_TFTP

/* CREDITS: linux gianfar driver, slightly adjusted... thanx. */

/* Set the appropriate hash bit for the given addr */

/* The algorithm works like so:
 * 1) Take the Destination Address (ie the multicast address), and
 * do a CRC on it (little endian), and reverse the bits of the
 * result.
 * 2) Use the 8 most significant bits as a hash into a 256-entry
 * table.  The table is controlled through 8 32-bit registers:
 * gaddr0-7.  gaddr0's MSB is entry 0, and gaddr7's LSB is
 * gaddr7.  This means that the 3 most significant bits in the
 * hash index which gaddr register to use, and the 5 other bits
 * indicate which bit (assuming an IBM numbering scheme, which
 * for PowerPC (tm) is usually the case) in the tregister holds
 * the entry. */
static int tsec_mcast_addr (struct eth_device *dev, u8 mcast_mac, u8 set)
{
 struct tsec_private *priv = privlist[1];
 volatile tsec_t *regs = priv->regs;
 volatile u32  *reg_array, value;
 u8 result, whichbit, whichreg;

	result = (u8)((ether_crc(MAC_ADDR_LEN,mcast_mac) >> 24) & 0xff);
	whichbit = result & 0x1f;	/* the 5 LSB = which bit to set */
	whichreg = result >> 5;		/* the 3 MSB = which reg to set it in */
	value = (1 << (31-whichbit));

	reg_array = &(regs->hash.gaddr0);

	if (set) {
		reg_array[whichreg] |= value;
	} else {
		reg_array[whichreg] &= ~value;
	}
	return 0;
}
#endif /* Multicast TFTP ? */
