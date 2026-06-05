/*******************************************************************************
* File Name: SelfTest_IO.h
*
* Description:
*  This file provides constants and parameter values used for the I/O self
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

/**
 * \addtogroup group_io
 * \{
 *
 * Digital I/Os are arranged into ports with up-to-eight pins per port. Some of
 * the I/O pins are multiplexed with special functions (USB, debug port, crystal oscillator).
 * Special functions are enabled using control registers associated with the specific functions.
 * The test goal is to ensure that I/O pins are not shorted to GND or Vcc.
 *
 * \section group_io_more_information More Information
 *
 * In normal operating conditions, the pin-to-ground and pin-to-VCC resistances are very high. To
 * detect any shorts, resistance values are compared with the internal pull-up resistors. To detect
 * a pin-to-ground short, the pin is configured in the resistive pull-up Drive mode. Under normal
 * conditions, the CPU reads a logical 1 because of the pull-up resistor. If the pin is connected
 * to ground through a small resistance, the input level is recognized as a logical 0. To detect
 * a sensor-to-VCC short, the sensor pin is configured in the resistive pull-down drive mode. The
 * input level is zero under normal conditions. If the pin is connected to VCC through a
 * small resistance, the input level is recognized as a logical one.
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
*  Not all pins may be compatible with this test based on the applications specifics.
*  By default, this function uses the "PinToTest" array to determine which pins
*  to test. To set a custom pin mask, use the SelfTest_IO_SetPinMask() function.
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
*  This function returns a pin number that causes an error in the SelfTest_IO function.
*
*
* \return
* The pin number that causes an error.
*
*******************************************************************************/
uint8_t SelfTest_IO_GetPinError(void);


/*******************************************************************************
* Function Name: SelfTest_IO_GetPortError
****************************************************************************//**
*
*  This function returns a port number that causes an error in the SelfTest_IO function.
*
*
* \return
* PORT number that cause an error
*
*******************************************************************************/
uint8_t SelfTest_IO_GetPortError(void);


/*******************************************************************************
* Function Name: SelfTest_IO_SetPinMask
***************************************************************************//**
*
* Summary:
*  This function sets a custom pin mask to be used in the SelfTest_IO function.
*
* \param pinMaskArr The custom pin mask array. The length of the array should be equal
*  to the IO_PORTS value. Each element of the array is a mask of the GPIO port pins,
*  to be tested in the SelfTest_IO function. The port sequence is the same
*  as in the PORT_Regs array. Pass the NULL value to use the default pin mask.
*
*******************************************************************************/
void SelfTest_IO_SetPinMask(const uint8_t* pinMaskArr);

/** \} group_gpio_functions */


/***************************************
* Initial Parameter Constants
***************************************/
#if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)

/** \addtogroup group_gpio_macros
 * \{
 */
/** The number of IO ports. This may differ depending on the device used(CAT1A, CAT1B(PSoC C3), CAT1C,
 *  CAT2). */
/* The number of IO ports: PORT0 - PORT12 */
#define IO_PORTS                        (12u)
/** \} group_gpio_macros */

/** \cond INTERNAL */

#elif defined(CY_DEVICE_SERIES_PSOC_4500S)
/* The number of IO ports: PORT0 - PORT7 */
#define IO_PORTS                        (8u)

#elif defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS)
/* The number of IO ports: PORT0 - PORT7 */
#define IO_PORTS                        (8u)

#elif defined(CY_DEVICE_SERIES_PSOC_4100S)
/* The number of IO ports: PORT0 - PORT4 */
#define IO_PORTS                        (5u)

#elif defined(CY_DEVICE_SERIES_PSOC_4000T)
/* The number of IO ports: PORT0 - PORT4 */
#define IO_PORTS                        (5u)

#elif defined(CY_DEVICE_SERIES_PSOC_4000S)
/* The number of IO ports: PORT0 - PORT4 */
#define IO_PORTS                        (5u)

#elif defined(CY_DEVICE_SERIES_PSOC_4700S)
/* The number of IO ports: PORT0 - PORT4 */
#define IO_PORTS                        (5u)

#elif defined(CY_DEVICE_SERIES_PSOC_4100T_PLUS)
/* The number of IO ports: PORT0 - PORT6 */
#define IO_PORTS                        (7u)

#endif /* if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX) */

#if (defined(CY_DEVICE_SERIES_PSOC_61) || defined(CY_DEVICE_SERIES_PSOC_62) || \
    defined(CY_DEVICE_SERIES_PSOC_63) || defined(CY_DEVICE_SERIES_PSOC_64))
/* The number of IO ports: PORT0 - PORT14 */
#define IO_PORTS                        (15u)
#endif

#if (defined(CY_DEVICE_SERIES_XMC7100) || defined(CY_DEVICE_SERIES_XMC7200))
/* The number of IO ports: PORT0 - PORT14 */
#define IO_PORTS                        (33u)
#endif

#if (defined(CY_DEVICE_SERIES_PSC3M3) || defined(CY_DEVICE_SERIES_PSC3M5) || \
    defined(CY_DEVICE_SERIES_PSC3P2) || defined(CY_DEVICE_SERIES_PSC3P5))
/* The number of IO ports: PORT0 - PORT9 */
#define IO_PORTS                        (10u)
#endif

#if (defined(CY_DEVICE_SERIES_XMC5100) || defined(CY_DEVICE_SERIES_XMC5200) || defined(CY_DEVICE_SERIES_XMC5300))
/* The number of IO ports: PORT0 - PORT23 */
#define IO_PORTS                        (24u)
#endif


/** \endcond */

/** \addtogroup group_gpio_macros
 * \{
 */
/** The number of IO pins per port */
#define IO_PINS                         (8u)

/** The pins bit mask */
#define IO_PINS_MASK                    (IO_PINS - 1u)

#if (defined(CY_DEVICE_SERIES_PSOC_61) || defined(CY_DEVICE_SERIES_PSOC_62) || \
    defined(CY_DEVICE_SERIES_PSOC_63) || defined(CY_DEVICE_SERIES_PSOC_64) || defined (CY_DOXYGEN))
/** The optimal delay cycle value needed to set up the GPIO Drive mode
 *  in Release configuration. This may differ depending on the device used(CAT1A, CAT1B(PSoC C3),
 *  CAT1C, CAT2).*/
#define DELAY_DRIVE_MODE_SETUP              (10u)
/** \} group_gpio_macros */

/** \cond INTERNAL */

#elif (defined(CY_DEVICE_SERIES_XMC7100) || defined(CY_DEVICE_SERIES_XMC7200))
    #define DELAY_DRIVE_MODE_SETUP          (500u)

#elif (defined(CY_DEVICE_SERIES_PSC3M3) || defined(CY_DEVICE_SERIES_PSC3M5) || \
    defined(CY_DEVICE_SERIES_PSC3P2) || defined(CY_DEVICE_SERIES_PSC3P5))
    #define DELAY_DRIVE_MODE_SETUP          (500u)

#elif (defined(CY_DEVICE_SERIES_XMC5100) || defined(CY_DEVICE_SERIES_XMC5200) || defined(CY_DEVICE_SERIES_XMC5300))
    #define DELAY_DRIVE_MODE_SETUP          (500u)

#else /* PSOC4 devices */
    #define DELAY_DRIVE_MODE_SETUP          (10u)
#endif /* if (defined(CY_DEVICE_SERIES_PSOC_61) || defined(CY_DEVICE_SERIES_PSOC_62) ||
        *     defined(CY_DEVICE_SERIES_PSOC_63) || defined(CY_DEVICE_SERIES_PSOC_64))
        */

/** \endcond */

/** \} group_io */
#endif /* SELFTEST_MEMORY_H */

/* [] END OF FILE */
