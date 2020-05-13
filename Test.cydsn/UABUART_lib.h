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
#ifndef USBUART_LIB_H
#define USBUART_LIB_H
    
#include "project.h"
#include "stdio.h"
#include "string.h"
    
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void USBUART_Start_Wrapper();
void USBUART_Connection_Check();
void USBUART_PutString_Wrapper(const char8 string[]);

void USBUART_Start_Wrapper(){
    USBUART_Start(0, USBUART_DWR_VDDD_OPERATION);
    //500ms TimeOut
    for(uint8_t i=0;i<100;i++){
        if(0u != USBUART_bGetConfiguration()){
            USBUART_CDC_Init();
            break;
        }
        CyDelay(5);
    }
}
void USBUART_Connection_Check(){
    if(0u != USBUART_IsConfigurationChanged()){
        if(0u != USBUART_GetConfiguration()){
            USBUART_CDC_Init();
        }
    }
}
void USBUART_PutString_Wrapper(const char8 string[]){
    if(0u != USBUART_GetConfiguration()){
        USBUART_PutString(string);
    }
}

uint8_t uartTxQueue[64];
uint8_t uartTxCount = 0;
CYBIT uartZlpRequired = 0;

SemaphoreHandle_t xUartTxBinarySemaphore;
SemaphoreHandle_t xUartTxFifoBinarySemaphore;

QueueHandle_t xUartTxFifoQueue;

CY_ISR(uart_que){
    BaseType_t xHigherPriorityTaskWoken;
    
    xHigherPriorityTaskWoken = pdFALSE;
   
    xSemaphoreGiveFromISR( xUartTxBinarySemaphore, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void USBUART_Setup(){
    xUartTxBinarySemaphore = xSemaphoreCreateBinary();
    
    xUartTxFifoQueue = xQueueCreate(4,64);
    isr_1_StartEx(uart_que);
}

void vPutCh(){
    
}
void vUartTxIsrTask(){
    uint8_t txBuffer[65];//txBuffer[64] = \0
    BaseType_t xStatusQue;
    CYBIT uartZlpRequired = 0;
    for(;;){
        xSemaphoreTake(xUartTxBinarySemaphore, portMAX_DELAY);
        
        taskENTER_CRITICAL();
        
        xStatusQue = xQueueReceive(xUartTxFifoQueue,txBuffer,portMAX_DELAY);
        if(xStatusQue == pdPASS || uartZlpRequired){
            if(USBUART_CDCIsReady()){
                if(strlen((char*)txBuffer)==64){
                    USBUART_PutData(uartTxQueue
                }
            }
        }
        if((uartTxCount > 0) || uartZlpRequired){
            if(USBUART_CDCIsReady()){
                USBUART_PutData(uartTxQueue,uartTxCount);
                uartZlpRequired = (uartTxCount == 64);
                uartTxCount = 0;
            }
        }
        
        taskEXIT_CRITICAL();
        //USBUART_PutString_Wrapper("Hello World\r\n");
    }
    
}

#endif
/* [] END OF FILE */
