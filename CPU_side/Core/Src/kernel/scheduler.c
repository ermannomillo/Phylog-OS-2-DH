
#include <kernel/scheduler.h>
#include "core_cm3.h"


SPI_HandleTypeDef spi_handler;
 uint32_t sysTicks = 0;
 uint32_t speculatedNextTask = 0;
 uint32_t currentTaskIdx = 0;
  TCB_t tasks[TASK_NUMBER_MAX] = {0};

TCB_t * run_ptr;

/* Scheduler */
uint32_t getCurrentPSP() {
  return *(tasks[currentTaskIdx].psp);
}

void saveCurrentPSP(uint32_t psp) {
  *(tasks[currentTaskIdx].psp) = psp;
}

void initKernel(SPI_HandleTypeDef hspi) {
	SPI_HandleTypeDef spi_handler = hspi;
}

uint32_t getCurrentKSP() {
  return *(tasks[currentTaskIdx].ksp);
}

void saveCurrentKSP(uint32_t ksp) {
  *(tasks[currentTaskIdx].ksp) = ksp;
}


void selectNextTask()
{
	uint16_t spiOutBuffer = 0;
	uint16_t spiInBuffer= 0;

	for(int i=0; i<TASK_NUMBER_MAX; i++) {
		if(sysTicks >= tasks[i].waitToTick) {
			uint16_t spiOutBuffer = (1 << 12) + (i << 8) + (1 << 4);  // Put the task in ready list, IFF it is blocked.
			HAL_SPI_TransmitReceive(&spi_handler, &spiOutBuffer, &spiInBuffer, 1, 100);

			if( spiInBuffer <= 15 && spiInBuffer > 0) {
				speculatedNextTask = spiInBuffer;
				 run_ptr = &tasks[currentTaskIdx];
			}
	    }
	}

	if(speculatedNextTask) {
		uint16_t spiOutBuffer = 0;

		do {
			HAL_SPI_TransmitReceive(&spi_handler, &spiOutBuffer, &spiInBuffer, 1, 100);
		} while((spiInBuffer & 0x0FFF) >= 0x00FF); // Loop until a task ID is received

		// It sends a request every circa 10 microseconds.
		currentTaskIdx = (spiInBuffer & 0x00FF);

		// Assemble and send execution confirm
		spiOutBuffer = (((uint32_t) spiInBuffer) << 8) + 0x00F0;
		HAL_SPI_TransmitReceive(&spi_handler, &spiOutBuffer, &spiInBuffer, 1, 100);
	} else {
		currentTaskIdx = speculatedNextTask;
	}

	if (currentTaskIdx >= TASK_NUMBER_MAX || tasks[currentTaskIdx].psp == 0) {
        currentTaskIdx = 0;
	}
}

void idleMain(void) {
  while(1) {
    __asm volatile("NOP");
  }
}

void addTask(void (*handler)) {

  // init idle if it is not found
  if (handler != idleMain) {
    if (tasks[0].handler != idleMain) {
      addTask(idleMain);
    }
  }

  // find an empty slot
  int i=0;
  for(; i<TASK_NUMBER_MAX; i++) {
    if (tasks[i].psp == 0) break;
  }


  if(i >= TASK_NUMBER_MAX) {
    printf("Can not register a new task anymore!\n");
    return;
  } else {
    printf("Register a task %p at slot %i\n", handler, i);
  }


  // calculate new PSP
  uint32_t* psp = (uint32_t*)(MAIN_STACK - (i+1)*TASK_STACK_SIZE);

  // fill dummy stack frame
  *(--psp) = 0x01000000u; // Dummy xPSR, just enable Thumb State bit;
  *(--psp) = (uint32_t) handler; // PC
  *(--psp) = 0xFFFFFFFDu; // LR with EXC_RETURN to return to Thread using PSP
  *(--psp) = 0x12121212u; // Dummy R12
  *(--psp) = 0x03030303u; // Dummy R3
  *(--psp) = 0x02020202u; // Dummy R2
  *(--psp) = 0x01010101u; // Dummy R1
  *(--psp) = 0x00000000u; // Dummy R0
  *(--psp) = 0x11111111u; // Dummy R11
  *(--psp) = 0x10101010u; // Dummy R10
  *(--psp) = 0x09090909u; // Dummy R9
  *(--psp) = 0x08080808u; // Dummy R8
  *(--psp) = 0x07070707u; // Dummy R7
  *(--psp) = 0x06060606u; // Dummy R6
  *(--psp) = 0x05050505u; // Dummy R5
  *(--psp) = 0x04040404u; // Dummy R4

  // save PSP
  tasks[i].psp = (uint32_t)psp;

  // calculate new KSP
   uint32_t* ksp = (uint32_t*)(MAIN_STACK - (i+1)*TASK_STACK_SIZE);

   // fill dummy stack frame
   *(--ksp) = 0x01000000u; // Dummy xPSR, just enable Thumb State bit;
   *(--ksp) = (uint32_t) handler; // PC
   *(--ksp) = 0x01000000u; // LR with EXC_RETURN to return to Thread using PSP
   *(--ksp) = 0x12121212u; // Dummy R12
   *(--ksp) = 0x03030303u; // Dummy R3
   *(--ksp) = 0x02020202u; // Dummy R2
   *(--ksp) = 0x01010101u; // Dummy R1
   *(--ksp) = 0x00000000u; // Dummy R0
   *(--ksp) = 0x11111111u; // Dummy R11
   *(--ksp) = 0x10101010u; // Dummy R10
   *(--ksp) = 0x09090909u; // Dummy R9
   *(--ksp) = 0x08080808u; // Dummy R8
   *(--ksp) = 0x07070707u; // Dummy R7
   *(--ksp) = 0x06060606u; // Dummy R6
   *(--ksp) = 0x05050505u; // Dummy R5
   *(--ksp) = 0x04040404u; // Dummy R4

   // save KSP
   tasks[i].ksp = (uint32_t)ksp;

   tasks[i].kernel_flag = 0;

  // initial state
  tasks[i].handler = handler;
}

void startScheduler()
{
  // start with the first task
  currentTaskIdx = 0;

  // prepare PSP of the first task
  __asm volatile("BL getCurrentPSP"); // return PSP in R0
  __asm volatile("MSR PSP, R0");  // set PSP

  // change to use PSP
  __asm volatile("MRS R0, CONTROL");
  __asm volatile("ORR R0, R0, #2"); // set bit[1] SPSEL
  __asm volatile("MSR CONTROL, R0");

  // start SysTick
  // clear and set the period
  SRVR &= ~0xFFFFFFFF;
  SRVR |= 16000-1; // 1000 Hz ~ 1 ms
  // enable SysTick
  SCSR |= (1 << 1); // enable SysTick Exception request
  SCSR |= (1 << 2); // select system clock
  SCSR |= (1 << 0); // start

  // Move to Unprivileged level
  __asm volatile("MRS R0, CONTROL");
  __asm volatile("ORR R0, R0, #1"); // Set bit[0] nPRIV
  __asm volatile("MSR CONTROL, R0");
  // right after here, access is limited

  // get the handler of the first task by tracing back from PSP which is at R4 slot
  void (*handler)() = tasks[currentTaskIdx].handler;

  // execute the handler
  handler();
}

void schedule(uint32_t priv)
{
  if(priv) {
    // trigger PendSV directly
    ICSR |= (1 << 28);
  } else {
    // call Supervisor exception to get Privileged access
    __asm volatile("SVC #255");
  }
}

void delayTask(uint32_t ticks) {
  if(currentTaskIdx) {
    tasks[currentTaskIdx].waitToTick = sysTicks + ticks;
    uint16_t spiOutBuffer = currentTaskIdx << 8 + (2 << 4);
    uint16_t  spiInBuffer = 0;
    HAL_SPI_TransmitReceive(&spi_handler, &spiOutBuffer, &spiInBuffer, 1, 100);

    if( spiInBuffer <= 15) {
    	speculatedNextTask = spiInBuffer;
    }
    schedule(0); // unprivileged
  }
}

