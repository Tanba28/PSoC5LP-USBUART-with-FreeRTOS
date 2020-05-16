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

void vPutString(const char string[]);
void vUSBUARTPutChar(const char ch);
void vUSBUARTTxTask();

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
QueueHandle_t xUartRxFifoQue;

SemaphoreHandle_t xUartTxBinarySemaphor;

void USBUART_Setup(){
    xUartTxFifoQue = xQueueCreate(64,1);//USBUART tx buffer queue.
    xUartRxFifoQue = xQueueCreate(64,1);//USBUART tx buffer queue.
    xUartTxBinarySemaphor = xSemaphoreCreateBinary();//Binary semaphore for tx buffer prevent write conflict.
    
    xSemaphoreGive(xUartTxBinarySemaphor) ;
}

void vUSBUARTPutString(const char *cString){    
    xSemaphoreTake(xUartTxBinarySemaphor,portMAX_DELAY);//Prevent queue write conflict.
    for(uint8_t ucCount=0; ucCount<strlen(cString); ucCount++){
        vUSBUARTPutChar(cString[ucCount]);        
    }    
}

void vUSBUARTPutChar(const char ch){
    xQueueSendToBack(xUartTxFifoQue,&ch,portMAX_DELAY);
}

void vUSBUARTTxTask(){
    char cUartTxBuffer[64];//One packet tx buffer
    uint8_t ucUartTxCounter = 0;
    uint8_t ucUartTxQueCounter = 0;
    
    for(;;){  
        xQueueReceive(xUartTxFifoQue,&cUartTxBuffer[ucUartTxCounter],portMAX_DELAY);
        ucUartTxQueCounter = uxQueueMessagesWaiting(xUartTxFifoQue);
        
        if(uxQueueMessagesWaiting(xUartTxFifoQue) == 0 || ucUartTxCounter == 63){
            if(USBUART_CDCIsReady()){
                USBUART_PutData((uint8_t*)cUartTxBuffer,ucUartTxCounter+1);
                ucUartTxCounter = 0;

            }
            
            if(uxQueueMessagesWaiting(xUartTxFifoQue) == 0){
                xSemaphoreGive(xUartTxBinarySemaphor) ;
            } 
        }
        
        else{
            ucUartTxCounter++;
        }
    }
}

void vUSBUARTGetChar(char *ch){
    xQueueReceive(xUartRxFifoQue,ch,portMAX_DELAY);
}

void vUSBUARTRxTask(){
    char cUartRxBuffer[64];//One packet rx buffer
    uint8_t ucUartRxCounter = 0;
    uint8_t ucUartRxQueCounter = 0;
    
    TickType_t tick = xTaskGetTickCount();
        
    for(;;){
        ucUartRxQueCounter = uxQueueMessagesWaiting(xUartRxFifoQue);

        if(ucUartRxQueCounter < 64 && ucUartRxCounter > 0){            
            xQueueSendToBack(xUartRxFifoQue,&cUartRxBuffer[ucUartRxCounter-1],portMAX_DELAY);
            ucUartRxCounter--;
        }       
        
        if(ucUartRxCounter < 64){
            USBUART_Connection_Check();
            if (USBUART_DataIsReady()) {
                cUartRxBuffer[ucUartRxCounter] = USBUART_GetChar();
                ucUartRxCounter++;
            }
        }
        vTaskDelayUntil(&tick,10);
    }
}
#endif
/* [] END OF FILE */
