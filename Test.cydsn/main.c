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

#include "USBUART_FreeRTOS.h"

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
    USBUARTStart();

    //xTaskCreate(vTestTask1,"test1",1000,NULL,3,NULL);//Transfer test task A
    //xTaskCreate(vTestTask2,"test2",1000,NULL,3,NULL);//Transfer test task B
    //xTaskCreate(vTestTask3,"test3",1000,NULL,3,NULL);//Receive test task
    xTaskCreate(vTestTask4,"test4",1000,NULL,3,NULL);//Echo back test task
    
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
        USBUARTPutString(buf,strlen(buf));
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
        USBUARTPutString(buf,strlen(buf));
        vTaskDelayUntil(&tick,10000);
    }    
}

void vTestTask3(){
    uint count = 0;
    //TickType_t tick = xTaskGetTickCount();
    char buf[128] = "0";    
    
    for(;;){      
        USBUARTGetString(&buf[count],1);
        if(buf[0] == 'b'){
            sprintf(buf,"B %07d World World World\r\n",(int)xTaskGetTickCount());
            USBUARTPutString(buf,strlen(buf));
            LED_1_Write(0);  
        }
        else if(buf[0] == 'a'){
            sprintf(buf,"A %07d Hello Hello Hello\r\n",(int)xTaskGetTickCount());
            USBUARTPutString(buf,strlen(buf));
            LED_1_Write(1);  
        }
            
        //vTaskDelayUntil(&tick,10);
    }    
}

void vTestTask4(){
    char buf[128] = "0";    
    uint8_t temp;
    for(;;){     
        temp = USBUARTGetRxBufferCount();
        USBUARTGetString(buf,temp);
        USBUARTPutString(buf,temp);
    }    
}

#if configCHECK_FOR_STACK_OVERFLOW != 0
//オーバーフローが起こった際のハンドラ
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    /* The stack space has been execeeded for a task, considering allocating more. */
    taskDISABLE_INTERRUPTS();
}/*---------------------------------------------------------------------------*/
#endif

#if configUSE_MALLOC_FAILED_HOOK != 0
//メモリの確保に失敗したときのハンドラ
void vApplicationMallocFailedHook( void )
{
    /* The heap space has been execeeded. */
    taskDISABLE_INTERRUPTS();
}
#endif


/* [] END OF FILE */
