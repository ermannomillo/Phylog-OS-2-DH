
#ifndef __SYS_H
#define __SYS_H

#include "stm32f1xx_hal.h"
#include <kernel/scheduler.h>
#include <stdint.h>
#include <stdio.h>



#define TASK0_ID	   1U
#define TASK1_ID  	   2U
#define TASK2_ID       3U
#define TASK3_ID   	   4U
#define TASK4_ID 	   5U
#define TASK5_ID 	   6U
#define TASK6_ID 	   7U
#define TASK7_ID 	   8U
#define LED_MUTEX_ID  10U
#define UART_MUTEX_ID 11U
#define RAM_MUTEX_ID  12U
#define ESPIC_ID 	  15U

#define kCall(code, args) { \
		asm volatile ("MOV %%r0, %0 " \
		: \
		: "r"  (args) \
		: "%r0");     \
		asm volatile ("svc %[immediate]" \
		: \
		:[immediate] "I" (code) \
		: );  	  \
}


void kExit();
void kMutexCloseConn(uint8_t mutexId, uint8_t * inBuffer);
void kMutexOpenConn(uint8_t mutexId, uint8_t * inBuffer);
void kMutexRAMAddByte(uint8_t mutexId, uint8_t outByte, uint8_t * inBuffer);
void kMutexRAMGetByte(uint8_t mutexId, uint8_t index, uint8_t * inBuffer);
void kMutexRAMGarbFrame(uint8_t mutexId, uint8_t * inBuffer);
void kMutexRAMAddFrame(uint8_t mutexId, uint8_t * inBuffer);
void kMutexLEDLight(uint8_t mutexId, uint8_t outByte, uint8_t flags, uint8_t * inBuffer);
void kMutexUARTAddByte(uint8_t mutexId, uint8_t outByte, uint8_t flags, uint8_t * inBuffer);
void kTaskSuspend(uint8_t taskId, uint8_t flags, uint8_t * inBuffer);
void kTaskWait(uint8_t taskId, uint8_t flags, uint8_t * inBuffer);
void kTaskReady(uint8_t taskId, uint8_t flags, uint8_t * inBuffer);
void kTaskKill(uint8_t taskId, uint8_t flags, uint8_t * inBuffer);
void kTaskIncreasePriority(uint8_t taskId, uint8_t priority, uint8_t flags, uint8_t * inBuffer);
void kTaskIncreaseExehit(uint8_t taskId, uint8_t exehit, uint8_t flags, uint8_t * inBuffer);
void kRawComm(uint8_t moduleId, uint8_t flags,uint8_t payload, uint8_t * inBuffer);
void kBlankComm ( uint8_t * inBuffer);


#endif
