#include "main.h"
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include <stdbool.h>

#define STACK_SIZE 0x200
#define MAX_STACK_POOL 32
#define DEFAULT_TIMESLICE 5 // Default timeslice value in milliseconds



void SVC_Handler_Main( unsigned int *svc_args )
{
	extern void runFirstThread(void);
	unsigned int svc_number;
	/*
	* Stack contains:
	* r0, r1, r2, r3, r12, r14, the return address and xPSR
	* First argument (r0) is svc_args[0]
	*/
	svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
	switch( svc_number )
	{
		case 0: //17 is sort of arbitrarily chosen
			printf("Success!\r\n");
			break;
		case 1: //17 is sort of arbitrarily chosen
			printf("Success Again!\r\n");
			break;
		case 3:
			__set_PSP(stackptr);
			runFirstThread();
			break;
		case 4:
			//Pend an interrupt to do the context switch
			_ICSR |= 1<<28;
			__asm("isb");
			break;
		default: /* unknown SVC */
			break;
	}
}




void setup_thread_stack(void)
{
	stackptr = allocatedStack();

	if(stackptr == NULL)
	{
		printf("OUT OF STACK SPACE!");
		return;
	}

	*(--stackptr) = 1<<24; //A magic number, this is xPSR
	*(--stackptr) = (uint32_t)print_continuously; //the function name
	for (int i = 0; i < 14; i++) {
		*(--stackptr) = 0xA;
	}
	stack_call();

}

uint32_t* allocatedStack(void)
{
    oldstackptr -= STACK_SIZE;
    stackNum++;

    if (stackNum >= MAX_STACK_POOL)
    {
        return NULL;
    }
    return oldstackptr;
}


thread arrayThread[MAX_STACK_POOL];

int current_thread_index = -1; //index of the current running thread

bool osCreateThread(void (*thread_function)(void*), void*arguments) {
    // Check if the maximum number of threads has been reached
    if (current_thread_index >= MAX_STACK_POOL - 1) {
        return false; // Unable to create a new thread
    }

    // Allocate a new stack for the thread
    stackptr = allocatedStack();
    if (stackptr == NULL) {
        return false; // Unable to allocate stack space
    }

    // Set up the stack
    *(--stackptr) = 1 << 24; // xPSR
    *(--stackptr) = (uint32_t)thread_function;// Thread function address in r0
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = (uint32_t)arguments; //This is R0
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;


//    for (int i = 0; i < 14; i++) {
//        *(--stackptr) = 0xA;
//    }
	// Set the thread's timeslice and runtime to the default value
	arrayThread[current_thread_index].timeslice = DEFAULT_TIMESLICE;
	arrayThread[current_thread_index].runtime = DEFAULT_TIMESLICE;
	arrayThread[current_thread_index].deadlines = 0;
	arrayThread[current_thread_index].timeline = 0;


    current_thread_index++; // Increment the index of the currently running thread

    arrayThread[current_thread_index].thread_function = thread_function;
    arrayThread[current_thread_index].sp = stackptr;

    return true; // Thread created successfully
}

bool osThreadCreateWithDeadline(void (*thread_function)(void*), void* arguments, int deadline) {
    // Check if the maximum number of threads has been reached
    if (current_thread_index >= MAX_STACK_POOL - 1) {
        return false; // Unable to create a new thread
    }

    // Allocate a new stack for the thread
    stackptr = allocatedStack();
    if (stackptr == NULL) {
        return false; // Unable to allocate stack space
    }

    // Set up the stack
//    *(--stackptr) = 1 << 24; // xPSR
//    *(--stackptr) = (uint32_t)thread_function; // Thread function address in r0
//    *(--stackptr) = (uint32_t)arguments; // Pass the address of interesting_value
//
//    for (int i = 0; i < 14; i++) {
//        *(--stackptr) = 0xA;
//    }
    *(--stackptr) = 1 << 24; // xPSR
    *(--stackptr) = (uint32_t)thread_function;// Thread function address in r0
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = arguments; //This is R0
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;
	*(--stackptr) = 0xA;


    current_thread_index++; // Increment the index of the currently running threa

    arrayThread[current_thread_index].thread_function = thread_function;
    arrayThread[current_thread_index].sp = stackptr;
    // Set the thread's timeslice and runtime to the specified deadline
    arrayThread[current_thread_index].timeslice = deadline;
    arrayThread[current_thread_index].runtime = deadline;
    arrayThread[current_thread_index].deadlines = deadline;
    arrayThread[current_thread_index].timeline = deadline;


    return true; // Thread created successfully
}


void osKernelInitialize() {
    // Initialize global variables and any other setup tasks
    current_thread_index = -1;// Set the index of the currently running thread to 0
    stackNum = 1;
    stackptr = *(uint32_t**)0x0;
    //set the priority of PendSV to almost the weakest
    SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
    SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV
}

void osKernelStart(){
	__asm("SVC #3"); //SVC RUN_FIRST_THREAD
}

void print_success(void)
{
	__asm("SVC #0");

}

//void osSched(){
//	arrayThread[current_thread_index].sp = (uint32_t*)(__get_PSP() - 8*4);
//	current_thread_index = (++current_thread_index)%(stackNum -1);
//	__set_PSP(arrayThread[current_thread_index].sp);
//}


void osYield(void){
	__asm("SVC #4");
}



void osSched() {
    // Save the current thread's stack pointer
    arrayThread[current_thread_index].sp = (uint32_t*)(__get_PSP() - 8 * 4);

    // Decrement the remaining runtime of the current thread
    arrayThread[current_thread_index].runtime -= DEFAULT_TIMESLICE;

    uint32_t earliest_deadline_thread_index = current_thread_index; // Initialize with the current thread
    uint32_t earliest_deadline = arrayThread[current_thread_index].deadlines;

    for (uint32_t i = 0; i <= stackNum - 2; i++) {
        // Check if the thread has remaining runtime
        if (arrayThread[i].runtime > 0) {
            // Find the thread with the earliest deadline and remaining runtime
            if (arrayThread[i].deadlines < earliest_deadline) {
                earliest_deadline_thread_index = i;
                earliest_deadline = arrayThread[i].deadlines;
            }
        }
    }

    // Check if the current thread's remaining runtime is zero and its deadline is earlier than the earliest deadline
    if (arrayThread[current_thread_index].runtime <= 0 && arrayThread[current_thread_index].deadlines <= earliest_deadline) {
        // Continue running the current thread
        arrayThread[current_thread_index].deadlines += arrayThread[current_thread_index].timeline;
    } else {
        // Switch to the thread with the earliest deadline and update its deadline
        if (current_thread_index != earliest_deadline_thread_index) {
            // Save the context of the current thread
            arrayThread[current_thread_index].sp = (uint32_t*)(__get_PSP() - 8 * 4);
        }
        current_thread_index = earliest_deadline_thread_index;
        arrayThread[current_thread_index].deadlines += arrayThread[current_thread_index].timeline;

        // Restore the context of the selected thread
        __set_PSP(arrayThread[current_thread_index].sp);
    }

    // Check if the current thread's remaining runtime is zero, then yield the processor
//    if (arrayThread[current_thread_index].runtime <= 0) {
//        osYield();
//    }
}



void print_success_again(void)
{
	__asm("SVC #1");
}

void stack_call(void)
{
	__asm("SVC #3");
}






