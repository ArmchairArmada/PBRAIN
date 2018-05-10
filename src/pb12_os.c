#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pb12.h"
#include "pb12_os.h"
#include "pb12_traps.h"
#include "pb12_hw.h"
#include "pb12_pcb.h"
#include "pb12_semaphore.h"
#include "pb12_strings.h"
#include "pb12_alloc.h"

/**
    Initialize Operating System.

    @param PB12_OS *os - Operating System
    @param PB12_HW *hw - Virtual Machine's hardware.

    @return int - PB21_SUCCESS
*/
int pb12InitOs(PB12_OS *os, PB12_HW *hw) {
    os->hw = hw;

    os->tick_count = 0;

    pb12AllocInit(&os->free_list, PB12_MEM_SIZE);

    pb12InitPcbList(&os->new_q);
    pb12InitPcbList(&os->ready_q);

    os->next_pid = 0;

    /* TODO: REMOVE AFTER PROJECT 3 */
    pb12SemInit(&os->forks[0], 1);
    pb12SemInit(&os->forks[1], 1);
    pb12SemInit(&os->forks[2], 1);
    pb12SemInit(&os->forks[3], 1);
    pb12SemInit(&os->forks[4], 1);

    pb12SemInit(&os->doorman, 4);

    return PB12_SUCCESS;
}


/**
    Destroys operating system.

    @param PB12_OS *os - Operating System.

    @return int - PB12_SUCCESS
*/
int pb12DestroyOs(PB12_OS *os) {
    pb12FreePcbList(&os->new_q);
    pb12FreePcbList(&os->ready_q);

    os->hw = NULL;

    return PB12_SUCCESS;
}


/**
    Starts up the OS.

    @param PB12_OS *os - Operating System.
*/
void pb12StartOs(PB12_OS *os) {
    pb12ReadyPrograms(os);
}


/**
    Queueing a program will create a PCB for a process and add it to the
    new process queue.  When a memory location is available, it will load
    the program into memory and move the PCB to the ready queue.

    @param PB12_OS *os - Operating System.
    @param const char *filename - File name of program.

    @return PB12_SUCCESS if file exists, else PB12_FAILURE
*/
int pb12QueueProgram(PB12_OS *os, const char *filename) {
    int pid;
    int mem_req;
    PB12_PCB *pcb;
    FILE *fp;
    char buffer[256];

    fp = fopen(filename, "rt");
    if (fp == NULL) {
        return PB12_FAILURE;
    }

    fgets(buffer, 256, fp);
    fclose(fp);
    fp = NULL;
    sscanf(buffer, "%d", &mem_req);

    pid = os->next_pid;
    ++os->next_pid;

    pcb = (PB12_PCB*) malloc(sizeof(PB12_PCB));
    pb12InitPcb(pcb, pid, filename, mem_req);

    pb12PushBackPcb(&os->new_q, pcb);

    return PB12_SUCCESS;
}


/**
    Loads as many programs as possible from the new_q that can fit into
    memory then moves them over to the ready_q to start executing them.

    @param PB12_OS *os - Operating System

    @return Number of processes that have been readied.
*/
int pb12ReadyPrograms(PB12_OS *os) {
    PB12_MemBlock *mem_block;
    PB12_PCB *pcb;
    int readied = 0;

    while (!pb12IsEmptyPcb(&os->new_q)) {
        pcb = os->new_q.head;

        /* TODO: Memory allocation, loading, and moving to ready_q */
        if (pb12Options & PB12_OPT_FIRST_FIT) {
            mem_block = pb12AllocFirstFit(&os->free_list, pcb->mem_req);
        }
        else if (pb12Options & PB12_OPT_WORST_FIT) {
            mem_block = pb12AllocWorstFit(&os->free_list, pcb->mem_req);
        }
        else {
            mem_block = pb12AllocBestFit(&os->free_list, pcb->mem_req);
        }

        if (mem_block) {
            pb12SetPcbMem(pcb, mem_block);
            pb12Load(&os->hw->mem, mem_block->address, pcb->program);
            pcb->wait_time = os->tick_count;

            os->stats[pcb->pid].start_time = os->tick_count;

            pb12MoveToReady(os, &os->new_q, pcb->pid);
            ++readied;
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Readied %s (%d) at %d, length %d, wait time %d.\n",
                       pcb->program, pcb->pid, mem_block->address,
                       mem_block->length, pcb->wait_time);
            }
        }
        else {
            return readied;
        }
    }

    return readied;
}


/**
    Destroys a process.

    @param PB12_OS *os - Operating System.
    @param in pid - Process ID.

    @return int - PB12_SUCCESS or PB12_FAILURE
*/
int pb12DestroyProcess(PB12_OS *os, int pid) {
    /* TODO: Fix destroying currently running process to switch to next process. */
    PB12_PCB *pcb;

    pcb = pb12RemovePcb(&os->new_q, pid);
    if (pcb != NULL) {
        pb12DestroyPcb(pcb);
        return PB12_SUCCESS;
    }

    pcb = pb12RemovePcb(&os->ready_q, pid);
    if (pcb != NULL) {
        pb12DestroyPcb(pcb);
        return PB12_SUCCESS;
    }

    pb12ErrorMsg(pb12ErrorStr[PB12_ERORR_DESTROYING_PROCESS], pid);

    return PB12_FAILURE;
}


/**
    Terminates the current process.

    @param PB12_OS *os - Operating System.
*/
void pb12TerminateProcess(PB12_OS *os) {
    PB12_PCB *pcb;

    pcb = pb12DequeuePcb(&os->ready_q);

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Terminating process (%d) '%s'.\n", pcb->pid, pcb->program);
    }

    os->stats[pcb->pid].end_time = os->tick_count;

    pb12AllocMerge(&os->free_list, pcb->mem_block);
    pb12DestroyPcb(pcb);
    pb12ReadyPrograms(os);
    if (os->ready_q.head != NULL) {
        /*memcpy(&os->hw->cpu, &os->ready_q.head->cpu, sizeof(PB12_CPU));*/
        pb12CopyCPU(&os->ready_q.head->cpu, &os->hw->cpu);
        if (pb12Options & PB12_OPT_VERBOSE) {
            printf("Process (%d) ready to exec '%s' with time slice of %d inst.\n",
                   os->ready_q.head->pid, os->ready_q.head->program, os->hw->cpu.ic);
        }
    }
}


/**
    Get ID of currently running process.

    @param PB12_OS *os - Operating System.

    @return int - Process ID.
*/
int pb12CurrentPid(PB12_OS *os) {
    if (os->ready_q.head == NULL)
        return -1;
    else
        return os->ready_q.head->pid;
}


/**
    Moves the PCB with the specified PID to the Ready Queue.

    @param PB12_OS *os - Operating System.
    @param PB12_PCB_List *source - List PCB will be moved from.
    @param int pid - Process ID
*/
void pb12MoveToReady(PB12_OS *os, PB12_PCB_List *source, int pid) {
    PB12_PCB *pcb;

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Moving process with PID %d to Ready Queue.\n", pid);
    }

    pcb = pb12RemovePcb(source, pid);
    pb12PushBackPcb(&os->ready_q, pcb);

    if (os->ready_q.head == pcb) {
        pb12CopyCPU(&pcb->cpu, &os->hw->cpu);
        if (pb12Options & PB12_OPT_VERBOSE) {
            printf("Process (%d) ready to exec '%s' with time slice of %d inst.\n",
                   os->ready_q.head->pid, os->ready_q.head->program, os->hw->cpu.ic);
        }
    }
}


/**
    Moves the PCB with the specified PID from the Ready Queue.

    @param PB12_OS *os - Operating System.
    @param PB12_PCB_List *dest - List PCB will be moved to.
    @param int pid - Process ID
*/
void pb12MoveFromReady(PB12_OS *os, PB12_PCB_List *dest, int pid) {
    PB12_PCB *pcb;
    PB12_PCB *current;

    if (pb12Options & PB12_OPT_VERBOSE && dest != &os->ready_q) {
        printf("Moving process with PID %d off of Ready Queue.\n", pid);
    }

    current = os->ready_q.head;
    pcb = pb12RemovePcb(&os->ready_q, pid);
    pb12PushBackPcb(dest, pcb);

    if (pcb == current) {
        pb12CopyCPU(&os->hw->cpu, &pcb->cpu);

        if (pb12IsEmptyPcb(&os->ready_q)) {
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Ready queue is empty!  Deadlock?\n");
            }
        }
        else {
            pb12CopyCPU(&os->ready_q.head->cpu, &os->hw->cpu);

            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Process (%d) ready to exec '%s' with time slice of %d inst.\n",
                       os->ready_q.head->pid, os->ready_q.head->program, os->hw->cpu.ic);
            }
        }
    }
}


/**
    Preempt the currently running process.

    @param PB12_OS *os - Operating System.
*/
void pb12Preempt(PB12_OS *os) {
    if (pb12Options & PB12_OPT_VERBOSE)
        printf("Process (%d) completed time slice. Placing at tail of ready queue.\n", os->ready_q.head->pid);

    pb12CopyCPU(&os->hw->cpu, &os->ready_q.head->cpu);

    pb12MoveFirstToTopPcb(&os->ready_q);

    pb12CopyCPU(&os->ready_q.head->cpu, &os->hw->cpu);

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Process (%d) ready to exec '%s' with time slice of %d inst.\n",
               os->ready_q.head->pid, os->ready_q.head->program, os->hw->cpu.ic);
    }

    /* pb12MoveFromReady(os, &os->ready_q, os->ready_q.head->pid); */
}


/**
    See if there is a trap instruction to respond to.

    @param PB12_OS *os - Operating System.
*/
void pb12TrapInterrupt(PB12_OS *os) {
    if (os->hw->trap_num < 0)
        return;

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Trap Operation: num: %d, op: %d.\n", os->hw->trap_num, os->hw->trap_op);
    }

    if (os->hw->trap_num < PB12_TRAP_MAX_TRAP) {
        pb12TrapVectors[os->hw->trap_num](os);
    }

    os->hw->trap_num = -1;
}


/**
    Update operating system.

    @param PB12_OS *os - Operating System.
    @param int cpu_status - Return value from CPU.

    @return int - PB12_SUCCESS if running and PB12_TERMINATE if no processes.
*/
int pb12OsTick(PB12_OS *os, int cpu_status) {
    bool preempt;
    preempt = false;

    ++os->tick_count;

    if (os->hw->cpu.ic == 0) {
        preempt = true;
        if (pb12Options & PB12_OPT_TIMESTEP) {
            os->hw->cpu.ic = pb12TimeStep;
        }
        else {
            os->hw->cpu.ic = pb12RandIc(PB12_TIME_SLICE);
        }
    }

    pb12TrapInterrupt(os);

    if (cpu_status == PB12_TERMINATE || cpu_status == PB12_FAILURE)
        pb12TerminateProcess(os);

    if (pb12IsEmptyPcb(&os->ready_q)) {
        return PB12_TERMINATE;
    }
    else {
        if (preempt) {
            pb12Preempt(os);
        }
    }

    return PB12_SUCCESS;
}
