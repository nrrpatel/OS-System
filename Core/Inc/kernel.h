
#include <stdio.h>
#include <stdbool.h>

#define STACK_SIZE 0x200
#define MAX_STACK_POOL 32
#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV

extern uint32_t* stackptr;
extern uint32_t* oldstackptr;
extern int stackNum;
extern int interesting_value;

extern void runFirstThread(void);
extern void print_continuously();
extern void print_again();

uint32_t* allocatedStack(void);

void updateThreadRuntimes(uint32_t elapsed_time);



void setup_thread_stack(void);

bool osCreateThread(void (*thread_function)(void*), void* arguments);
bool osThreadCreateWithDeadline(void (*thread_function)(void*), void* arguments, int deadline);

void SVC_Handler_Main(unsigned int* svc_args);
void print_success(void);
void print_success_again(void);
void stack_call(void);

void osKernelInitialize();
void osKernelStart();
void osSched();
void osYield();
void increment_time();

//void setup_thread_stack(void);

typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
	uint32_t timeslice; // timeslice for the thread in milliseconds
	uint32_t runtime; // runtime remaining for the thread in milliseconds
	uint32_t deadlines;
	uint32_t timeline;
}thread;

typedef struct {
    int value1;
    int value2;
} ThreadArguments;

extern thread arrayThread[MAX_STACK_POOL];
extern int current_thread_index;


