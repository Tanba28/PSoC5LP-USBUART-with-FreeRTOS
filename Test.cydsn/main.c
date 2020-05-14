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
#include "xprintf.h"
void vTestTask1();
void vTestTask2();
void FreeRTOS_Setup();



int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    FreeRTOS_Setup();
    USBUART_Start_Wrapper();
    
    xTaskCreate(vTestTask1,"test1",1000,NULL,3,NULL);
    xTaskCreate(vTestTask2,"test2",1000,NULL,3,NULL);
    
    USBUART_Setup();
    
    xTaskCreate(vUSBUARTTxTask,"test3",100,NULL,2,NULL);
    
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
    uint count = 0;
    TickType_t tick = xTaskGetTickCount();
    char buf[128];
    
    for(;;){
        LED_1_Write(1);  
        sprintf(buf,"A %07d Hello Hello Hello Hello Hello Hello Hello\r\n",xTaskGetTickCount());
        vUSBUARTPutString(buf);
        count++;
        vTaskDelayUntil(&tick,100);
    }    
}

void vTestTask2(){
    uint count = 0;
    vTaskDelay(5);  
    TickType_t tick = xTaskGetTickCount();
    char buf[128];    
    
    for(;;){      
        LED_1_Write(0);  
        sprintf(buf,"B %07d World World World World World World World\r\n",xTaskGetTickCount());
        vUSBUARTPutString(buf);
        count++;
        vTaskDelayUntil(&tick,100);
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
