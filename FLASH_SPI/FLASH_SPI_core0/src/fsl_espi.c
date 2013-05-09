/*
 * eSPI controller driver.
 *
 * Copyright (C) 2009-2010 Freescale Semiconductor, Inc. All rights reserved.
 * Author: Mingkai Hu (Mingkai.hu@freescale.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
#include <common.h>
#include <malloc.h>
#include <spi.h>                          uncomment linux
#include <asm/immap_85xx.h>
*/
#include <stdio.h>
#include "common.h"
#include "malloc.h"
#include "spi.h"
#include "asm\immap_85xx.h"
#include "processor.h"

static int transaction=0;
static int j=0;

#define MPC85xx_ESPI_ADDR    0xE0007000

#define ESPI_MAX_CS_NUM		4

#define ESPI_EV_RNE		(1 << 9)
#define ESPI_EV_TNF		(1 << 8)

#define ESPI_MODE_EN		(1 << 31)	/* Enable interface */
#define ESPI_MODE_TXTHR(x)	((x) << 8)	/* Tx FIFO threshold */
#define ESPI_MODE_RXTHR(x)	((x) << 0)	/* Rx FIFO threshold */

#define ESPI_COM_CS(x)		((x) << 30)
#define ESPI_COM_TRANLEN(x)	((x) << 0)

#define ESPI_CSMODE_CI_INACTIVEHIGH	(1 << 31)
#define ESPI_CSMODE_CP_BEGIN_EDGCLK	(1 << 30)
#define ESPI_CSMODE_REV_MSB_FIRST	(1 << 29)
#define ESPI_CSMODE_DIV16		(1 << 28)
#define ESPI_CSMODE_PM(x)		((x) << 24)
#define ESPI_CSMODE_POL_ASSERTED_LOW	(1 << 20)
#define ESPI_CSMODE_LEN(x)		((x) << 16)
#define ESPI_CSMODE_CSBEF(x)		((x) << 12)
#define ESPI_CSMODE_CSAFT(x)		((x) << 8)
#define ESPI_CSMODE_CSCG(x)		((x) << 3)

#define ESPI_CSMODE_INIT_VAL (ESPI_CSMODE_POL_ASSERTED_LOW | \
		ESPI_CSMODE_CSBEF(0) | ESPI_CSMODE_CSAFT(0) | \
		ESPI_CSMODE_CSCG(1))

#define ESPI_MAX_DATA_TRANSFER_LEN 0xFFF0

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,unsigned int max_hz, unsigned int mode)
{
    //заполнение структруры но адресс другой!!!!!
    //volatile ccsr_espi_t *espi = (void *)(CONFIG_SYS_MPC85xx_ESPI_ADDR);
    #define MHZ50        50000000 
    #define MHZ66        66666666
    #define MHZ100       100000000
    #define MHZ400       400000000
    #define MHZ533       533333333
    #define MHZ667       666666666
    #define MHZ800       800000000
         
	volatile ccsr_espi_t *espi = (void *)(MPC85xx_ESPI_ADDR);
	struct spi_slave *slave;
	
	
	
	//Структура с данными процессора
	sys_info_t sysinfo;
	unsigned long spibrg = 0;
	unsigned char pm = 0;
	int i;
	
	
	
    //Проверка еа валидность Chip select 
	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	slave = malloc(sizeof(struct spi_slave));
	if (!slave)
		return NULL;

	slave->bus = bus;
	slave->cs = cs;
	slave->max_transfer_length = ESPI_MAX_DATA_TRANSFER_LEN;

	// Enable eSPI interface 
	espi->mode = ESPI_MODE_RXTHR(3) | ESPI_MODE_TXTHR(4) | ESPI_MODE_EN;

	espi->event = 0xffffffff;	// Clear all eSPI events 
	espi->mask = 0x00000000;	// Mask  all eSPI interrupts

	// Init CS mode interface 
	for (i = 0; i < ESPI_MAX_CS_NUM; i++)
		espi->csmode[i] = ESPI_CSMODE_INIT_VAL;

	espi->csmode[cs] &= ~(ESPI_CSMODE_PM(0xF) | ESPI_CSMODE_DIV16
		| ESPI_CSMODE_CI_INACTIVEHIGH | ESPI_CSMODE_CP_BEGIN_EDGCLK
		| ESPI_CSMODE_REV_MSB_FIRST | ESPI_CSMODE_LEN(0xF));

    
     //В дальнейшем используем эту структуру.
     
	// Set eSPI BRG clock source
	 get_sys_info(&sysinfo);
	 spibrg = sysinfo.freqSystemBus / 2;
	
	//spibrg =MHZ50 / 2;
	if ((spibrg / max_hz) > 32) {
		espi->csmode[cs] |= ESPI_CSMODE_DIV16;
		pm = spibrg / (max_hz * 16 * 2);
		if (pm > 16) {
			pm = 16;
			//debug("Requested speed is too low: %d Hz" " %d Hz is used.\n", max_hz, spibrg / (32 * 16));
		    printf("Requested speed is too low: %d Hz" " %d Hz is used.\n", max_hz, spibrg / (32 * 16));
		}
	} else
		pm = spibrg / (max_hz * 2);
	if (pm)
		pm--;
	espi->csmode[cs] |= ESPI_CSMODE_PM(pm);

	// Set eSPI mode 
	if (mode & SPI_CPHA)
		espi->csmode[cs] |= ESPI_CSMODE_CP_BEGIN_EDGCLK;
	if (mode & SPI_CPOL)
		espi->csmode[cs] |= ESPI_CSMODE_CI_INACTIVEHIGH;

	// Character bit order: msb first 
	espi->csmode[cs] |= ESPI_CSMODE_REV_MSB_FIRST;

	// Character length in bits, between 0x3~0xf, i.e. 4bits~16bits 
	espi->csmode[cs] |= ESPI_CSMODE_LEN(7);
    
	return slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	//free_task_struct(slave);
}

void spi_init(void)
{

}

int spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{

}

int espi_xfer(struct spi_slave *slave)
{   
	//volatile ccsr_espi_t *espi = (void *)(CONFIG_SYS_MPC85xx_ESPI_ADDR);  
    unsigned int tmpdout, tmpdin, event;  
    const unsigned char *dout;
    unsigned int *din=NULL;  
    unsigned long flags;
    
    //unsigned int *ch;
    unsigned int num_bytes=0;
    unsigned int l_len=0;
    unsigned int numBlks=0;
    //Структуры
    volatile ccsr_espi_t *espi; 
    
    //struct espi_transfer *t;
    //uk_p=malloc(sizeof(din));
     
	 
	//struct espi_transfer *t = slave->transfer;
	
	//struct espi_transfer *t= malloc(sizeof(struct espi_transfer)); ;   
	//unsigned int len = t->cmd_len + t->data_len;
	
	//unsigned int len =9; //t->cmd_len + t->data_len;
	
	//unsigned char *l_out;
	
	
    //len=9;
	espi = (void *)(MPC85xx_ESPI_ADDR);
	//t=malloc(sizeof(struct espi_transfer)); 
   
	
   //unsigned int my_out[4];
	
	//Excelent variant
	 //const void *dout = t->tx_buf;
	//void *din = t->rx_buf;
   //printf("DELAY\n"); 
   l_len=slave->transfer->cmd_len+slave->transfer->data_len;	
   dout=slave->transfer->tx_buf;
   /*
   din=malloc(sizeof(l_len));
   if(!din)
   {
   	printf("ERROR ALLOCATE memory for din\n");
   }
   */	 
	     //dout=t->tx_buf;	   	  
	     
	     
   //din = t->rx_buf;	
     
     
    
    	 //flags = t->flags;
    	   flags =slave->transfer->flags;
           numBlks = l_len/ 4 + (l_len % 4 ? 1 : 0);
           num_bytes = l_len % 4;

     //din =malloc(numBlks*sizeof(u32)); 
      

    //t = malloc(sizeof(struct espi_transfer));
    //const unsigned char *dout=t->tx_buf;
    
	/*if (t->data_len > ESPI_MAX_DATA_TRANSFER_LEN)
		return -1;*/
    	
	
//	printf("numBlks=%d,num_bytes=%d ,len=%d\n",numBlks,num_bytes,len);
    
	
//	for(j=0;j<numBlks;j++)
//	printf("[%d]   = %08X\n ",j,dout[j]); 
	
	
    
	
	

	//debug("spi_xfer: slave %u:%u dout %08X(%08x) din %08X(%08x) len %u\n",
	//      slave->bus, slave->cs, *(uint *) dout, dout, *(uint *) din, din, len);
   
    //printf("spi_xfer: slave %u:%u dout %08X(%08x) din %08X(%08x) len %u\n",slave->bus, slave->cs, *(uint *) dout, dout, *(uint *) din, din, len); 
    
    
    
	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	// Clear all eSPI events 
	espi->event = 0xffffffff;  
	// handle data in 32-bit chunks 
	while (numBlks--) 
	{
        
		event = espi->event;
		if (event & ESPI_EV_TNF) 
		{
		  //Правильный вариант работает потом разберусь
		    tmpdout = *(u32 *)dout;
			     
			   //Фигня полная неправильный вариант всё висит
			  //tmpdout = *(u32 *)*dout;
             //tmpdout =0x9f000000; //*(u32 *)dout;
            //tmpdout =0x00000000;

			// Set up the next iteration if sending > 4 bytes 
			if (l_len > 4) 
			{
				l_len -= 4;
				dout += 4;            
			}

			espi->tx = tmpdout;
			espi->event |= ESPI_EV_TNF;
			//debug("*** espi_xfer: ... %08x written\n", tmpdout);
			//printf("*** send:%08x\n", tmpdout);
		}

		// Wait for eSPI transmit to get out 
		udelay(80);
        printf("TRANSACTION %d\n",transaction++);	
		event = espi->event;
		
		if (event & ESPI_EV_RNE) 
		{
			tmpdin = espi->rx;
			if (numBlks == 0 && num_bytes != 0) 
			{
			    //tmpdin=0xBBBBBBBB;
				  //tmpdin=0xDDDDDDDD;
				  	din[j++]=tmpdin;
				//*(u32 *)din=tmpdin; 
				//ch =(unsigned char*)&tmpdin;				
				//while(num_bytes--)
				//{	 
				// *din++ =*ch++;
				 //*(unsigned char *)din++ = *ch++;            // zavtra ispravit	
				//}
		        
	       
			} 
			else 
			{
			
			   // *(u32 *)din =0xEEEEEEEE;
			//	*(u32 *)din =0xAAAAAAAA; //tmpdin;
			    //*(u32 *)din=tmpdin;	  
				
				//din[j++]=0xEEEEEEEE;
				 din[j++]=tmpdin; 
				
				
				//din += 4;                                                  //zavtra ispravit
			    
			}
            
			espi->event |= ESPI_EV_RNE;
			// printf("recive:%08x\n",espi->rx); 
			//debug("*** espi_xfer: ... %08x readed\n", tmpdin);
		    //printf("*** espi_xfer: ... %08x readed\n", tmpdin);   
		    // printf("recive:%08x \n",tmpdin);
	                                                  	       
		       //printf("recive:%08x\n",my_out[numBlks]);
		      // printf("Number=%d",num++);
		   
		    //my_out[numBlks]=tmpdin;
		    //printf("recive:%08x\n",my_out[numBlks]);   
		}
		
	   if(transaction>=150000)break; 
	}
    
	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);
   
    
    
   //Полученные данные надо сдвигать на 3 байта вправо т.к значение 
   //1 байта располгаеться в 4 байтах!!!!!
    
    slave->transfer->rx_buf=din;
	return 0;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs < ESPI_MAX_CS_NUM;
}

void spi_cs_activate(struct spi_slave *slave)
{	
	//volatile ccsr_espi_t *espi = (void *)(CONFIG_SYS_MPC85xx_ESPI_ADDR);
    
    volatile ccsr_espi_t *espi = (void *)(MPC85xx_ESPI_ADDR);
	struct espi_transfer *t = slave->transfer;
	unsigned int com = 0;
	unsigned int len = t->cmd_len + t->data_len;

	com &= ~(ESPI_COM_CS(0x3) | ESPI_COM_TRANLEN(0xFFFF));
	com |= ESPI_COM_CS(slave->cs);
	com |= ESPI_COM_TRANLEN(len - 1);
	espi->com = com;

	return;
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	//volatile ccsr_espi_t *espi = (void *)(CONFIG_SYS_MPC85xx_ESPI_ADDR); 
      volatile ccsr_espi_t *espi = (void *)(MPC85xx_ESPI_ADDR);
	// clear the RXCNT and TXCNT 
    espi->mode &= ~ESPI_MODE_EN;
	espi->mode |= ESPI_MODE_EN;
}
