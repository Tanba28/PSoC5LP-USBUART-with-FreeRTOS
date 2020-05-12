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
#include "project.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "UABUART_lib.h"

void vTestTask1();
void vTestTask2();
void FreeRTOS_Setup();

SemaphoreHandle_t xBinarySemaphore;

CY_ISR(uart_que){
    BaseType_t xHigherPriorityTaskWoken;
    
    xHigherPriorityTaskWoken = pdFALSE;
   
    xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void USBUART_Setup(){
    xBinarySemaphore = xSemaphoreCreateBinary();
    isr_1_StartEx(uart_que);
}

void vUartTxIsrTask(){
    for(;;){
        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
        
        USBUART_PutString_Wrapper("Hello World\r\n");
    }
    
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    FreeRTOS_Setup();
    USBUART_Start_Wrapper();
    
    xTaskCreate(vTestTask1,"test1",100,NULL,3,NULL);
    xTaskCreate(vTestTask2,"test2",100,NULL,3,NULL);
    
    USBUART_Setup();
    
    xTaskCreate(vUartTxIsrTask,"test3",100,NULL,3,NULL);
    
    vTaskStartScheduler();
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

void FreeRTOS_Setup(){
    extern void xPortPendSVHandler( void );
    extern void xPortSysTickHandler( void );
    extern void vPortSVCHandler( void );
    extern cyisraddress CyRamVectors[];

    CyRamVectors[ 11 ] = ( cyisraddress ) vPortSVCHandler;
    CyRamVectors[ 14 ] = ( cyisraddress ) xPortPendSVHandler;
    CyRamVectors[ 15 ] = ( cyisraddress ) xPortSysTickHandler;
}

void vTestTask1(){
    for(;;){
        vTaskDelay(1000);
        LED_1_Write(1);        
    }    
}
//LEDを消灯させるタスク
void vTestTask2(){
    vTaskDelay(500);
    for(;;){
        vTaskDelay(1000);
        LED_1_Write(0);        
    }    
}

//オーバーフローが起こった際のハンドラ
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    /* The stack space has been execeeded for a task, considering allocating more. */
    taskDISABLE_INTERRUPTS();
}/*---------------------------------------------------------------------------*/

//メモリの確保に失敗したときのハンドラ
void vApplicationMallocFailedHook( void )
{
    /* The heap space has been execeeded. */
    taskDISABLE_INTERRUPTS();
}



/* [] END OF FILE */
