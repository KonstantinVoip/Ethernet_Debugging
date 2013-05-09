/*
 * Copyright 2004, 2007 Freescale Semiconductor.
 * Copyright(c) 2003 Motorola Inc.
 */

#ifndef	__ETH_H__
#define __ETH_H__
#include "net.h"

//int eth_register(struct eth_device* dev);


static struct eth_device *eth_devices, *eth_current;

//Prototype functions
int eth_register(struct eth_device* dev)
{
	struct eth_device *d;

	if (!eth_devices) {
		eth_current = eth_devices = dev;
#ifdef CONFIG_NET_MULTI
		// update current ethernet name 
		{
			// char *act = getenv("ethact");
			// if (act == NULL || strcmp(act, eth_current->name) != 0)
	      	//	setenv("ethact", eth_current->name);
	        
		}
#endif
	} else {
		for (d=eth_devices; d->next!=eth_devices; d=d->next);
		d->next = dev;
	}

	dev->state = ETH_STATE_INIT;
	dev->next  = eth_devices;

	return 0;
}


#endif	/* __MPC85xx_H__ */
