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
#include "USBUART_FreeRTOS.h"

/* Static Function *************************************************/
static void initVariable();

/* Static Tx Prototype Function */
static void putChar(const char ch);
static void txTask();

/* StaticRx Prototype Function */
static void getChar(char *ch);
static void rxTask();
    
/* FreeRTOS Hundle */
static QueueHandle_t tx_fifo_que;
static QueueHandle_t rx_fifo_que;

static SemaphoreHandle_t tx_binary_semaphor;
static SemaphoreHandle_t rx_binary_semaphor;

void USBUARTStart(){
    USBUART_Start(0, USBUART_DWR_VDDD_OPERATION);
    //500ms TimeOut
    for(uint8_t i=0;i<100;i++){
        if(0u != USBUART_bGetConfiguration()){
            USBUART_CDC_Init();
            break;
        }
        CyDelay(5);
    }
    initVariable();
    
    xTaskCreate(txTask,"UART_TX",100,NULL,3,NULL);
    xTaskCreate(rxTask,"UART_RX",100,NULL,3,NULL);
}

void USBUARTPutString(const char *string, const uint8_t len){    
    xSemaphoreTake(tx_binary_semaphor,portMAX_DELAY);//Prevent queue write conflict.
    for(uint8_t count=0; count<len; count++){
        putChar(string[count]);
    }    
    xSemaphoreGive(tx_binary_semaphor);
}

void USBUARTGetString(char * string,uint8_t len){
    xSemaphoreTake(rx_binary_semaphor,portMAX_DELAY);//Prevent queue read conflict.
    for(uint8_t count=0; count<len; count++){
        getChar(&string[count]);
    }  
    xSemaphoreGive(rx_binary_semaphor);
}

/* USBUART Setup*/
static void initVariable(){
    tx_fifo_que = xQueueCreate(64,1);//USBUART tx buffer queue.
    rx_fifo_que = xQueueCreate(64,1);//USBUART rx buffer queue.
    tx_binary_semaphor = xSemaphoreCreateBinary();//Binary semaphore for tx buffer prevent write conflict.
    rx_binary_semaphor = xSemaphoreCreateBinary();//Binary semaphore for rx buffer prevent Read conflict.
    
    xSemaphoreGive(tx_binary_semaphor);
    xSemaphoreGive(rx_binary_semaphor);
}

/* Tx Function */
static void putChar(const char ch){
    xQueueSendToBack(tx_fifo_que,&ch,portMAX_DELAY);
}

static void txTask(){
    char tx_buffer[64];//One packet tx buffer
    uint8_t tx_que_count = 0;
    
    TickType_t tick = xTaskGetTickCount();
    for(;;){
        tx_que_count = uxQueueMessagesWaiting(tx_fifo_que);
        for(uint8_t count=0;count<tx_que_count;count++){
            xQueueReceive(tx_fifo_que,&tx_buffer[count],0);
        }        
        
        if(tx_que_count != 0){
            if(USBUART_CDCIsReady()){
                USBUART_PutData((uint8_t*)tx_buffer,tx_que_count);
                
            }           
        }
        vTaskDelayUntil(&tick,5);
    }
}

/* Rx Function */

static void getChar(char *ch){
    xQueueReceive(rx_fifo_que,ch,portMAX_DELAY);
}

static void rxTask(){
    char rx_buffer[64];//One packet rx buffer
    uint8_t rx_buffer_count = 0;
    uint8_t rx_que_count = 0;
    
    TickType_t tick = xTaskGetTickCount();
        
    for(;;){
        rx_que_count = uxQueueMessagesWaiting(rx_fifo_que);

        if(rx_que_count < 64 && rx_buffer_count > 0){            
            xQueueSendToBack(rx_fifo_que,&rx_buffer[rx_buffer_count-1],portMAX_DELAY);
            rx_buffer_count--;
        }       
        
        if(rx_buffer_count < 64){         
            if (USBUART_DataIsReady()) {
                rx_buffer[rx_buffer_count] = USBUART_GetChar();
                rx_buffer_count++;
            }
        }
        
        vTaskDelayUntil(&tick,5);
    }
}


/* [] END OF FILE */
