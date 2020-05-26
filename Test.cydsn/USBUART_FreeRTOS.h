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
#include "string.h"
    
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* USBUART Start Wrapper Prototype*/
void vUSBUARTStart();

/* Tx Prototype Function */
void vUSBUARTPutString(const char string[]);
void vUSBUARTPutChar(const char ch);
void vUSBUARTTxTask();

/* Rx Prototype Function */
void vUSBUARTGetChar(char *ch);
void vUSBUARTRxTask();
    
/* FreeRTOS Hundle */
QueueHandle_t xUartTxFifoQue;
QueueHandle_t xUartRxFifoQue;

SemaphoreHandle_t xUartTxBinarySemaphor;
SemaphoreHandle_t xUartRxBinarySemaphor;

void vUSBUARTStart(){
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

/* USBUART Start Wrapper Prototype*/
void USBUART_Setup(){
    xUartTxFifoQue = xQueueCreate(64,1);//USBUART tx buffer queue.
    xUartRxFifoQue = xQueueCreate(64,1);//USBUART rx buffer queue.
    xUartTxBinarySemaphor = xSemaphoreCreateBinary();//Binary semaphore for tx buffer prevent write conflict.
    xUartRxBinarySemaphor = xSemaphoreCreateBinary();//Binary semaphore for Rx buffer prevent Read conflict.
    
    xSemaphoreGive(xUartTxBinarySemaphor) ;
}

/* Tx Function */
void vUSBUARTPutString(const char *cString){    
    xSemaphoreTake(xUartTxBinarySemaphor,portMAX_DELAY);//Prevent queue write conflict.
    for(uint8_t ucCount=0; ucCount<strlen(cString); ucCount++){
        vUSBUARTPutChar(cString[ucCount]);
    }    
    xSemaphoreGive(xUartTxBinarySemaphor);
}

void vUSBUARTPutChar(const char ch){
    xQueueSendToBack(xUartTxFifoQue,&ch,portMAX_DELAY);
}

void vUSBUARTTxTask(){
    char cUartTxBuffer[64];//One packet tx buffer
    uint8_t ucUartTxCounter = 0;
    uint8_t ucUartTxQueCounter = 0;
    
    TickType_t xTick = xTaskGetTickCount();
    for(;;){
        ucUartTxQueCounter = uxQueueMessagesWaiting(xUartTxFifoQue);
        for(ucUartTxCounter=0;ucUartTxCounter<ucUartTxQueCounter;ucUartTxCounter++){
            xQueueReceive(xUartTxFifoQue,&cUartTxBuffer[ucUartTxCounter],0);
        }        
        
        if(ucUartTxQueCounter != 0){
            if(USBUART_CDCIsReady()){
                USBUART_PutData((uint8_t*)cUartTxBuffer,ucUartTxQueCounter);
                ucUartTxCounter = 0;

            }           
        }
        vTaskDelayUntil(&xTick,5);
    }
}

/* Rx Function */
void vUSBUARTGetString(char * cString,uint8_t len){
    xSemaphoreTake(xUartRxBinarySemaphor,portMAX_DELAY);//Prevent queue read conflict.
    for(uint8_t ucCount=0; ucCount<len; ucCount++){
        vUSBUARTGetChar(&cString[ucCount]);
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
