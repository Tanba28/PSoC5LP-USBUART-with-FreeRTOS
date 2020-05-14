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



QueueHandle_t xUartTxFifoQue;

void USBUART_Setup(){    
    xUartTxFifoQue = xQueueCreate(4,64);
}

CYBIT USBUARTPutStringQue(const char8 string[]){
    BaseType_t xStatus;
    
    xStatus = xQueueSendToBack(xUartTxFifoQue,string,100);
    
    if(xStatus == errQUEUE_FULL){
        return 1;
    }
    return 0;
}

void vUartTxTask(){
    char txBuffer[64];
    BaseType_t xStatus;
    
    for(;;){
        xStatus = xQueueReceive( xUartTxFifoQue, &txBuffer, portMAX_DELAY );

        if(xStatus == pdPASS){
            if(USBUART_CDCIsReady()){
                USBUART_PutData((uint8_t*)txBuffer,strlen(txBuffer));
            }
        }
    }
    
}

#endif
/* [] END OF FILE */
