/*
 * (C) Copyright 2008-2010 Freescale Semiconductor, Inc.
 * Terry Lv
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include "./temp.h"




#define REG_RD(base, reg) \
        (*(volatile unsigned int *)((base) + (reg)))
#define REG_WR(base, reg, value) \
        ((*(volatile unsigned int *)((base) + (reg))) = (value))
#define REG_SET(base, reg, value) \
    ((*(volatile unsigned int *)((base) + (reg ## _SET))) = (value))
#define REG_CLR(base, reg, value) \
    ((*(volatile unsigned int *)((base) + (reg ## _CLR))) = (value))
#define REG_TOG(base, reg, value) \
    ((*(volatile unsigned int *)((base) + (reg ## _TOG))) = (value))

#define AIPS1_ARB_BASE_ADDR             0x02000000
#define ATZ1_BASE_ADDR              AIPS1_ARB_BASE_ADDR
#define AIPS1_OFF_BASE_ADDR         (ATZ1_BASE_ADDR + 0x80000)
#define ANATOP_BASE_ADDR            (AIPS1_OFF_BASE_ADDR + 0x48000)




#define HW_ANADIG_PLL_SYS   (0x00000000)
#define HW_ANADIG_PLL_SYS_SET   (0x00000004)
#define HW_ANADIG_PLL_SYS_CLR   (0x00000008)
#define HW_ANADIG_PLL_SYS_TOG   (0x0000000c)

#define HW_ANADIG_PLL_528   (0x00000030)
#define HW_ANADIG_PLL_528_SET   (0x00000034)
#define HW_ANADIG_PLL_528_CLR   (0x00000038)
#define HW_ANADIG_PLL_528_TOG   (0x0000003c)


#define AHB_CLK_ROOT 132000000
#define IPG_CLK_ROOT 66000000
#define ENET_FREQ_0 25000000
#define ENET_FREQ_1 50000000
#define ENET_FREQ_2 100000000
#define ENET_FREQ_3 125000000



enum pll_clocks {
	CPU_PLL1,	/* System PLL */
	BUS_PLL2,	/* System Bus PLL*/
	USBOTG_PLL3,    /* OTG USB PLL */
	AUD_PLL4,	/* Audio PLL */
	VID_PLL5,	/* Video PLL */
	MLB_PLL6,	/* MLB PLL */
	USBHOST_PLL7,   /* Host USB PLL */
	ENET_PLL8,      /* ENET PLL */
};

enum {
    CPU_CLK=0,
    PERIPH_CLK,
    AHB_CLK,
    IPG_CLK,
    IPG_PERCLK,
    UART_CLK,
    CSPI_CLK,
    DDR_CLK,
    NFC_CLK,
    ALL_CLK,
};


#define SZ_DEC_1M       1000000
#define CONFIG_REF_CLK_FREQ 24000000
#define CONFIG_MX6_HCLK_FREQ 24000000
#if 1
#define PLL1_FREQ_MAX	1300000000
#define PLL1_FREQ_MIN	650000000
#define PLL2_FREQ_MAX	528000000
#define PLL2_FREQ_MIN	480000000
#define MAX_DDR_CLK     PLL2_FREQ_MAX
#define AHB_CLK_MAX     132000000
#define IPG_CLK_MAX     (AHB_CLK_MAX >> 1)
#define NFC_CLK_MAX     PLL2_FREQ_MAX
#endif

static int config_pll_clk(enum pll_clocks pll, u32 divider)
{
    u32 ccsr = readl(CCM_BASE_ADDR + CLKCTL_CCSR);
#if 1
	switch (pll) {
	case CPU_PLL1:
		/* Switch ARM to PLL2 clock */
		writel(ccsr | 0x4, CCM_BASE_ADDR + CLKCTL_CCSR);
		REG_CLR(ANATOP_BASE_ADDR, HW_ANADIG_PLL_SYS,
			BM_ANADIG_PLL_SYS_DIV_SELECT);
        REG_SET(ANATOP_BASE_ADDR, HW_ANADIG_PLL_SYS,
			BF_ANADIG_PLL_SYS_DIV_SELECT(divider));
		/* Enable CPU PLL1 */
		REG_SET(ANATOP_BASE_ADDR, HW_ANADIG_PLL_SYS,
			BM_ANADIG_PLL_SYS_ENABLE);
		/* Wait for PLL lock */
		while (REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_PLL_SYS) &
			BM_ANADIG_PLL_SYS_LOCK)
			udelay(10);
		/* Clear bypass bit */
		REG_CLR(ANATOP_BASE_ADDR, HW_ANADIG_PLL_SYS,
			BM_ANADIG_PLL_SYS_BYPASS);

		/* Switch back */
		writel(ccsr & ~0x4, CCM_BASE_ADDR + CLKCTL_CCSR);
		break;
	case BUS_PLL2:
		/* Switch to pll2 bypass clock */
		writel(ccsr | 0x2, CCM_BASE_ADDR + CLKCTL_CCSR);

		REG_CLR(ANATOP_BASE_ADDR, HW_ANADIG_PLL_528,
			BM_ANADIG_PLL_528_DIV_SELECT);
		REG_SET(ANATOP_BASE_ADDR, HW_ANADIG_PLL_528,
			divider);
		/* Enable BUS PLL2 */
		REG_SET(ANATOP_BASE_ADDR, HW_ANADIG_PLL_528,
			BM_ANADIG_PLL_528_ENABLE);
		/* Wait for PLL lock */
		while (REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_PLL_528) &
			BM_ANADIG_PLL_528_LOCK)
			udelay(10);
		/* Clear bypass bit */
		REG_CLR(ANATOP_BASE_ADDR, HW_ANADIG_PLL_528,
			BM_ANADIG_PLL_528_BYPASS);

		/* Switch back */
		writel(ccsr & ~0x2, CCM_BASE_ADDR + CLKCTL_CCSR);
		break;
	default:
		return -1;
	}

	return 0;
#endif
}

static int calc_pll_divider(enum pll_clocks pll, u32 ref, u32 target)
{
	int i, div;

	switch (pll) {
	case CPU_PLL1:
		if (target < PLL1_FREQ_MIN || target > PLL1_FREQ_MAX) {
			printf("PLL1 frequency should be"
			"within [%d - %d] MHz\n", PLL1_FREQ_MIN / SZ_DEC_1M,
				PLL1_FREQ_MAX / SZ_DEC_1M);
			return -1;
		}
		for (i = 54, div = i; i < 109; i++) {
			if ((ref * (i >> 1)) > target)
				break;
			div = i;
		}
		break;
	case BUS_PLL2:
		if (target < PLL2_FREQ_MIN || target > PLL2_FREQ_MAX) {
			printf("PLL2 frequency should be"
			"within [%d - %d] MHz\n", PLL2_FREQ_MIN / SZ_DEC_1M,
				PLL2_FREQ_MAX / SZ_DEC_1M);
			return -1;
		}
		for (i = 0, div = i; i < 2; i++) {
			if (ref * (20 + (i << 1)) > target)
				break;
			div = i;
		}
		break;
	default:
		printf("Changing this PLL not supported\n");
		return -1;
		break;
	}

	return div;
}



static int config_core_clk(u32 ref, u32 freq)
{
	int div = calc_pll_divider(CPU_PLL1, ref, freq);
	if (div < 0) {
		printf("Can't find pll parameters\n");
		return div;
	}

	return config_pll_clk(CPU_PLL1, div);
}

static u32 __decode_pll(enum pll_clocks pll, u32 infreq)
{
	u32 div;

	switch (pll) {
	case CPU_PLL1:
		div = REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_PLL_SYS) &
			BM_ANADIG_PLL_SYS_DIV_SELECT;
		return infreq * (div >> 1);
	case BUS_PLL2:
		div = REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_PLL_528) &
			BM_ANADIG_PLL_528_DIV_SELECT;
		return infreq * (20 + (div << 1));
	case AUD_PLL4:
	case VID_PLL5:
	case MLB_PLL6:
	case USBHOST_PLL7:
	default:
		return 0;
	}
}

static u32 __get_periph_clk(void)
{
	u32 reg;
    printf("reg===============0x%x\n",reg);
	//reg = __REG(MXC_CCM_CBCDR);
    reg = *((volatile u32 *)(MXC_CCM_CBCDR));  
    printf("reg===============0x%x\n",reg);
	if (reg & MXC_CCM_CBCDR_PERIPH_CLK_SEL) {
		//reg = __REG(MXC_CCM_CBCMR);
        reg = *((volatile u32 *)(MXC_CCM_CBCDR));  
		switch ((reg & MXC_CCM_CBCMR_PERIPH_CLK2_SEL_MASK) >>
			MXC_CCM_CBCMR_PERIPH_CLK2_SEL_OFFSET) {
		case 0:
			return __decode_pll(USBOTG_PLL3, CONFIG_MX6_HCLK_FREQ);
		case 1:
		case 2:
			return CONFIG_MX6_HCLK_FREQ;
		default:
			return 0;
		}
	} else {
		//reg = __REG(MXC_CCM_CBCMR);
        reg = *((volatile u32 *)(MXC_CCM_CBCDR));  
		switch ((reg & MXC_CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK) >>
			MXC_CCM_CBCMR_PRE_PERIPH_CLK_SEL_OFFSET) {
		default:
		case 0:
			return __decode_pll(BUS_PLL2, CONFIG_MX6_HCLK_FREQ);
		case 1:
			return PLL2_PFD2_FREQ;
		case 2:
			return PLL2_PFD0_FREQ;
		case 3:
			return PLL2_PFD2_DIV_FREQ;
		}
	}
}

static int config_ddr_clk(u32 ddr_clk)
{
	u32 clk_src = __get_periph_clk();
	u32 i, podf;
	u32 cbcdr = readl(CCM_BASE_ADDR + CLKCTL_CBCDR);

	if (ddr_clk > MAX_DDR_CLK) {
		printf("DDR clock should be less than"
			"%d MHz, assuming max value\n",
			(MAX_DDR_CLK / SZ_DEC_1M));
		ddr_clk = MAX_DDR_CLK;
	}

	for (i = 1; i < 9; i++)
		if ((clk_src / i) <= ddr_clk)
			break;

	podf = i - 1;

	cbcdr &= ~(MXC_CCM_CBCDR_MMDC_CH0_PODF_MASK |
			MXC_CCM_CBCDR_MMDC_CH1_PODF_MASK);
	cbcdr |= (podf << MXC_CCM_CBCDR_MMDC_CH0_PODF_OFFSET) |
			(podf << MXC_CCM_CBCDR_MMDC_CH1_PODF_OFFSET);
	writel(cbcdr, CCM_BASE_ADDR + CLKCTL_CBCDR);
	while (readl(CCM_BASE_ADDR + CLKCTL_CDHIPR) != 0)
		;
	writel(0x0, CCM_BASE_ADDR + CLKCTL_CCDR);

	return 0;
}

    
static int clk_config(u32 ref, u32 freq, u32 clk_type)
{
	freq *= SZ_DEC_1M;

	switch (clk_type) {
	case CPU_CLK:
		if (config_core_clk(ref, freq))
			return -1;
		break;
	case DDR_CLK:
		if (config_ddr_clk(freq))
			return -1;
		break;
	default:
		printf("Unsupported or invalid clock type! :(\n");
		return -1;
	}

	return 0;

}


int do_clkops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int rc = 0;
	int freq = 0;

	freq = simple_strtoul(argv[2], NULL, 10);
	if (strcmp(argv[1], "core") == 0)
       	clk_config(CONFIG_REF_CLK_FREQ, freq, CPU_CLK);
	else if (strcmp(argv[1], "periph") == 0)
		clk_config(CONFIG_REF_CLK_FREQ, freq, PERIPH_CLK);
	else if (strcmp(argv[1], "ddr") == 0)
		clk_config(CONFIG_REF_CLK_FREQ, freq, DDR_CLK);
	else if (strcmp(argv[1], "nfc") == 0)
		clk_config(CONFIG_REF_CLK_FREQ, freq, NFC_CLK);
	else
		printf("Unsupported clock type!\n");


}


U_BOOT_CMD(
	clk, 3, 1, do_clkops,
	"Clock sub system",
	"Setup/Display clock\n"
	"clk - Display all clocks\n"
	"clk core <core clock in MHz> - Setup/Display core clock\n"
	"clk periph <peripheral clock in MHz> -"
	"Setup/Display peripheral clock\n"
	"clk ddr <DDR clock in MHz> - Setup/Display DDR clock\n"
	"clk nfc <NFC clk in MHz> - Setup/Display NFC clock\n"
	"Example:\n"
	"clk - Show various clocks\n"
	"clk core 665 - Set core clock to 665MHz\n"
	"clk periph 600 - Set peripheral clock to 600MHz\n"
	"clk ddr 166 - Set DDR clock to 166MHz");

