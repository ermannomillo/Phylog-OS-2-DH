
#include <kernel/syscall.h>


void  kExit() {
	// save kernel context
	__ASM volatile ("MRS     R12, MSP");
	__ASM volatile ("STMDB   R12!, {R4-R11}");  //push R4-R11
	__ASM volatile ("MSR     MSP, R12");
	__ASM volatile ("LDR     R0,=run_ptr");
	__ASM volatile ("LDR     R1, [R0]");
	__ASM volatile ("LDR     R2, [R1,#4]");
	__ASM volatile ("STR     R12, [R2]  ");
	// load user context
	__ASM volatile ("LDR     R2, [R1]");
	__ASM volatile ("LDMIA   R2!, {R4-R11}");
	__ASM volatile ("MOV     LR, #0xFFFFFFFD");
	__ASM volatile ("MSR     PSP, R2");
	__ASM volatile ("MOV     R0, #0x03");
	__ASM volatile ("MSR     CONTROL, R0");
	__ASM volatile ("ISB");
	__ASM volatile ("BX      LR ");
}

void kMutexCloseConn(uint8_t mutexId, uint8_t * inBuffer )
{
	__disable_irq();
	// Send to FPGA command: 1111 + Mutex ID + Release Sequence
	uint8_t spiOutBuffer[2];

	spiOutBuffer[0] = 0xF0 +  mutexId;
	spiOutBuffer[1] = 0xFF;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexOpenConn(uint8_t mutexId, uint8_t * inBuffer)
{
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = 0xF0 + mutexId;
	spiOutBuffer[1] = 0xFF;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);
	if(*inBuffer <= 15 && *inBuffer > 0) {
	    speculatedNextTask =*inBuffer;
	}
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexRAMAddByte(uint8_t mutexId, uint8_t outByte, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = 0x10 + mutexId;
	spiOutBuffer[1] = outByte;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kMutexRAMGetByte(uint8_t mutexId, uint8_t index, uint8_t * inBuffer)
{
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = 0x00 + mutexId;
	spiOutBuffer[1] = index;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexRAMGarbFrame(uint8_t mutexId, uint8_t * inBuffer)
{
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = 0x20 + mutexId;
	spiOutBuffer[1] = 0x00;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexRAMAddFrame(uint8_t mutexId, uint8_t * inBuffer)
{
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = 0x30 + mutexId;
	spiOutBuffer[1] = 0x00;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexLEDLight(uint8_t mutexId, uint8_t outByte, uint8_t flags, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = mutexId + (((uint16_t)flags ) << 4);
	spiOutBuffer[1] = outByte;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kMutexUARTAddByte(uint8_t mutexId, uint8_t outByte, uint8_t flags, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = mutexId +  (((uint16_t)flags )<< 4);
	spiOutBuffer[1] = outByte;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kMutexUARTGetByte(uint8_t mutexId, uint8_t flags, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = mutexId + (((uint16_t) flags) << 4);
	spiOutBuffer[1] = 0x00;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskSuspend(uint8_t taskId, uint8_t flags, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = taskId + (((uint16_t) flags) << 4);
	spiOutBuffer[1] = ( 2 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskWait(uint8_t taskId, uint8_t flags,  uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = taskId + (((uint16_t) flags) << 4);
	spiOutBuffer[1] = ( 3 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskReady(uint8_t taskId, uint8_t flags, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = taskId + (((uint16_t) flags) << 4);
	spiOutBuffer[1] = ( 1 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskKill(uint8_t taskId, uint8_t flags, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = taskId + (((uint16_t) flags) << 4);
	spiOutBuffer[1] = ( 4 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskIncreasePriority(uint8_t taskId, uint8_t flags, uint8_t priority, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[1] = taskId + (((uint16_t) flags) << 4);
	spiOutBuffer[0] = ( 5 << 4) + priority;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskIncreaseExehit(uint8_t taskId, uint8_t flags, uint8_t exehit, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[1] = taskId + (((uint16_t) flags) << 4);
	spiOutBuffer[0] = ( 6 << 4) + exehit;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kRawComm(uint8_t moduleId, uint8_t flags, uint8_t payload, uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = moduleId + (((uint16_t) flags) << 4);
	spiOutBuffer[1] = payload;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kBlankComm ( uint8_t * inBuffer) {
	__disable_irq();

	uint8_t spiOutBuffer[2];
	spiOutBuffer[0] = 0x00;
	spiOutBuffer[1] = 0x00;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t*) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0 ) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

