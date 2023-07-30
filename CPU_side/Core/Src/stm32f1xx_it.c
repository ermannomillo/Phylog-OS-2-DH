/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */


/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
	__asm volatile("TST LR, 4"); // check LR to know which stack is used
	__asm volatile("ITE EQ"); // 2 next instructions are conditional
	__asm volatile("MRSEQ R0, MSP"); // save MSP if bit 2 is 0
	__asm volatile("MRSNE R0, PSP"); // save PSP if bit 2 is 1
	__asm volatile("B SVC_Handler_main"); // pass R0 as the argument
}

void saveKContext(uint32_t* SP) {
	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 8)) = (int32_t) SP[0];
	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 1)) = (1 << 24); // T=1

	__ASM volatile ("MSR MSP, %0" : : "r" (tasks[currentTaskIdx].ksp) : );
	__ASM volatile ("POP {R4-R11}");
	__ASM volatile ("MOV LR, #0xFFFFFFF9");
	__ASM volatile ("MOV R0, #0x0");
	__ASM volatile ("MSR CONTROL, R0");
	__ASM volatile ("ISB");
	__ASM volatile ("BX LR");
}

void SVC_Handler_main(uint32_t* SP) {
  // get the address of the instruction saved in PC
  uint8_t *pInstruction = (uint8_t*)(SP[6]);

  // go back 2 bytes (16-bit opcode)
  pInstruction -= 2;

  // get the opcode, in little endian
  uint8_t svc_num = *pInstruction;

  switch(svc_num) {
    case 0xFF:
      // trigger PendSV
      ICSR |= (1 << 28);
      break;

    case _mutexCloseConn:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexCloseConn;
    	saveKContext(SP);
	    break;

    case _exitK					:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kExit;
    	saveKContext(SP);
    		    break;
    case _mutexOpenConn			:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexOpenConn;
    	saveKContext(SP);
    		    break;

    case _mutexRAMAddByte		:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexRAMAddByte;
    	saveKContext(SP);
    		    break;

    case _mutexRAMGetByte		:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexRAMGetByte;
    	saveKContext(SP);
    		    break;

    case _mutexRAMGarbFrame		:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexRAMGarbFrame;
    	saveKContext(SP);
    		    break;

    case _mutexRAMAddFrame		:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexRAMAddFrame;
    	saveKContext(SP);
    		    break;

    case _mutexLEDLight			:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexLEDLight;
    	saveKContext(SP);
    		    break;

    case _mutexUARTAddByte		:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexUARTAddByte;
    	saveKContext(SP);
    		    break;

    case _mutexUARTGetByte		:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kMutexRAMGetByte;
    	saveKContext(SP);
    		    break;

    case _taskSuspend			:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kTaskSuspend;
    	saveKContext(SP);
    		    break;

    case _taskWait				:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kTaskWait;
    	saveKContext(SP);
    		    break;

    case _taskReady				:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kTaskReady	;
    	saveKContext(SP);
    		    break;

    case _taskKill				:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kTaskKill;
    	saveKContext(SP);
    		    break;

    case _taskIncreasePriority	:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kTaskIncreasePriority;
    	saveKContext(SP);
    		    break;

    case _taskIncreaseExehit	:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kTaskIncreaseExehit;
    	saveKContext(SP);
    		    break;

    case _rawComm				:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kRawComm;
    	saveKContext(SP);
    		    break;

    case _blankComm 			:
    	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kBlankComm;
    	saveKContext(SP);
    		    break;
    case _semPost				:
        	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kRawComm;
        	saveKContext(SP);
        		    break;

    case _semWait			:
        	*(tasks[currentTaskIdx].ksp + (TASK_STACK_SIZE - 2)) = (int32_t) kBlankComm;
        	saveKContext(SP);
        		    break;


    default:
      break;
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
	// save LR back to main, must do this firstly
	   __asm volatile("PUSH {LR}");

	  /* Save the context of current task */

	  // get current PSP
	  __asm volatile("MRS R0, PSP");
	  // save R4 to R11 to PSP Frame Stack
	  __asm volatile("STMDB R0!, {R4-R11}"); // R0 is updated after decrement
	  // save current value of PSP
	  __asm volatile("BL saveCurrentPSP"); // R0 is first argument

	  /* Do scheduling */

	  // select next task
	  __asm volatile("BL selectNextTask");

	  /* Retrieve the context of next task */

	  // get its past PSP value
	  __asm volatile("BL getCurrentPSP"); // return PSP is in R0
	  // retrieve R4-R11 from PSP Fram Stack
	  __asm volatile("LDMIA R0!, {R4-R11}"); // R0 is updated after increment
	  // update PSP
	  __asm volatile("MSR PSP, R0");

	  // exit
	  __asm volatile("POP {LR}");
	  __asm volatile("BX LR");
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  sysTicks++;
    schedule(1); // privileged
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(B1_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
