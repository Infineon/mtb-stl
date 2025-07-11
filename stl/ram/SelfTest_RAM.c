/* *****************************************************************************
* File Name: SelfTest_RAM.c
*
* Description:
*  This file provides the source code to the API for the runtime SRAM self tests.
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

#include "cy_pdl.h"
#include "SelfTest_RAM.h"
#include "SelfTest_ErrorInjection.h"

uint8_t SelfTest_SRAM_MARCH(uint8_t* startAddr, uint32_t size, uint8_t* buffAddr,
                            uint32_t buffSize);
uint8_t SelfTest_SRAM_GALPAT(uint8_t* startAddr, uint32_t size, uint8_t* buffAddr,
                             uint32_t buffSize);

uint8_t SRAM_Test_Read0(uint8_t* stPtr, uint8_t* endPtr);
void SRAM_Test_Write_0(uint8_t* stPtr, uint8_t* endPtr);
void SRAM_Test_Write_1(uint8_t* stPtr, uint8_t* endPtr);
uint8_t SRAM_Test_Read0_Write1_Dec(uint8_t* stPtr, uint8_t* endPtr);
uint8_t SRAM_Test_Read0_Write1_Inc(uint8_t* stPtr, uint8_t* endPtr);
uint8_t SRAM_Test_Read1_Write0_Dec(uint8_t* stPtr, uint8_t* endPtr);
uint8_t SRAM_Test_Read1_Write0_Inc(uint8_t* stPtr, uint8_t* endPtr);
uint8_t SRAM_Test_Read1_by_Inverting_1Byte(uint8_t* stPtr, uint8_t* endPtr, uint8_t* invertingByte);
uint8_t SRAM_Test_Read0_by_Inverting_1Byte(uint8_t* stPtr, uint8_t* endPtr, uint8_t* invertingByte);
void copy_buffer(uint8_t* srcPtr, uint8_t* srcEndPtr, uint8_t* destPtr);

/*******************************************************************************
* Function Name: SelfTest_SRAM
********************************************************************************
*
*  This function performs the self test on a block of RAM. It can detect stuck-at faults and direct
*  coupling faults. The RAM tests are destructible in nature, so the function also accepts a temporary
*  buffer that can be used to store/restore memory block under test. The RAM block under test
*  must not overlap with memory that is not supposed to be overwritten for e.g. stack. This kind of
*  situation must be tested at the startup.  Because of RAM destructible behavior, it is advisable
*  to disable the interrupts before starting the test.
*
* \param type
* The type of RAM test to run.
* \param startAddr
* The pointer to the start of the RAM block to test.
* \param size
* The size of RAM block to test.
* \param buffAddr
* The pointer to the start of the buffer to use for store/restore data of RAM block to test.
* If NULL, test would be performed without store/restore being performed.
* \param buffSize
* The size of the buffer, which is used for store/restore. If buffSize is smaller than the size,
* the test internally loops through the RAM block operating on memory size
* equal to buffSize in each iteration.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/

uint8_t SelfTest_SRAM(stl_sram_test_mode_t type, uint8_t* startAddr, uint32_t size,
                      uint8_t* buffAddr, uint32_t buffSize)
{
    uint8_t testStatus = 0u;
    uint32_t buffSizeL = buffSize;
    uint8_t* buffAddrL = buffAddr;
    /* Check the proper buffer is provided for store/restore */
    if ((buffAddrL != NULL) && (buffSizeL != 0u))
    {
        if (buffSizeL > size)
        {
            buffSizeL = size;
        }
    }
    else
    {
        buffAddrL = NULL;
        buffSizeL = 0u;
    }

    /* Check for the test type */
    if (type == SRAM_GALPAT_TEST_MODE)
    {
        testStatus= SelfTest_SRAM_GALPAT(startAddr, size, buffAddrL, buffSizeL);
    }
    else
    {
        /* Default MARCH */
        testStatus= SelfTest_SRAM_MARCH(startAddr, size, buffAddrL, buffSizeL);
    }
    return testStatus;
}


/*******************************************************************************
 * Function Name: SelfTests_SRAM_March
 ********************************************************************************
 *
 *  This function perform SRAM self test using March method.
 *  - Stores the data from RAM under test to a buffer (optional)
 *  - Writes 0 to full block of RAM under test
 *  - Reads 0 and writes 1 on the block of RAM under test
 *  - Reads 1 and writes 0 on the block of RAM under test
 *  - Reads 0 from the block of RAM under test
 *  - Reads 0 and writes 1 on the block of RAM under test in Decrementing order
 *  - Reads 1 and writes 0 on the block of RAM under test in Decrementing order
 *  - Reads 0 from the block of RAM under test
 *  - Restores the data back from buffer to block of RAM that is tested
 *
 * \param type
 * The type of RAM test to run.
 * \param startAddr
 * The pointer to the start of the RAM block to test.
 * \param size
 * The size of RAM block to test.
 * \param buffAddr
 * The pointer to the start of the buffer to use for store/restore data of RAM block to test.
 * If NULL, test would be performed without store/restore being performed.
 * \param buffSize
 * The size of the buffer, which is used for store/restore. If buffSize is smaller than the size,
 * the test internally loops through the RAM block operating on memory size
 * equal to buffSize in each iteration.
 *
 * \return
 *  0 - Test passed <br>
 *  1 - Test failed
 *
 **********************************************************************************/

uint8_t SelfTest_SRAM_MARCH(uint8_t* startAddr, uint32_t size, uint8_t* buffAddr, uint32_t buffSize)
{
    uint8_t testStatus = OK_STATUS;
    uint8_t* startAddrL = startAddr;
    uint32_t buffSizeL = buffSize;
    if (buffSizeL == 0u)
    {
        buffSizeL = size;
    }
    for (uint32_t i = 0u; i < size; i += buffSizeL)
    {
        /* Check if a copy required */
        if (buffAddr != NULL)
        {
            /* Copy SRAM area, which is being tested in this loop */
            copy_buffer(startAddrL, (startAddrL + buffSizeL), buffAddr);
        }

        SRAM_Test_Write_0(startAddrL, (startAddrL + buffSizeL));
        testStatus = SRAM_Test_Read0_Write1_Inc(startAddrL, (startAddrL + buffSizeL));
        if (testStatus == OK_STATUS)
        {
            testStatus = SRAM_Test_Read1_Write0_Inc(startAddrL, (startAddrL + buffSizeL));
        }
        if (testStatus == OK_STATUS)
        {
            testStatus = SRAM_Test_Read0(startAddrL, (startAddrL + buffSizeL));
        }
        if (testStatus == OK_STATUS)
        {
            testStatus = SRAM_Test_Read0_Write1_Dec(startAddrL, (startAddrL + buffSizeL));
        }
        if (testStatus == OK_STATUS)
        {
            testStatus = SRAM_Test_Read1_Write0_Dec(startAddrL, (startAddrL + buffSizeL));
        }
        if (testStatus == OK_STATUS)
        {
            testStatus = SRAM_Test_Read0(startAddrL, (startAddrL + buffSizeL));
        }
        if (buffAddr != NULL)
        {
            /* Copy buffer back into SRAM area */
            copy_buffer(buffAddr, (buffAddr + buffSizeL), startAddrL);
        }
        if (testStatus != OK_STATUS)
        {
            break;
        }
        startAddrL = startAddrL + buffSizeL;
    }
    return testStatus;
}


/*******************************************************************************
 * Function Name: SelfTests_SRAM_GALPAT
 ********************************************************************************
 *
 * Summary:
 *  This function performs SRAM self test using GALPAT method.
 *  - Stores the data from RAM under test to a buffer (optional)
 *  - Writes 0 to full block of RAM under test
 *  - Writes 1 to a reference Inverting byte, reads 1 from reference Inverting byte and reads 0 from
 *    all other bytes
 *  - Repeat above step to all the bytes (cells) by incrementing Inverting byte's position until
 *    whole RAM block is tested
 *  - Writes 1 to full block of RAM under test
 *  - Writes 0 to a reference Inverting byte, reads 0 from reference Inverting byte and reads 1 from
 *    all other bytes. Write 1 to Inverting Byte
 *  - Repeat above step to all the bytes (cells) by incrementing Inverting byte's position until
 *    whole RAM block is tested
 *  - Restores the data back from buffer to block of RAM that is tested
 *
 * \param type
 * The type of RAM test to run.
 * \param startAddr
 * The pointer to the start of the RAM block to test.
 * \param size
 * The size of RAM block to test.
 * \param buffAddr
 * The pointer to the start of the buffer to use for store/restore data of RAM block to test.
 * If NULL, test would be performed without store/restore being performed.
 * \param buffSize
 * The size of the buffer, which is used for store/restore. If buffSize is smaller than the size,
 * the test internally loops through the RAM block operating on memory size
 * equal to buffSize in each iteration.
 *
 * \return
 *  0 - Test passed <br>
 *  1 - Test failed
 *
 **********************************************************************************/
uint8_t SelfTest_SRAM_GALPAT(uint8_t* startAddr, uint32_t size, uint8_t* buffAddr,
                             uint32_t buffSize)
{
    uint8_t testStatus = OK_STATUS;
    uint8_t* startAddrL = startAddr;
    uint8_t* invertingByte = startAddr;
    uint32_t buffSizeL = buffSize;

    if (buffSizeL == 0u)
    {
        buffSizeL = size;
    }
    for (uint32_t i = 0u; i < size; i += buffSizeL)
    {
        /* Check if copy required */
        if (buffAddr != NULL)
        {
            /* Copy SRAM area which is being tested in this loop */
            copy_buffer(startAddrL, (startAddrL + buffSizeL), buffAddr);
        }

        SRAM_Test_Write_0(startAddrL, (startAddrL + buffSizeL));
        do
        {
            testStatus = SRAM_Test_Read0_by_Inverting_1Byte(startAddrL, (startAddrL + buffSizeL),
                                                            invertingByte);
            if (testStatus != OK_STATUS)
            {
                break;
            }
            uint8_t read0 = 0u;
            *invertingByte = read0;
            invertingByte += 1u;
        } while(invertingByte < (startAddrL + buffSizeL));

        if (testStatus == OK_STATUS)
        {
            invertingByte = startAddrL;
            SRAM_Test_Write_1(startAddrL, (startAddrL + buffSizeL));
            do
            {
                testStatus = SRAM_Test_Read1_by_Inverting_1Byte(startAddrL,
                                                                (startAddrL + buffSizeL),
                                                                invertingByte);
                if (testStatus != OK_STATUS)
                {
                    break;
                }
                uint8_t read1 = 0xffu;
                *invertingByte = read1;
                invertingByte += 1u;
            } while(invertingByte < (startAddrL + buffSizeL));
        }
        if (buffAddr != NULL)
        {
            /* Copy the buffer back into SRAM area */
            copy_buffer(buffAddr, (buffAddr + buffSizeL), startAddrL);
        }
        if (testStatus != OK_STATUS)
        {
            break;
        }
        startAddrL = startAddrL + buffSizeL;
    }
    return testStatus;
}


/*******************************************************************************
* Function Name: SelfTest_SRAM_Stack
****************************************************************************//**
*
*  This function perform self test on Stack. It can detect stuck-at faults and direct coupling
*  faults. The RAM tests are destructible in nature so this function copies the stack in use to
*  the provided RAM area. Once the test is completed, stack is copied back to its original
*  location. Because of RAM destructible behavior, it is advisable to disable
*  interrupts before starting the test.
*
* \param stackBase
* The pointer to the Stack Base
* \param stackSize
* The size of Stack
* \param altStackBase
* The pointer to start of the RAM area to be used as alternate Stack Base. RAM address
* from altStackBase to (altStackBase - stackSize) is used to store/restore the Stack under test.
* The existing content of this area will be destructed and this area must not overlap with the Stack
* under test. The platform might set stack limit using __set_MSPLIM, ensure that
* the alternate stack is within the stack limit set by the platform
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
static uint32_t stackPointer;
static uint8_t currentStackSize;


uint8_t SelfTest_SRAM_Stack(uint8_t* stackBase, uint32_t stackSize, uint8_t* altStackBase)
{
    uint8_t stackTestStatus = OK_STATUS;
    __asm volatile("mov %0, sp" : "=r" (stackPointer));
    currentStackSize = (uint8_t)((uint32_t)stackBase - stackPointer);

    /* Check the proper buffer address is provided for store/restore */
    if (((uint32_t)altStackBase >= ((uint32_t)stackBase - stackSize)) &&
        ((uint32_t)altStackBase <= (uint32_t)stackBase))
    {
        return ERROR_STATUS;
    }
    copy_buffer((uint8_t*)stackPointer, stackBase, altStackBase - currentStackSize);
    __asm volatile("mov sp, %0" :: "r" ((uint32_t)altStackBase - currentStackSize));
    stackTestStatus = SelfTest_SRAM_MARCH(stackBase - stackSize, stackSize, NULL, 0);
    copy_buffer((uint8_t*)altStackBase - currentStackSize, altStackBase, (uint8_t*)stackPointer);
    __asm volatile("mov sp, %0" :: "r" (stackPointer));
    return stackTestStatus;
}


/*******************************************************************************
* Function Name: SRAM_Test_Write_0
****************************************************************************//**
*
*  Writes 0 to the entire block of RAM under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be written.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be written.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
void SRAM_Test_Write_0(uint8_t* stPtr, uint8_t* endPtr)
{
    uint8_t* stPtrL = stPtr;
    do
    {
        *stPtrL = 0x0u;
        stPtrL += 1u;
    } while (stPtrL < endPtr);
    return;
}


/*******************************************************************************
* Function Name: SRAM_Test_Write_1
****************************************************************************//**
*
*  Writes 1 to the entire block of RAM under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be written.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be written.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
void SRAM_Test_Write_1(uint8_t* stPtr, uint8_t* endPtr)
{
    /* Check if an intentional error should be made for testing */
    #if (ERROR_IN_SRAM_GALPAT == 1u)
    *stPtr = 0u;
    return;
    #endif /* End (ERROR_IN_SRAM_GALPAT == 1u) */
    uint8_t* stPtrL = stPtr;
    do
    {
        *stPtrL = 0xffu;
        stPtrL += 1u;
    } while (stPtrL < endPtr);
    return;
}


/*******************************************************************************
* Function Name: SRAM_Test_Read0_Write1_Inc
****************************************************************************//**
*
*  Reads 0 and writes 1 on the block of RAM under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read0_Write1_Inc(uint8_t* stPtr, uint8_t* endPtr)
{
    uint8_t status_flag = 0u;
    uint8_t* stPtrL = stPtr;
    do
    {
        /* Check if an intentional error should be made for testing */
        #if (ERROR_IN_SRAM_MARCH == 1u)
        *stPtr =  0xffu;
        #endif /* End (ERROR_IN_SRAM_MARCH == 1u) */
        uint8_t read0 = *stPtrL;
        if (read0 == 0x0u)
        {
            *stPtrL = 0xffu;
            stPtrL += 1u;
        }
        else
        {
            status_flag = ERROR_STATUS;
            break;
        }
    } while(stPtrL < endPtr);
    return status_flag;
}


/*******************************************************************************
* Function Name: SRAM_Test_Read1_Write0_Inc
****************************************************************************//**
*
*  Reads 1 and writes 0 on the block of RAM under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read1_Write0_Inc(uint8_t* stPtr, uint8_t* endPtr)
{
    uint8_t status_flag = 0u;
    uint8_t* stPtrL = stPtr;
    do
    {
        uint8_t read1 = *stPtrL;
        if (read1 == 0xFFu)
        {
            read1 = 0u;
            *stPtrL = read1;
            stPtrL += 1u;
        }
        else
        {
            status_flag = ERROR_STATUS;
            break;
        }
    } while(stPtrL < endPtr);
    return status_flag;
}


/*******************************************************************************
* Function Name: SRAM_Test_Read0
****************************************************************************//**
*
*  Reads 0 from the block of RAM under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read0(uint8_t* stPtr, uint8_t* endPtr)
{
    uint8_t status_flag = 0u;
    uint8_t* stPtrL = stPtr;
    do
    {
        uint8_t read0 = *stPtrL;
        if (read0 == 0x0u)
        {
            stPtrL += 1u;
        }
        else
        {
            status_flag = ERROR_STATUS;
            break;
        }
    } while(stPtrL < endPtr);
    return status_flag;
}


/*******************************************************************************
* Function Name: SRAM_Test_Read0_Write1_Dec
****************************************************************************//**
*
*  Reads 0 and writes 1 on the block of RAM under test in Decrementing order.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read0_Write1_Dec(uint8_t* stPtr, uint8_t* endPtr)
{
    uint8_t status_flag = 0u;
    uint8_t* endPtr1 = endPtr - 1u;
    do
    {
        uint8_t read0 = *endPtr1;
        if (read0 == 0u)
        {
            read0 = 0xffu;
            *endPtr1 = read0;
            endPtr1 = endPtr1 - 1u;
        }
        else
        {
            status_flag = ERROR_STATUS;
            break;
        }
    } while(endPtr1 >= stPtr);
    return status_flag;
}


/*******************************************************************************
* Function Name: SRAM_Test_Read1_Write0_Dec
****************************************************************************//**
*
*  Reads 1 and writes 0 on the block of RAM under test in Decrementing order.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read1_Write0_Dec(uint8_t* stPtr, uint8_t* endPtr)
{
    uint8_t status_flag = 0u;
    uint8_t* endPtr1 = endPtr - 1u;
    do
    {
        uint8_t read1 = *endPtr1;
        if (read1 == 0xFFu)
        {
            read1 = 0u;
            *endPtr1 = read1;
            endPtr1 = endPtr1 - 1u;
        }
        else
        {
            status_flag = ERROR_STATUS;
            break;
        }
    } while(endPtr1 >= stPtr);
    return status_flag;
}


/*******************************************************************************
* Function Name: copy_buffer
****************************************************************************//**
*
*  Stores/Restores the data from RAM/STACK under test to a buffer
*
* \param srcPtr
*  Pointer to the start address of the source buffer.
*
* \param srcEndPtr
*  Pointer to the end address of the source buffer.
*
* \param destPtr
*  Pointer to the start address of the destination buffer.
*
*******************************************************************************/
void copy_buffer(uint8_t* srcPtr, uint8_t* srcEndPtr, uint8_t* destPtr)
{
    uint8_t* srcPtrL = srcPtr;
    uint8_t* destPtrL = destPtr;
    do
    {
        *destPtrL = *srcPtrL;
        srcPtrL++;
        destPtrL++;
    } while(srcPtrL < srcEndPtr);
}


/*******************************************************************************
* Function Name: SRAM_Test_Read0_by_Inverting_1Byte
****************************************************************************//**
*
*  Writes 1 to a reference inverting byte, reads 1 from the reference inverting byte,
*  and reads 0 from all other bytes in the RAM block under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \param invertingByte
*  Pointer to the byte within the RAM block that will be inverted (written with 1).
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read0_by_Inverting_1Byte(uint8_t* stPtr, uint8_t* endPtr, uint8_t* invertingByte)
{
    uint8_t testStatus = 0u;
    uint8_t read0 = 0xffu;
    uint8_t* stPtrL = stPtr;
    *invertingByte = read0;
    do
    {
        if (stPtrL == invertingByte)
        {
            read0 = *invertingByte;
            if (read0 == 0xFFu)
            {
                stPtrL += 1u;
            }
            else
            {
                return ERROR_STATUS;
            }
        }
        else
        {
            read0 = *stPtrL;
            if (read0 == 0u)
            {
                read0 = *invertingByte;
                if (read0 == 0xFFu)
                {
                    stPtrL += 1u;
                }
                else
                {
                    return ERROR_STATUS;
                }
            }
            else
            {
                return ERROR_STATUS;
            }
        }
    } while(stPtrL < endPtr);
    return testStatus;
}


/*******************************************************************************
* Function Name: SRAM_Test_Read1_by_Inverting_1Byte
****************************************************************************//**
*
*  Writes 0 to a reference inverting byte, reads 0 from the reference inverting byte,
*  and reads 1 from all other bytes in the RAM block under test.
*
* \param stPtr
*  Pointer to the start address of the RAM block to be checked.
*
* \param endPtr
*  Pointer to the end address of the RAM block to be checked.
*
* \param invertingByte
*  Pointer to the byte within the RAM block that will be inverted (written with 0).
*
* \return
*  0 - Test passed <br>
*  1 - Test  failed
*
*******************************************************************************/
uint8_t SRAM_Test_Read1_by_Inverting_1Byte(uint8_t* stPtr, uint8_t* endPtr, uint8_t* invertingByte)
{
    uint8_t testStatus = 0u;
    uint8_t read1 = 0u;
    uint8_t* stPtrL = stPtr;
    *invertingByte = read1;
    do
    {
        if (stPtrL == invertingByte)
        {
            read1 = *invertingByte;
            if (read1 == 0u)
            {
                stPtrL += 1u;
            }
            else
            {
                return ERROR_STATUS;
            }
        }
        else
        {
            read1 = *stPtrL;
            if (read1 == 0xFFu)
            {
                read1 = *invertingByte;
                if (read1 == 0u)
                {
                    stPtrL += 1u;
                }
                else
                {
                    return ERROR_STATUS;
                }
            }
            else
            {
                return ERROR_STATUS;
            }
        }
    } while(stPtrL < endPtr);
    return testStatus;
}


/* [] END OF FILE */
