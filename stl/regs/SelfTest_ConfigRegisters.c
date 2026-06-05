/*******************************************************************************
* File Name: SelfTest_ConfigRegisters.c
*
* Description:
*  This file provides the source code to the APIs for the configuration register self
*  tests.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cy_pdl.h"
#include "SelfTest_CRC_calc.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_ConfigRegisters.h"
#include "SelfTest_Flash.h"

#if defined(SELFTEST_PSOC4_FAMILY)

#if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)
/* The table of register 32Bit registers to test. */
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(PASS0_DSAB->DSAB_CTRL),
    &(SAR0->PUMP_CTRL),
    &(CTBM0->CTB_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),
    &(HSIOM_PRT5->PORT_SEL),
    &(HSIOM_PRT6->PORT_SEL),
    &(HSIOM_PRT7->PORT_SEL),
    &(HSIOM_PRT8->PORT_SEL),
    &(HSIOM_PRT9->PORT_SEL),
    &(HSIOM_PRT10->PORT_SEL),
    &(HSIOM_PRT11->PORT_SEL),
    &(HSIOM_PRT12->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
    &(GPIO_PRT5->PC),
    &(GPIO_PRT5->PC2),
    &(GPIO_PRT6->PC),
    &(GPIO_PRT6->PC2),
    &(GPIO_PRT7->PC),
    &(GPIO_PRT7->PC2),

    &(GPIO_PRT8->PC),
    &(GPIO_PRT8->PC2),
    &(GPIO_PRT9->PC),
    &(GPIO_PRT9->PC2),
    &(GPIO_PRT10->PC),
    &(GPIO_PRT10->PC2),
    &(GPIO_PRT11->PC),
    &(GPIO_PRT11->PC2),
    &(GPIO_PRT12->PC),
    &(GPIO_PRT12->PC2),
};
#endif /*  4100S MAX  */

/* The table of register 32Bit registers to test */
#if (defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS)) || \
    (defined(CY_DEVICE_SERIES_PSOC_4500S))
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(PASS0_DSAB->DSAB_CTRL),
    &(SAR0->PUMP_CTRL),
    &(CTBM0->CTB_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),
    &(HSIOM_PRT5->PORT_SEL),
    &(HSIOM_PRT6->PORT_SEL),
    &(HSIOM_PRT7->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
    &(GPIO_PRT5->PC),
    &(GPIO_PRT5->PC2),
    &(GPIO_PRT6->PC),
    &(GPIO_PRT6->PC2),
    &(GPIO_PRT7->PC),
    &(GPIO_PRT7->PC2),
};
#endif /* 4500S & 4100S Plus */

#if defined(CY_DEVICE_SERIES_PSOC_4100S)
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(PASS0_DSAB->DSAB_CTRL),
    &(SAR0->PUMP_CTRL),
    &(CTBM0->CTB_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
};
#endif /* 4100S */

#if defined(CY_DEVICE_SERIES_PSOC_4100T_PLUS)
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(SAR0->PUMP_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),
    &(HSIOM_PRT5->PORT_SEL),
    &(HSIOM_PRT6->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
    &(GPIO_PRT5->PC),
    &(GPIO_PRT5->PC2),
    &(GPIO_PRT6->PC),
    &(GPIO_PRT6->PC2),
};
#endif /* 4100T Plus */

#if defined(CY_DEVICE_SERIES_PSOC_4000T)
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
};
#endif /* 4000T */

#if defined(CY_DEVICE_SERIES_PSOC_4000S) || \
    defined(CY_DEVICE_SERIES_PSOC_4700S)
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
};
#endif /* 4000S and 4700S */

#elif defined(SELFTEST_PSOC6_FAMILY) || defined(SELFTEST_XMC7X_FAMILY)

static volatile uint32_t* regs32_ToTest[] =
{
    #if defined(SELFTEST_PSOC6_FAMILY)
    /* Analog routing regs */
    /* This value will get changes by SelfTests_Init_StartUp_ConfigReg function during runtime. */
    (uint32_t*)(0x409F0E00),

    /* Clock registers */
    &(SRSS->WDT_CTL),
    #endif
    /* SRSS_CLK_PATH_SELECT */
    &(SRSS->CLK_PATH_SELECT[0]),
    &(SRSS->CLK_PATH_SELECT[1]),
    &(SRSS->CLK_PATH_SELECT[2]),
    &(SRSS->CLK_PATH_SELECT[3]),
    &(SRSS->CLK_PATH_SELECT[4]),
    &(SRSS->CLK_PATH_SELECT[5]),
    &(SRSS->CLK_PATH_SELECT[6]),
    &(SRSS->CLK_PATH_SELECT[7]),
    &(SRSS->CLK_PATH_SELECT[8]),
    &(SRSS->CLK_PATH_SELECT[9]),
    &(SRSS->CLK_PATH_SELECT[10]),
    &(SRSS->CLK_PATH_SELECT[11]),
    &(SRSS->CLK_PATH_SELECT[12]),
    &(SRSS->CLK_PATH_SELECT[13]),
    &(SRSS->CLK_PATH_SELECT[14]),
    &(SRSS->CLK_PATH_SELECT[15]),
    /* SRSS_CLK_ROOT_SELECT */
    &(SRSS->CLK_ROOT_SELECT[0]),
    &(SRSS->CLK_ROOT_SELECT[1]),
    &(SRSS->CLK_ROOT_SELECT[2]),
    &(SRSS->CLK_ROOT_SELECT[3]),
    &(SRSS->CLK_ROOT_SELECT[4]),
    &(SRSS->CLK_ROOT_SELECT[5]),
    &(SRSS->CLK_ROOT_SELECT[6]),
    &(SRSS->CLK_ROOT_SELECT[7]),
    &(SRSS->CLK_ROOT_SELECT[8]),
    &(SRSS->CLK_ROOT_SELECT[9]),
    &(SRSS->CLK_ROOT_SELECT[10]),
    &(SRSS->CLK_ROOT_SELECT[11]),
    &(SRSS->CLK_ROOT_SELECT[12]),
    &(SRSS->CLK_ROOT_SELECT[13]),
    &(SRSS->CLK_ROOT_SELECT[14]),
    &(SRSS->CLK_ROOT_SELECT[15]),
    &(SRSS->CLK_SELECT),
    #if defined(SELFTEST_PSOC6_FAMILY)
    &(SRSS->CLK_TIMER_CTL),
    &(SRSS->CLK_ILO_CONFIG),
    #elif defined(SELFTEST_XMC7X_FAMILY)
    &(SRSS->CLK_ILO0_CONFIG),
    &(SRSS->CLK_ILO1_CONFIG),
    #endif
    &(SRSS->CLK_OUTPUT_SLOW),
    &(SRSS->CLK_OUTPUT_FAST),
    &(SRSS->CLK_ECO_CONFIG),
    &(SRSS->CLK_PILO_CONFIG),
    &(SRSS->CLK_FLL_CONFIG),
    &(SRSS->CLK_FLL_CONFIG2),
    &(SRSS->CLK_FLL_CONFIG3),
    /* SRSS_CLK_PLL_CONFIG */
    &(SRSS->CLK_PLL_CONFIG[0]),
    &(SRSS->CLK_PLL_CONFIG[1]),
    &(SRSS->CLK_PLL_CONFIG[2]),
    &(SRSS->CLK_PLL_CONFIG[3]),
    &(SRSS->CLK_PLL_CONFIG[4]),
    &(SRSS->CLK_PLL_CONFIG[5]),
    &(SRSS->CLK_PLL_CONFIG[6]),
    &(SRSS->CLK_PLL_CONFIG[7]),
    &(SRSS->CLK_PLL_CONFIG[8]),
    &(SRSS->CLK_PLL_CONFIG[9]),
    &(SRSS->CLK_PLL_CONFIG[10]),
    &(SRSS->CLK_PLL_CONFIG[11]),
    &(SRSS->CLK_PLL_CONFIG[12]),
    &(SRSS->CLK_PLL_CONFIG[13]),
    &(SRSS->CLK_PLL_CONFIG[14]),


    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL0),
    &(HSIOM_PRT0->PORT_SEL1),
    &(HSIOM_PRT1->PORT_SEL0),
    &(HSIOM_PRT1->PORT_SEL1),
    &(HSIOM_PRT2->PORT_SEL0),
    &(HSIOM_PRT2->PORT_SEL1),
    &(HSIOM_PRT3->PORT_SEL0),
    &(HSIOM_PRT3->PORT_SEL1),
    &(HSIOM_PRT4->PORT_SEL0),
    &(HSIOM_PRT4->PORT_SEL1),
    &(HSIOM_PRT5->PORT_SEL0),
    &(HSIOM_PRT5->PORT_SEL1),
    &(HSIOM_PRT6->PORT_SEL0),
    &(HSIOM_PRT6->PORT_SEL1),
    &(HSIOM_PRT7->PORT_SEL0),
    &(HSIOM_PRT7->PORT_SEL1),
    &(HSIOM_PRT8->PORT_SEL0),
    &(HSIOM_PRT8->PORT_SEL1),
    &(HSIOM_PRT9->PORT_SEL0),
    &(HSIOM_PRT9->PORT_SEL1),
    &(HSIOM_PRT10->PORT_SEL0),
    &(HSIOM_PRT10->PORT_SEL1),
    &(HSIOM_PRT11->PORT_SEL0),
    &(HSIOM_PRT11->PORT_SEL1),
    &(HSIOM_PRT12->PORT_SEL0),
    &(HSIOM_PRT12->PORT_SEL1),
    &(HSIOM_PRT13->PORT_SEL0),
    &(HSIOM_PRT13->PORT_SEL1),

    /* IO Pin registers */
    &(GPIO_PRT0->CFG),
    &(GPIO_PRT1->CFG),
    &(GPIO_PRT2->CFG),
    &(GPIO_PRT3->CFG),
    &(GPIO_PRT4->CFG),
    &(GPIO_PRT5->CFG),
    &(GPIO_PRT6->CFG),
    &(GPIO_PRT7->CFG),
    &(GPIO_PRT8->CFG),
    &(GPIO_PRT9->CFG),
    &(GPIO_PRT10->CFG),
    &(GPIO_PRT11->CFG),
    &(GPIO_PRT12->CFG),
    &(GPIO_PRT13->CFG),
};

#elif defined(SELFTEST_PSC3_FAMILY)
static volatile uint32_t* regs32_ToTest[] =
{
    /* SRSS_CLK_PATH_SELECT */
    &(SRSS->CLK_PATH_SELECT[0]),
    &(SRSS->CLK_PATH_SELECT[1]),
    &(SRSS->CLK_PATH_SELECT[2]),
    &(SRSS->CLK_PATH_SELECT[3]),
    &(SRSS->CLK_PATH_SELECT[4]),
    &(SRSS->CLK_PATH_SELECT[5]),
    &(SRSS->CLK_PATH_SELECT[6]),
    &(SRSS->CLK_PATH_SELECT[7]),
    &(SRSS->CLK_PATH_SELECT[8]),
    &(SRSS->CLK_PATH_SELECT[9]),
    &(SRSS->CLK_PATH_SELECT[10]),
    &(SRSS->CLK_PATH_SELECT[11]),
    &(SRSS->CLK_PATH_SELECT[12]),
    &(SRSS->CLK_PATH_SELECT[13]),
    &(SRSS->CLK_PATH_SELECT[14]),
    &(SRSS->CLK_PATH_SELECT[15]),
    /* SRSS_CLK_ROOT_SELECT */
    &(SRSS->CLK_ROOT_SELECT[0]),
    &(SRSS->CLK_ROOT_SELECT[1]),
    &(SRSS->CLK_ROOT_SELECT[2]),
    &(SRSS->CLK_ROOT_SELECT[3]),
    &(SRSS->CLK_ROOT_SELECT[4]),
    &(SRSS->CLK_ROOT_SELECT[5]),
    &(SRSS->CLK_ROOT_SELECT[6]),
    &(SRSS->CLK_ROOT_SELECT[7]),
    &(SRSS->CLK_ROOT_SELECT[8]),
    &(SRSS->CLK_ROOT_SELECT[9]),
    &(SRSS->CLK_ROOT_SELECT[10]),
    &(SRSS->CLK_ROOT_SELECT[11]),
    &(SRSS->CLK_ROOT_SELECT[12]),
    &(SRSS->CLK_ROOT_SELECT[13]),
    &(SRSS->CLK_ROOT_SELECT[14]),
    &(SRSS->CLK_ROOT_SELECT[15]),
    &(SRSS->CLK_SELECT),
    &(SRSS->CLK_OUTPUT_SLOW),
    &(SRSS->CLK_OUTPUT_FAST),
    &(SRSS->CLK_ECO_CONFIG),
    &(SRSS->CLK_PILO_CONFIG),
    &(SRSS->CLK_FLL_CONFIG),
    &(SRSS->CLK_FLL_CONFIG2),
    &(SRSS->CLK_FLL_CONFIG3),
    /* SRSS_CLK_PLL_CONFIG */
    &(SRSS->CLK_PLL_CONFIG[0]),
    &(SRSS->CLK_PLL_CONFIG[1]),
    &(SRSS->CLK_PLL_CONFIG[2]),
    &(SRSS->CLK_PLL_CONFIG[3]),
    &(SRSS->CLK_PLL_CONFIG[4]),
    &(SRSS->CLK_PLL_CONFIG[5]),
    &(SRSS->CLK_PLL_CONFIG[6]),
    &(SRSS->CLK_PLL_CONFIG[7]),
    &(SRSS->CLK_PLL_CONFIG[8]),
    &(SRSS->CLK_PLL_CONFIG[9]),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL0),
    &(HSIOM_PRT0->PORT_SEL1),
    &(HSIOM_PRT1->PORT_SEL0),
    &(HSIOM_PRT1->PORT_SEL1),
    &(HSIOM_PRT2->PORT_SEL0),
    &(HSIOM_PRT2->PORT_SEL1),
    &(HSIOM_PRT3->PORT_SEL0),
    &(HSIOM_PRT3->PORT_SEL1),
    &(HSIOM_PRT4->PORT_SEL0),
    &(HSIOM_PRT4->PORT_SEL1),
    &(HSIOM_PRT5->PORT_SEL0),
    &(HSIOM_PRT5->PORT_SEL1),
    &(HSIOM_PRT6->PORT_SEL0),
    &(HSIOM_PRT6->PORT_SEL1),
    &(HSIOM_PRT7->PORT_SEL0),
    &(HSIOM_PRT7->PORT_SEL1),
    &(HSIOM_PRT8->PORT_SEL0),
    &(HSIOM_PRT8->PORT_SEL1),
    &(HSIOM_PRT9->PORT_SEL0),
    &(HSIOM_PRT9->PORT_SEL1),

    /* IO Pin registers */
    &(GPIO_PRT0->CFG),
    &(GPIO_PRT1->CFG),
    &(GPIO_PRT2->CFG),
    &(GPIO_PRT3->CFG),
    &(GPIO_PRT4->CFG),
    &(GPIO_PRT5->CFG),
    &(GPIO_PRT6->CFG),
    &(GPIO_PRT7->CFG),
    &(GPIO_PRT8->CFG),
    &(GPIO_PRT9->CFG),
};

#elif defined(SELFTEST_XMC5X_FAMILY)

static volatile uint32_t* regs32_ToTest[] =
{
    /* SRSS_CLK_PATH_SELECT (4 paths) */
    &(SRSS->CLK_PATH_SELECT[0]),
    &(SRSS->CLK_PATH_SELECT[1]),
    &(SRSS->CLK_PATH_SELECT[2]),
    &(SRSS->CLK_PATH_SELECT[3]),
    /* SRSS_CLK_ROOT_SELECT (3 HF roots) */
    &(SRSS->CLK_ROOT_SELECT[0]),
    &(SRSS->CLK_ROOT_SELECT[1]),
    &(SRSS->CLK_ROOT_SELECT[2]),
    &(SRSS->CLK_SELECT),
    &(SRSS->CLK_ILO0_CONFIG),
    &(SRSS->CLK_ILO1_CONFIG),
    &(SRSS->CLK_OUTPUT_SLOW),
    &(SRSS->CLK_OUTPUT_FAST),
    &(SRSS->CLK_ECO_CONFIG),
    /* No CLK_PILO_CONFIG - PILO not present on XMC5X */
    &(SRSS->CLK_FLL_CONFIG),
    &(SRSS->CLK_FLL_CONFIG2),
    &(SRSS->CLK_FLL_CONFIG3),
    /* SRSS_CLK_PLL_CONFIG (1 PLL) */
    &(SRSS->CLK_PLL_CONFIG[0]),

    /* HSIOM registers (24 ports) */
    &(HSIOM_PRT0->PORT_SEL0),
    &(HSIOM_PRT0->PORT_SEL1),
    &(HSIOM_PRT1->PORT_SEL0),
    &(HSIOM_PRT1->PORT_SEL1),
    &(HSIOM_PRT2->PORT_SEL0),
    &(HSIOM_PRT2->PORT_SEL1),
    &(HSIOM_PRT3->PORT_SEL0),
    &(HSIOM_PRT3->PORT_SEL1),
    &(HSIOM_PRT4->PORT_SEL0),
    &(HSIOM_PRT4->PORT_SEL1),
    &(HSIOM_PRT5->PORT_SEL0),
    &(HSIOM_PRT5->PORT_SEL1),
    &(HSIOM_PRT6->PORT_SEL0),
    &(HSIOM_PRT6->PORT_SEL1),
    &(HSIOM_PRT7->PORT_SEL0),
    &(HSIOM_PRT7->PORT_SEL1),
    &(HSIOM_PRT8->PORT_SEL0),
    &(HSIOM_PRT8->PORT_SEL1),
    &(HSIOM_PRT9->PORT_SEL0),
    &(HSIOM_PRT9->PORT_SEL1),
    &(HSIOM_PRT10->PORT_SEL0),
    &(HSIOM_PRT10->PORT_SEL1),
    &(HSIOM_PRT11->PORT_SEL0),
    &(HSIOM_PRT11->PORT_SEL1),
    &(HSIOM_PRT12->PORT_SEL0),
    &(HSIOM_PRT12->PORT_SEL1),
    &(HSIOM_PRT13->PORT_SEL0),
    &(HSIOM_PRT13->PORT_SEL1),
    &(HSIOM_PRT14->PORT_SEL0),
    &(HSIOM_PRT14->PORT_SEL1),
    &(HSIOM_PRT15->PORT_SEL0),
    &(HSIOM_PRT15->PORT_SEL1),
    &(HSIOM_PRT16->PORT_SEL0),
    &(HSIOM_PRT16->PORT_SEL1),
    &(HSIOM_PRT17->PORT_SEL0),
    &(HSIOM_PRT17->PORT_SEL1),
    &(HSIOM_PRT18->PORT_SEL0),
    &(HSIOM_PRT18->PORT_SEL1),
    &(HSIOM_PRT19->PORT_SEL0),
    &(HSIOM_PRT19->PORT_SEL1),
    &(HSIOM_PRT20->PORT_SEL0),
    &(HSIOM_PRT20->PORT_SEL1),
    &(HSIOM_PRT21->PORT_SEL0),
    &(HSIOM_PRT21->PORT_SEL1),
    &(HSIOM_PRT22->PORT_SEL0),
    &(HSIOM_PRT22->PORT_SEL1),
    &(HSIOM_PRT23->PORT_SEL0),
    &(HSIOM_PRT23->PORT_SEL1),

    /* IO Pin registers (24 ports) */
    &(GPIO_PRT0->CFG),
    &(GPIO_PRT1->CFG),
    &(GPIO_PRT2->CFG),
    &(GPIO_PRT3->CFG),
    &(GPIO_PRT4->CFG),
    &(GPIO_PRT5->CFG),
    &(GPIO_PRT6->CFG),
    &(GPIO_PRT7->CFG),
    &(GPIO_PRT8->CFG),
    &(GPIO_PRT9->CFG),
    &(GPIO_PRT10->CFG),
    &(GPIO_PRT11->CFG),
    &(GPIO_PRT12->CFG),
    &(GPIO_PRT13->CFG),
    &(GPIO_PRT14->CFG),
    &(GPIO_PRT15->CFG),
    &(GPIO_PRT16->CFG),
    &(GPIO_PRT17->CFG),
    &(GPIO_PRT18->CFG),
    &(GPIO_PRT19->CFG),
    &(GPIO_PRT20->CFG),
    &(GPIO_PRT21->CFG),
    &(GPIO_PRT22->CFG),
    &(GPIO_PRT23->CFG),
};

#endif /* SELFTEST device family blocks */

/* The buffer to store Flash row. */
#if !defined(CY_IP_MXFLASHC_VERSION_ECT)
CY_ALIGN(4) static uint32_t flashRowData[CY_FLASH_SIZEOF_ROW/(sizeof(uint32_t))];
#else
CY_ALIGN(4) static uint32_t flashRowData[(2*CY_FLASH_SIZEOF_ROW)/(sizeof(uint32_t))];
#endif

#if defined(SELFTEST_PSOC6_FAMILY)
void SelfTests_Init_StartUp_ConfigReg(void)
{
    volatile uint32_t* pass_aref = &PASS_AREF_AREF_CTRL;
    regs32_ToTest[0] = pass_aref;
}


#endif


#if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)
static uint8_t SelfTests_Check_StartUp_Cfg_CRC(uint32_t Current_CRC);
#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */


#if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)

/*******************************************************************************
 * Function Name: SelfTests_Save_StartUp_ConfigReg(void)
 *******************************************************************************
 *
 * Summary:
 *  This function stores the configuration registers to FlashRowData array
 *  and writes this array to Flash.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  0 - Writing to Flash is successful.
 *  >=1 - Writing to Flash is not successful. Refer to the Flash Driver PDL documentation.
 *  for error codes.
 *
 ******************************************************************************/
cy_en_flashdrv_status_t SelfTests_Save_StartUp_ConfigReg(void)
{
    /* Fill Flash row with "0" */
    (void)memset(flashRowData, 0x00, sizeof(flashRowData));

    #if !defined(CY_IP_MXFLASHC_VERSION_ECT)
    uint32_t* flash_checksum_ptr = (uint32_t*)FLASH_END_ADDR;
    uint32_t no_of_data_to_copy = (FLASH_RESERVED_CHECKSUM_SIZE/sizeof(flashRowData[0]));
    #endif

    #if defined(CY_DEVICE_PSOC6ABLE2)
    /* Setting Regs affected by Flash delay for 1M device*/
    SRSS_CLK_OUTPUT_SLOW = _VAL2FLD(SRSS_CLK_OUTPUT_SLOW_SLOW_SEL0, 6);
    SRSS_CLK_CAL_CNT1 = _VAL2FLD(SRSS_CLK_CAL_CNT1_CAL_COUNTER1, 0);
    #endif

    /* Store 32Bit registers */
    for (uint32_t i = 0u; i < (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u])); i++)
    {
        flashRowData[i] = CY_GET_REG32(regs32_ToTest[i]);
    }

    #if !defined(CY_IP_MXFLASHC_VERSION_ECT)
    for (uint32_t i = 0; i < no_of_data_to_copy; i++)
    {
        flashRowData[(CY_FLASH_SIZEOF_ROW/(sizeof(uint32_t))) - no_of_data_to_copy + i] =
            flash_checksum_ptr[i];
    }
    return Cy_Flash_WriteRow(CONF_REG_FIRST_ROW_ADDR, flashRowData);
    #else
    uint16_t guardCnt = 0u;
    cy_en_flashdrv_status_t ret;
    Cy_Flashc_MainWriteEnable();
    ret = Cy_Flash_EraseSector(CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE);
    while ((Cy_Flash_IsOperationComplete() != CY_FLASH_DRV_SUCCESS) && (guardCnt < 1000U))
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    }
    if (guardCnt >= 1000U)
    {
        return ret;
    }
    uint32_t regsToTestSize = sizeof(regs32_ToTest);
    if (regsToTestSize > CY_FLASH_SIZEOF_ROW)
    {
        uint32_t addr_to_write = (uint32_t)(CONF_REG_FIRST_ROW_ADDR);
        uint32_t* data_to_write = flashRowData;
        for (uint32_t i = 0; i < 2U; i++)
        {
            ret = Cy_Flash_ProgramRow(addr_to_write, data_to_write);
            if (ret != CY_FLASH_DRV_SUCCESS)
            {
                return ret;
            }
            addr_to_write = addr_to_write + CY_FLASH_SIZEOF_ROW;
            data_to_write = &flashRowData[(i+1U)*(CY_FLASH_SIZEOF_ROW/sizeof(uint32_t))];
        }
    }
    else
    {
        ret = Cy_Flash_ProgramRow(CONF_REG_FIRST_ROW_ADDR, flashRowData);
    }
    Cy_Flashc_MainWriteDisable();
    return ret;
    #endif /* !defined(CY_IP_MXFLASHC_VERSION_ECT) */
}


#elif (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)
/*******************************************************************************
 * Function Name: SelfTests_Check_StartUp_Cfg_CRC
 *******************************************************************************
 *
 * Summary:
 *  This function compares the calculated CRC (uint32_t Current_CRC) with the
 *  CRC previously stored in Flash if the status semaphore for a stored CRC is
 *  set.
 *  If the semaphore status is not set, it calculates a new CRC
 *  (uint32_t Current_CRC) and stores it to Flash and sets the status semaphore.
 *
 * Parameters:
 *  uint32_t Current_CRC - CRC to compare with the saved CRC.
 *
 * Return:
 *  0 - No error, CRC matched or Flash write is successful.
 *  Not 0 - Error detected, CRC did not match or Flash write failed.
 *
 ******************************************************************************/
static uint8_t SelfTests_Check_StartUp_Cfg_CRC(uint32_t Current_CRC)
{
    uint8_t* regPointer;
    uint8_t  ret = ERROR_STATUS;
    cy_en_flashdrv_status_t status;

    /* The pointer to Flash base */
    #if !defined(CY_IP_MXFLASHC_VERSION_ECT)
    regPointer = (uint8_t*)CY_FLASH_BASE;
    #else
    regPointer = (uint8_t*)CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE;
    #endif
    /* Check if FLASH CRC semaphore is set */
    #if !defined(CY_IP_MXFLASHC_VERSION_ECT)
    if (regPointer[CY_FLASH_SIZE - CRC_STARTUP_SEMAPHORE_SHIFT] == CRC_STARTUP_SEMAPHORE)
    #else
    if (regPointer[CONF_REG_FLASH_SMALL_SECTOR_SIZE - CRC_STARTUP_SEMAPHORE_SHIFT] ==
        CRC_STARTUP_SEMAPHORE)
    #endif
    {
        /* Check if an intentional error should be made for testing */
        #if (ERROR_IN_STARTUP_CONF_REG)

        /* Make an artificial error in CRC */
        Current_CRC++;
        #endif /* End (ERROR_IN_STARTUP_CONF_REG) */
        /* if "yes" - check the calculated FLASH CRC from CRC component with CRC stored in Flash */
        #if !defined(CY_IP_MXFLASHC_VERSION_ECT)
        if ((regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 0u] == ((Current_CRC >> 0u) & 0xFFuL)) &&
            (regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 1u] == ((Current_CRC >> 8u) & 0xFFuL)) &&
            (regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 2u] == ((Current_CRC >> 16u) & 0xFFuL)) &&
            (regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 3u] == ((Current_CRC >> 24u) & 0xFFuL)))
        #else
        if ((regPointer[CONF_REG_FLASH_SMALL_SECTOR_SIZE - CRC_STARTUP_LO + 0u] ==
             ((Current_CRC >> 0u) & 0xFFuL)) &&
            (regPointer[CONF_REG_FLASH_SMALL_SECTOR_SIZE - CRC_STARTUP_LO + 1u] ==
             ((Current_CRC >> 8u) & 0xFFuL)) &&
            (regPointer[CONF_REG_FLASH_SMALL_SECTOR_SIZE - CRC_STARTUP_LO + 2u] ==
             ((Current_CRC >> 16u) & 0xFFuL)) &&
            (regPointer[CONF_REG_FLASH_SMALL_SECTOR_SIZE - CRC_STARTUP_LO + 3u] ==
             ((Current_CRC >> 24u) & 0xFFuL)))
        #endif /* !defined(CY_IP_MXFLASHC_VERSION_ECT) */
        {
            ret = PASS_COMPLETE_STATUS;
        }
    }
    else
    {
        /* Read last row from Flash */
        for (uint16_t i = 0; i < CY_FLASH_SIZEOF_ROW; i++)
        {
            ((uint8_t*)flashRowData)[i] = regPointer[LAST_ROW_IN_FLASH_OFFSET + i];
        }


        /* If "no" - store the calculated FLASH CRC and status byte to the last bytes in Flash with the defined
         * shift. */
        for (uint8_t i = 0; i < sizeof(Current_CRC); i++)
        {
            ((uint8_t*)flashRowData)[CY_FLASH_SIZEOF_ROW  - CRC_STARTUP_LO +
                                     i] = (uint8_t)((Current_CRC >> (8u * i)) & 0xFFuL);
        }

        ((uint8_t*)flashRowData)[CY_FLASH_SIZEOF_ROW -
                                 CRC_STARTUP_SEMAPHORE_SHIFT] = CRC_STARTUP_SEMAPHORE;

        /* Write the array to the last FLASH row */
        #if !defined(CY_IP_MXFLASHC_VERSION_ECT)
        status = Cy_Flash_WriteRow(LAST_ROW_IN_FLASH_ADDR, flashRowData);
        #else
        Cy_Flashc_MainWriteEnable();
        status = Cy_Flash_ProgramRow(LAST_ROW_IN_FLASH_ADDR, flashRowData);
        Cy_Flashc_MainWriteDisable();
        #endif
        if (CY_FLASH_DRV_SUCCESS == status)
        {
            /* Return the status that CRC is already stored */
            ret = CRC_SAVED_STATUS;
        }
    }

    return ret;
}


#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */


/*******************************************************************************
 * Function Name: SelfTests_StartUp_ConfigReg(void)
 *******************************************************************************
 *
 * Summary:
 *  This function call checks the configuration registers by comparing the value
 *  stored in Flash with the current configuration registers value.
 *  If the values are different, the function returns a fail.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  0 - pass test.
 *  1 - fail test.
 *
 ******************************************************************************/
uint8_t SelfTests_StartUp_ConfigReg(void)
{
    #if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)
    uint32_t* cfgRegPointer;
    uint8_t ret = OK_STATUS;
    uint32_t tmp;

    /* Set the base address to Flash where configuration registers are stored */
    cfgRegPointer = (uint32_t*)(CONF_REG_FIRST_ROW_ADDR);

    /* Compare the register values with the saved values */
    uint32_t i = 0u;
    while (((i < (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u]))) && (ret == OK_STATUS)))
    {
        tmp = cfgRegPointer[i];

        /* Check if an intentional error should be made for testing */
        #if (ERROR_IN_STARTUP_CONF_REG)

        /* Make an artificial error in the configuration register */
        tmp++;
        #endif /* End (ERROR_IN_STARTUP_CONF_REG) */

        if (tmp != CY_GET_REG32(regs32_ToTest[i]))
        {
            ret = ERROR_STATUS;
        }
        i++;
    }

    /* Return the OK status if no error detected */
    return ret;

    #elif (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)
    uint32_t calculated_CRC;

    /* Fill the Flash row with "0" */
    (void)memset(flashRowData, 0x00, sizeof(flashRowData));

    /* Store the 32Bit registers */
    for (uint32_t i = 0u; i < (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u])); i++)
    {
        flashRowData[i] = CY_GET_REG32(regs32_ToTest[i]);
    }

    /* Calculate CRC of Start Up registers */
    calculated_CRC =
        SelfTests_CRC32((uint32_t)flashRowData,
                        (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u])));

    /* Compare the calculated CRC with the previously saved and return the status of matches */
    return SelfTests_Check_StartUp_Cfg_CRC(calculated_CRC);
    #endif /* (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */
}


/* [] END OF FILE */
