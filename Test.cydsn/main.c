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
#include "string.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "USBUART_FreeRTOS.h"
#include "xprintf.h"

void vTestTask1();
void vTestTask2();
void vTestTask3();
void vTestTask4();
void FreeRTOS_Setup();

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    FreeRTOS_Setup();
    vUSBUARTStart();
    USBUART_Setup();
    //xTaskCreate(vTestTask1,"test1",1000,NULL,3,NULL);//Transfer test task A
    //xTaskCreate(vTestTask2,"test2",1000,NULL,3,NULL);//Transfer test task B
    //xTaskCreate(vTestTask3,"test3",1000,NULL,3,NULL);//Receive test task
    xTaskCreate(vTestTask4,"test4",1000,NULL,3,NULL);//Echo back test task
    
    
    xTaskCreate(vUSBUARTTxTask,"UART_TX",100,NULL,3,NULL);
    xTaskCreate(vUSBUARTRxTask,"UART_RX",100,NULL,3,NULL);
    
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
    TickType_t tick = xTaskGetTickCount();
    char buf[128];
    
    for(;;){
        LED_1_Write(1);  
        sprintf(buf,"A %07d Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello\r\n",(int)xTaskGetTickCount());   
        vUSBUARTPutString(buf,strlen(buf));
        vTaskDelayUntil(&tick,10000);
    }    
}

void vTestTask2(){
    vTaskDelay(5000);  
    TickType_t tick = xTaskGetTickCount();
    char buf[128];       
    for(;;){      
        LED_1_Write(0);  
        sprintf(buf,"B %07d World World World World World World World World World World World World World World\r\n",(int)xTaskGetTickCount());
        vUSBUARTPutString(buf,strlen(buf));
        vTaskDelayUntil(&tick,10000);
    }    
}

void vTestTask3(){
    uint count = 0;
    //TickType_t tick = xTaskGetTickCount();
    char buf[128] = "0";    
    
    for(;;){      
        vUSBUARTGetChar(&buf[count]);
        if(buf[0] == 'b'){
            sprintf(buf,"B %07d World World World\r\n",(int)xTaskGetTickCount());
            vUSBUARTPutString(buf,strlen(buf));
            LED_1_Write(0);  
        }
        else if(buf[0] == 'a'){
            sprintf(buf,"A %07d Hello Hello Hello\r\n",(int)xTaskGetTickCount());
            vUSBUARTPutString(buf,strlen(buf));
            LED_1_Write(1);  
        }
            
        //vTaskDelayUntil(&tick,10);
    }    
}

void vTestTask4(){
    char buf[128] = "0";    
    
    for(;;){      
        vUSBUARTGetString(buf,1);
        vUSBUARTPutString(buf,1);
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
