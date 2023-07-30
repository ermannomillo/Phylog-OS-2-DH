
#include <kernel/f_syscall.h>


void  kExit(void) {
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

void kMutexCloseConn(uint8_t mutexId, uint16_t * inBuffer )
{
	__disable_irq();
	// Send to FPGA command: 1111 + Mutex ID + Release Sequence
	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) mutexId) << 8) + 0xF0FF;
	HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *) &spiOutBuffer, inBuffer, 1, 100);
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexOpenConn(uint8_t mutexId, uint16_t * inBuffer)
{
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer =  (((uint16_t)  mutexId) << 8) + 0xF000;
	HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *) &spiOutBuffer, inBuffer, 1, 100);
	if(*inBuffer <= 15 && *inBuffer > 0) {
	    speculatedNextTask =*inBuffer;
	}
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexRAMAddByte(uint8_t mutexId, uint8_t outByte, uint16_t * inBuffer)
{
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = 0x1000 + (((uint16_t) mutexId) << 8) + outByte;
	HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *) &spiOutBuffer, inBuffer, 1, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kMutexRAMGetByte(uint8_t mutexId, uint8_t index, uint16_t * inBuffer)
{
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = 0x00 + (((uint16_t) mutexId) << 8) + index;
	HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *) &spiOutBuffer, inBuffer, 1, 100);
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexRAMGarbFrame(uint8_t mutexId, uint16_t * inBuffer)
{
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = 0x2000 + (((uint16_t) mutexId) << 8);
	HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *) &spiOutBuffer, inBuffer, 1, 100);
	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexRAMAddFrame(uint8_t mutexId, uint16_t * inBuffer)
{
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = 0x3000 + (((uint16_t) mutexId) << 8);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	__enable_irq();
	kExit(); // Exit kernel cooperatively
}

void kMutexLEDLight(uint8_t mutexId, uint8_t outByte, uint8_t flags, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) mutexId) << 8) + (((uint16_t)flags ) << 12) + outByte;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kMutexUARTAddByte(uint8_t mutexId, uint8_t outByte, uint8_t flags, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) mutexId) << 8) +  (((uint16_t)flags ) << 12) + outByte;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kMutexUARTGetByte(uint8_t mutexId, uint8_t flags, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) mutexId) << 8) + (((uint16_t) flags) << 12);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskSuspend(uint8_t taskId, uint8_t flags, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) taskId) << 8) + (((uint16_t) flags) << 12) + ( 2 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		    speculatedNextTask = *inBuffer;
		}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskWait(uint8_t taskId, uint8_t flags,  uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) taskId) << 8) + (((uint16_t) flags) << 12) + ( 3 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		    speculatedNextTask = *inBuffer;
		}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskReady(uint8_t taskId, uint8_t flags, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) taskId) << 8) + (((uint16_t) flags) << 12) + ( 1 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
		speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskKill(uint8_t taskId, uint8_t flags, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) taskId) << 8) + (((uint16_t) flags) << 12) + ( 4 << 4);
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
			speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskIncreasePriority(uint8_t taskId, uint8_t flags, uint8_t priority, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) taskId) << 8) + (((uint16_t) flags) << 12) + ( 5 << 4) + priority;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
			speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kTaskIncreaseExehit(uint8_t taskId, uint8_t flags, uint8_t exehit, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) taskId) << 8) + (((uint16_t) flags) << 12) + ( 6 << 4) + exehit;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0){
			speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kRawComm(uint8_t moduleId, uint8_t flags, uint8_t payload, uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = (((uint16_t) moduleId) << 8) + (((uint16_t) flags) << 12) +  payload;
	HAL_SPI_TransmitReceive(&spi_handler, (uint8_t *) &spiOutBuffer, inBuffer, 1, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
			speculatedNextTask = *inBuffer;
	}

	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

void kBlankComm ( uint16_t * inBuffer) {
	__disable_irq();

	uint16_t spiOutBuffer;
	spiOutBuffer = 0x0000;

	HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *) &spiOutBuffer, inBuffer, 2, 100);

	if(*inBuffer <= 15 && *inBuffer > 0) {
			speculatedNextTask = *inBuffer;
	}
	__enable_irq();

	kExit(); // Exit kernel cooperatively
}

Queue * Queue_init(int N){
    Queue * q = malloc (sizeof(Queue) );
    q->size = N;
    q->buffer = malloc( N * sizeof(void*) );
    q->empty =  N;
    q->full=  0;
    q->extract_pos = 0;
    q->insert_pos = 0;
    return q;
}

void Queue_destroy(Queue * q){
    free ( (*q).buffer);
    free(q);
}

void Queue_insert(Queue * q, void * item){
	void * args[2];

	uint16_t semId = EMPTY_SEM_ID ;
	uint16_t moduleId = TASK0_ID;

	args[0] = &semId;
	args[1] = &moduleId;

    kCall(_semWait, args);
    q->buffer[q->insert_pos] = item;
    q->insert_pos =  (q->insert_pos + 1) % q->size;
    semId = FULL_SEM_ID ;
    kCall(_semPost, args);
}

void * Queue_extract(Queue * q){
	void * item;
	void * args[2];

	uint16_t semId = FULL_SEM_ID ;
	uint16_t moduleId = TASK0_ID;

	args[0] = &semId;
	args[1] = &moduleId;

	kCall(_semWait, args);

    item = &(q->buffer[q->extract_pos]);
    q->extract_pos =  (q->extract_pos + 1) % q->size;
    semId = EMPTY_SEM_ID ;
    kCall(_semPost, args);
    return item;
}

void kSemWait( uint16_t semId, uint16_t nodeId ) {
	 uint16_t inBuffer;
	 uint16_t outBuffer;
	 do {
		 __disable_irq();
		 outBuffer = 0xC000 + (semId << 8) + (nodeId << 12);
		 HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *)&outBuffer, (uint16_t *) &inBuffer, 1, 100);
		 outBuffer = 0x0000 + (semId << 8) + 0x0020 + (nodeId << 12);
		 HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *)&outBuffer, (uint16_t *) &inBuffer, 1, 100);
		 __enable_irq();
	 }while(inBuffer !=  0x0000 + nodeId  && inBuffer != 0x2000 + nodeId);
	 outBuffer = 0xC0FF + (semId << 8) + (nodeId << 12);
	 HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *)&outBuffer, (uint16_t *) &inBuffer, 1, 100);

	 kExit(); // Exit kernel cooperatively

}

void kSemPost( uint8_t semId, uint16_t nodeId ) {
	 uint16_t inBuffer;
	 uint16_t outBuffer;
	 do {
		 __disable_irq();
		 outBuffer = 0xC000 + (semId << 8) + (nodeId << 12);
		 HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *)&outBuffer, (uint16_t *) &inBuffer, 1, 100);
		 outBuffer =  0x0000 + (semId << 8) + 0x0020  + (nodeId << 12);
		 HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *)&outBuffer, (uint16_t *) &inBuffer, 1, 100);
		 __enable_irq();
	 }while(inBuffer !=  0x0000 + nodeId  && inBuffer != 0x1000 + nodeId);
	 outBuffer = 0xC0FF + (semId << 8) + (nodeId << 12);
	 HAL_SPI_TransmitReceive(&spi_handler, (uint16_t *)&outBuffer, (uint16_t *) &inBuffer, 1, 100);

	 kExit(); // Exit kernel cooperatively
}

