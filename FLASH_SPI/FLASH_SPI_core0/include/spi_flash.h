/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

/*#include <spi.h>                   //unclomment for linux
#include <linux/types.h>*/
#include "spi.h"
#include "linux\types.h"


int do_spi_flash_probe();
int get_clocks();
int do_clocks();
void udelay(unsigned long usec);

size_t min(size_t,unsigned int);
size_t max(size_t,unsigned int);

/////
struct spi_flash_region {
	unsigned int	count;
	unsigned int	size;
};

struct spi_flash {
	struct spi_slave *spi;
	const char	*name;
	u32		size;
	int		(*read)(struct spi_flash *flash, u32 offset,size_t len, void *buf);
	int		(*write)(struct spi_flash *flash, u32 offset,size_t len, const void *buf);
	int		(*erase)(struct spi_flash *flash, u32 offset,size_t len);
};



//struct spi_flash *flash;


struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,unsigned int max_hz, unsigned int spi_mode);
void spi_flash_free(struct spi_flash *flash);

static inline int spi_flash_read(struct spi_flash *flash, u32 offset,size_t len, void *buf)
{
	return flash->read(flash, offset, len, buf);
}

static inline int spi_flash_write(struct spi_flash *flash, u32 offset,size_t len, const void *buf)
{
	return flash->write(flash, offset, len, buf);
}

static inline int spi_flash_erase(struct spi_flash *flash, u32 offset,size_t len)
{
	return flash->erase(flash, offset, len);
}

#endif /* _SPI_FLASH_H_ */
