/*******************************************************************************
* File Name: SelfTest_IO.h
*
* Description:
*  This file provides constants and parameter values used for I/O self
*  tests.
*
*******************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/**
* \addtogroup group_io
* \{
*
* Digital I/Os are arranged into ports, with up to eight pins per port. Some of 
* the I/O pins are multiplexed with special functions (USB, debug port, crystal oscillator). 
* Special functions are enabled using control registers associated with the specific functions.
* The test goal is to ensure that I/O pins are not shorted to GND or Vcc.
*
* \section group_io_more_information More Information
*
* In normal operating conditions, the pin-to-ground and pin-to-VCC resistances are very high. To detect any shorts, 
* resistance values are compared with the internal pull-up resistors. To detect a pin-to-ground short, the pin is 
* configured in the resistive pull-up drive mode. Under normal conditions, the CPU reads a logical one because of 
* the pull-up resistor. If the pin is connected to ground through a small resistance, the input level is recognized 
* as a logical zero. To detect a sensor-to-VCC short, the sensor pin is configured in the resistive pull-down drive 
* mode. The input level is zero under normal conditions. If the pin is connected to VCC through a small resistance, 
* the input level is recognized as a logical one.
*
*
* \defgroup group_gpio_macros Macros
* \defgroup group_gpio_functions Functions
*/

#if !defined(SELFTEST_IO_H)
    #define SELFTEST_IO_H
#include "SelfTest_common.h"

/***************************************
* Function Prototypes
***************************************/

/**
* \addtogroup group_gpio_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_IO
****************************************************************************//**
*
*  This function performs I/O tests to detect pin shorts to Ground or Vcc.
*  Not all pins maybe compatible with this test based on applications specifics.
*  The users must fill up the "PinToTest" table corresponding to their needs.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed (Shorts to VCC) <br>
*  2 - Test failed (Shorts to GND)
*
*******************************************************************************/
uint8_t SelfTest_IO(void);



/*******************************************************************************
* Function Name: SelfTest_IO_GetPinError
****************************************************************************//**
*
*  This function returns a PIN number that cause an error in SelfTest_IO function.
*
*
* \return
* PIN number that cause an error
*
*******************************************************************************/
uint8_t SelfTest_IO_GetPinError(void);


/*******************************************************************************
* Function Name: SelfTest_IO_GetPortError
****************************************************************************//**
*
*  This function returns a PORT number that cause an error in SelfTest_IO function.
*
*
* \return
* PORT number that cause an error
*
*******************************************************************************/
uint8_t SelfTest_IO_GetPortError(void);

/** \} group_gpio_functions */


/***************************************
* Initial Parameter Constants
***************************************/
#if CY_CPU_CORTEX_M0P
#if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)

/** \addtogroup group_gpio_macros
* \{
*/
/** Number of IO ports. This may differ depending on the device used(CAT1A, CAT1B(PSoC C3), CAT1C, CAT2). */
/* Number of IO ports: PORT0 - PORT12 */
#define IO_PORTS                        (12u)
/** \} group_gpio_macros */

/** \cond INTERNAL */

#elif defined(CY_DEVICE_SERIES_PSOC_4500S) || defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS)
/* Number of IO ports: PORT0 - PORT7 */
#define IO_PORTS                        (8u)

#elif defined(CY_DEVICE_SERIES_PSOC_4100S)
/* Number of IO ports: PORT0 - PORT4 */
#define IO_PORTS                        (5u)

#elif defined(CY_DEVICE_SERIES_PSOC_4100T_PLUS)
/* Number of IO ports: PORT0 - PORT6 */
#define IO_PORTS                        (7u)
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX) */


#elif CY_CPU_CORTEX_M4
/* Number of IO ports: PORT0 - PORT14 */
#define IO_PORTS                        (15u)

#elif CY_CPU_CORTEX_M7
/* Number of IO ports: PORT0 - PORT14 */
#define IO_PORTS                        (33u)
#elif CY_CPU_CORTEX_M33
/* Number of IO ports: PORT0 - PORT9 */
#define IO_PORTS                        (10u)
#endif

/** \endcond */

/** \addtogroup group_gpio_macros
* \{
*/
/** Number of IO pins per port */
#define IO_PINS                         (8u)

/** Pins bit mask */
#define IO_PINS_MASK                    (IO_PINS - 1u)

#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M0P) && (CY_CPU_CORTEX_M0P)))
/** Optimal delay cycle value needed to setup the GPIO drive mode
 *  in Release configuration. This may differ depending on the device used(CAT1A, CAT1B(PSoC C3), CAT1C, CAT2).*/
#define DELAY_DRIVE_MODE_SETUP          (10u)
/** \} group_gpio_macros */

/** \cond INTERNAL */

#elif CY_CPU_CORTEX_M7
/* Optimal delay cycle value needed for setup the GPIO drive mode
 * in Release configuration */
#define DELAY_DRIVE_MODE_SETUP          (500u)
#elif CY_CPU_CORTEX_M33
/* Optimal delay cycle value needed for setup the GPIO drive mode
 * in Release configuration */
#define DELAY_DRIVE_MODE_SETUP          (500u)
#endif

/** \endcond */

/** \} group_io */
#endif /* SELFTEST_MEMORY_H */

/* [] END OF FILE */
