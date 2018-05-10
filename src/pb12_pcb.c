#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pb12.h"
#include "pb12_pcb.h"
#include "pb12_cpu.h"
#include "pb12_strings.h"
#include "pb12_alloc.h"

/**
    Initialize Process Control Block.

    @param PB12_PCB * pcb - Process Control Block.
    @param int pid - Process ID.
    @param int mem_req - Required amount of memory for process.
*/
void pb12InitPcb(PB12_PCB *pcb, int pid, const char* filename, int mem_req) {
    pcb->next_pcb = NULL;
    pcb->pid = pid;
    pcb->mem_req = mem_req;
    pcb->mem_block = NULL;
    pcb->wait_time = 0;

    pb12InitCpu(&pcb->cpu);

    if (pb12Options & PB12_OPT_TIMESTEP) {
        pcb->cpu.ic = pb12TimeStep;
    }
    else {
        pcb->cpu.ic = pb12RandIc(PB12_TIME_SLICE);
    }

    strncpy(pcb->program, filename, sizeof(pcb->program));

    if (pb12Options & PB12_OPT_VERBOSE)
        printf("Init PCB - PID: %d, Program: \"%s\", Memory: %d, IC = %d.\n",
               pcb->pid, pcb->program, pcb->mem_req, pcb->cpu.ic);
}


/**
    Destroy Process Control Block.

    @param PB12_PCB * pcb - Process Control Block.
*/
void pb12DestroyPcb(PB12_PCB *pcb) {
    free(pcb);
}


/**
    Associates a memory block with a PCB.

    @param PB12_PCB *pcb - Process Control Block.
    @param struct S_PB12_MemBlock *mem_block - Memory Block.
*/
void pb12SetPcbMem(PB12_PCB *pcb, struct S_PB12_MemBlock *mem_block) {
    pcb->mem_block = mem_block;
    pcb->cpu.bar = mem_block->address;
    pcb->cpu.lr = mem_block->address + mem_block->length;
}


/**
    Generate random instruction count number.

    @return int range - Random number of instructions.
*/
int pb12RandIc(int range) {
    return 1 + (int)(range * ((float)rand() / RAND_MAX));
}


/**
    Initialize PCB List.

    @param PB12_PCB_LIST *pcb_list - PCB List.
*/
void pb12InitPcbList(PB12_PCB_List *pcb_list) {
    pcb_list->head = NULL;
    pcb_list->tail = NULL;
}


/**
    Free the PCB List.  Destroys all PCBs in list.

    @param PB12_PCB_LIST *pcb_list - PCB List.
*/
void pb12FreePcbList(PB12_PCB_List *pcb_list) {
    PB12_PCB *pcb;
    PB12_PCB *next;

    pcb = pcb_list->head;
    while (pcb != NULL) {
        next = pcb->next_pcb;
        pb12DestroyPcb(pcb);
        pcb = next;
    }

    pcb_list->head = NULL;
    pcb_list->tail = NULL;
}


/**
    Check if PCB list is empty.

    @param PB12_PCB_LIST *pcb_list - PCB List.

    @return bool - true if empty, else false
*/
bool pb12IsEmptyPcb(PB12_PCB_List *pcb_list) {
    return pcb_list->head == NULL;
}


/**
    Finds a PCB by its PID.

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param int pid - Finds a PCB by its pid.

    @return PB12_PCB* - PCB for process.
*/
PB12_PCB* pb12FindPcb(PB12_PCB_List *pcb_list, int pid) {
    PB12_PCB *pcb;

    pcb = pcb_list->head;
    while (pcb != NULL && pcb->pid != pid) {
        pcb = pcb->next_pcb;
    }

    return pcb;
}


/**
    Finds the PCB that comes before the PCB that has a PID.

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param int pid - Process ID.

    @param PB12_PCB* PCB before the one that has the pid.
*/
PB12_PCB* pb12FindPrevPcb(PB12_PCB_List *pcb_list, int pid) {
    PB12_PCB *pcb;

    pcb = pcb_list->head;
    if (pcb->pid == pid)
        return NULL;

    while (pcb != NULL && pcb->next_pcb != NULL && pcb->next_pcb->pid != pid) {
        pcb = pcb->next_pcb;
    }

    return pcb;
}


/**
    Remove a Process Control Block.

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param int pid - ID of process to remove.

    @param PB12_PCB* - PCB that has been removed.
*/
PB12_PCB* pb12RemovePcb(PB12_PCB_List *pcb_list, int pid) {
    PB12_PCB *prev_pcb;
    PB12_PCB *pcb;

    prev_pcb = pb12FindPrevPcb(pcb_list, pid);
    if (prev_pcb == NULL) {
        if (pcb_list->head->pid == pid) {
            pcb = pcb_list->head;
            pcb_list->head = pcb_list->head->next_pcb;
            if (pcb_list->tail == pcb)
                pcb_list->tail = prev_pcb;
            return pcb;
        }
        else {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_PCB_NOT_FOUND]);
            return NULL;
        }
    }

    pcb = prev_pcb->next_pcb;
    prev_pcb->next_pcb = pcb->next_pcb;
    pcb->next_pcb = NULL;
    if (pcb_list->tail == pcb)
        pcb_list->tail = prev_pcb;

    return pcb;
}


/**
    Push PCB onto the back of the PCB List (putting it on tail).

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param PB12_PCB *pcb - PCB to put on tail of list.
*/
void pb12PushBackPcb(PB12_PCB_List *pcb_list, PB12_PCB *pcb) {
    pcb->next_pcb = NULL;
    if (pcb_list->tail == NULL) {
        pcb_list->head = pcb;
        pcb_list->tail = pcb;
    }
    else {
        pcb_list->tail->next_pcb = pcb;
        pcb_list->tail = pcb;
    }
}


/**
    Push PCB onto the front of the PCB List (putting it at head).

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param PB12_PCB *pcb - PCB to put on head of list.
*/
void pb12PushFrontPcb(PB12_PCB_List *pcb_list, PB12_PCB *pcb) {
    if (pcb_list->head == NULL) {
        pcb_list->head = pcb;
        pcb_list->tail = pcb;
    }
    else {
        pcb->next_pcb = pcb_list->head;
        pcb_list->head = pcb;
    }
}


/**
    Take PCB off of head of list.

    @param PB12_PCB_LIST *pcb_list - PCB List.

    @return PB12_PCB* PCB removed from front of list.
*/
PB12_PCB* pb12DequeuePcb(PB12_PCB_List *pcb_list) {
    PB12_PCB *pcb;

    pcb = pcb_list->head;

    if (pcb_list->head != NULL) {
        pcb_list->head = pcb_list->head->next_pcb;
        if (pcb_list->head == NULL) {
            pcb_list->tail = NULL;
        }
    }

    return pcb;
}


/**
    Move PCB from head to tail of list.

    @param PB12_PCB_LIST *pcb_list - PCB List.
*/
void pb12MoveFirstToTopPcb(PB12_PCB_List *pcb_list) {
    PB12_PCB *pcb;

    pcb = pb12DequeuePcb(pcb_list);
    if (pcb == NULL) {
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_MOVING_PCB]);
    }

    pb12PushBackPcb(pcb_list, pcb);
}


/**
    Move PCB with a PID from one List to another List.

    @param PB12_PCB_LIST *source - PCB List to take PCB from.
    @param PB12_PCB_LIST *dest - PCB List to move PCB to.
    @param int pid - Process ID of PCB to move.
*/
void pb12MovePcb(PB12_PCB_List *source, PB12_PCB_List *dest, int pid) {
    PB12_PCB *pcb;

    pcb = pb12RemovePcb(source, pid);
    if (pcb == NULL)
        return;

    pb12PushBackPcb(dest, pcb);
}
