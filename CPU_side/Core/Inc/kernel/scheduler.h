
#ifndef SCHED_H__
#define SCHED_H__

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "core_cm3.h"


/* RAM */
#define RAM_START         (0x20000000u)
#define RAM_SIZE          (20 * 1024) // 20 KB

/* Stacks */
#define MAIN_STACK        (RAM_START + RAM_SIZE)
#define TASK_NUMBER_MAX   (16)
#define TASK_STACK_SIZE   (1024u)

/* Debug Exception and Monitor Control Register base address */
#define DEMCR                 *((volatile uint32_t*) 0xE000EDFCu)
/* ITM register addresses */
#define ITM_STIMULUS_PORT0    *((volatile uint32_t*) 0xE0000000u)
#define ITM_TRACE_EN          *((volatile uint32_t*) 0xE0000E00u)
/* System Handler control and state register */
#define SHCSR                 *((volatile uint32_t*) 0xE000ED24u)
/* SysTick register address */
#define SCSR                  *((volatile uint32_t*) 0xE000E010u)
#define SRVR                  *((volatile uint32_t*) 0xE000E014u)
/* Interrupt Control and State Register */
#define ICSR                  *((volatile uint32_t*) 0xE000ED04u)

typedef struct {
  void (*handler)(void);	// function pointer
  uint32_t * psp;				// Process Stack Pointer
  uint32_t * ksp; 			//ksp saved from the last interrupted kernel thread
  uint32_t  kernel_flag; // 0=kernel, 1=user
  uint32_t waitToTick;		// Wait until tick
} TCB_t;

extern SPI_HandleTypeDef spi_handler;
extern uint32_t sysTicks;
extern uint32_t speculatedNextTask;
extern uint32_t currentTaskIdx;
extern  TCB_t tasks[TASK_NUMBER_MAX];

extern TCB_t * run_ptr;



uint32_t getCurrentPSP();
void saveCurrentPSP(uint32_t psp);
uint32_t getCurrentKSP();
void saveCurrentKSP(uint32_t ksp);
void selectNextTask() ;
void initKernel(SPI_HandleTypeDef hspi);
void idleMain(void);
void addTask(void (*handler));
void startScheduler();
void schedule(uint32_t priv);
void delayTask(uint32_t ticks);

#endif
