#ifndef PB12_OS_H
#define PB12_OS_H

#include "pb12_pcb.h"
#include "pb12_alloc.h"
#include "pb12_semaphore.h"
#include "pb12_stats.h"

struct S_PB12_HW;

typedef struct S_PB12_OS {
    struct S_PB12_HW *hw;       /* Pointer to hardware for convenience */

    unsigned int tick_count;

    PB12_MemList free_list;     /* Free memory */

    PB12_PCB_List new_q;
    PB12_PCB_List ready_q;

    int next_pid;

    PB12_ProcStat stats[50];   /* This is a quick hack for project 4 */

    /* THE FOLLOWING IS ONLY USED FOR PROJECT 3 */
    /* TODO: REMOVE AFTER PROJECT 3 */
    PB12_Semaphore forks[5];
    PB12_Semaphore doorman;
} PB12_OS;


/**
    Initialize Operating System.

    @param PB12_OS *os - Operating System
    @param PB12_HW *hw - Virtual Machine's hardware.

    @return int - PB21_SUCCESS
*/
int pb12InitOs(PB12_OS *os, struct S_PB12_HW *hw);


/**
    Destroys operating system.

    @param PB12_OS *os - Operating System.

    @return int - PB12_SUCCESS
*/
int pb12DestroyOs(PB12_OS *os);


/**
    Starts up the OS.

    @param PB12_OS *os - Operating System.
*/
void pb12StartOs(PB12_OS *os);


/**
    Queueing a program will create a PCB for a process and add it to the
    new process queue.  When a memory location is available, it will load
    the program into memory and move the PCB to the ready queue.

    @param PB12_OS *os - Operating System.
    @param const char *filename - File name of program.

    @return PB12_SUCCESS if file exists, else PB12_FAILURE
*/
int pb12QueueProgram(PB12_OS *os, const char *filename);


/**
    Loads as many programs as possible from the new_q that can fit into
    memory then moves them over to the ready_q to start executing them.

    @param PB12_OS *os - Operating System

    @return Number of processes that have been readied.
*/
int pb12ReadyPrograms(PB12_OS *os);


/**
    Destroys a process.

    @param PB12_OS *os - Operating System.
    @param in pid - Process ID.

    @return int - PB12_SUCCESS or PB12_FAILURE
*/
int pb12DestroyProcess(PB12_OS *os, int pid);


/**
    Terminates the current process.

    @param PB12_OS *os - Operating System.
*/
void pb12TerminateProcess(PB12_OS *os);


/**
    Get ID of currently running process.

    @param PB12_OS *os - Operating System.

    @return int - Process ID.
*/
int pb12CurrentPid(PB12_OS *os);


/**
    Moves the PCB with the specified PID to the Ready Queue.

    @param PB12_OS *os - Operating System.
    @param PB12_PCB_List *source - List PCB will be moved from.
    @param int pid - Process ID
*/
void pb12MoveToReady(PB12_OS *os, PB12_PCB_List *source, int pid);


/**
    Moves the PCB with the specified PID from the Ready Queue.

    @param PB12_OS *os - Operating System.
    @param PB12_PCB_List *dest - List PCB will be moved to.
    @param int pid - Process ID
*/
void pb12MoveFromReady(PB12_OS *os, PB12_PCB_List *dest, int pid);


/**
    Preempt the currently running process.

    @param PB12_OS *os - Operating System.
*/
void pb12Preempt(PB12_OS *os);


/**
    See if there is a trap instruction to respond to.

    @param PB12_OS *os - Operating System.
*/
void pb12TrapInterrupt(PB12_OS *os);


/**
    Update operating system.

    @param PB12_OS *os - Operating System.
    @param int cpu_status - Value returned from CPU.

    @return int - PB12_SUCCESS if running and PB12_TERMINATE if no processes.
*/
int pb12OsTick(PB12_OS *os, int cpu_status);

#endif /* PB12_OS_H */
