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
void USBUARTStart();/* USBUART Start Wrapper Prototype */
void USBUARTPutString(const char *string, const uint8_t len);
void USBUARTGetString(char * string,uint8_t len);

uint8_t USBUARTGetTxBufferCount();
uint8_t USBUARTGetRxBufferCount();

#endif
/* [] END OF FILE */
