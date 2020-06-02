/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef USBUART_FREERTOS_H
#define USBUART_FREERTOS_H
    
#include "project.h"
#include "stdio.h"
    
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Public Function *************************************************/
void vUSBUARTStart();/* USBUART Start Wrapper Prototype */
void vUSBUARTPutString(const char string[],const uint8_t ucLen);
void vUSBUARTGetString(char * cString,uint8_t ucLen);

#endif
/* [] END OF FILE */
