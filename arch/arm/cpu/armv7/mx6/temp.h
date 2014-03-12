/*
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define IMX_IO_BASE   0x00200000
#define IO_ADDRESS(x) ((x) | IMX_IO_BASE)

# ifndef __ASSEMBLY__
# define __REG(x)	(*((volatile u32 *)IO_ADDRESS(x)))
# define __REG2(x,y)        (*(volatile u32 *)((u32)&__REG(x) + (y)))
# else
#  define __REG(x) (x)
#  define __REG2(x,y) ((x)+(y))
#endif


#define MXC_CCM_BASE   CCM_BASE_ADDR

/* Register addresses of CCM*/
#define MXC_CCM_CCR		(MXC_CCM_BASE + 0x00)
#define MXC_CCM_CCDR		(MXC_CCM_BASE + 0x04)
#define MXC_CCM_CSR		(MXC_CCM_BASE + 0x08)
#define MXC_CCM_CCSR		(MXC_CCM_BASE + 0x0C)
#define MXC_CCM_CACRR		(MXC_CCM_BASE + 0x10)
#define MXC_CCM_CBCDR		(MXC_CCM_BASE + 0x14)
#define MXC_CCM_CBCMR		(MXC_CCM_BASE + 0x18)
#define MXC_CCM_CSCMR1		(MXC_CCM_BASE + 0x1C)
#define MXC_CCM_CSCMR2		(MXC_CCM_BASE + 0x20)
#define MXC_CCM_CSCDR1		(MXC_CCM_BASE + 0x24)
#define MXC_CCM_CS1CDR		(MXC_CCM_BASE + 0x28)
#define MXC_CCM_CS2CDR		(MXC_CCM_BASE + 0x2C)
#define MXC_CCM_CDCDR		(MXC_CCM_BASE + 0x30)
#define MXC_CCM_CHSCDR		(MXC_CCM_BASE + 0x34)
#define MXC_CCM_CSCDR2		(MXC_CCM_BASE + 0x38)
#define MXC_CCM_CSCDR3		(MXC_CCM_BASE + 0x3C)
#define MXC_CCM_CSCDR4		(MXC_CCM_BASE + 0x40)
#define MXC_CCM_CWDR		(MXC_CCM_BASE + 0x44)
#define MXC_CCM_CDHIPR		(MXC_CCM_BASE + 0x48)
#define MXC_CCM_CDCR		(MXC_CCM_BASE + 0x4C)
#define MXC_CCM_CTOR		(MXC_CCM_BASE + 0x50)
#define MXC_CCM_CLPCR		(MXC_CCM_BASE + 0x54)
#define MXC_CCM_CISR		(MXC_CCM_BASE + 0x58)
#define MXC_CCM_CIMR		(MXC_CCM_BASE + 0x5C)
#define MXC_CCM_CCOSR		(MXC_CCM_BASE + 0x60)
#define MXC_CCM_CGPR		(MXC_CCM_BASE + 0x64)
#define MXC_CCM_CCGR0		(MXC_CCM_BASE + 0x68)
#define MXC_CCM_CCGR1		(MXC_CCM_BASE + 0x6C)
#define MXC_CCM_CCGR2		(MXC_CCM_BASE + 0x70)
#define MXC_CCM_CCGR3		(MXC_CCM_BASE + 0x74)
#define MXC_CCM_CCGR4		(MXC_CCM_BASE + 0x78)
#define MXC_CCM_CCGR5		(MXC_CCM_BASE + 0x7C)
#define MXC_CCM_CCGR6		(MXC_CCM_BASE + 0x80)
#define MXC_CCM_CCGR7		(MXC_CCM_BASE + 0x80)
#define MXC_CCM_CMEOR		(MXC_CCM_BASE + 0x88)


