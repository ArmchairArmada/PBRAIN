#ifndef PB12_PCB_H
#define PB12_PCB_H

#include <stdbool.h>
#include "pb12_cpu.h"

/* struct S_PB12_CPU; */
struct S_PB12_MemBlock;


typedef struct S_PB12_PCB {
    struct S_PB12_PCB *next_pcb;
    PB12_CPU cpu;
    int pid;                    /* Process id */
    int mem_req;    /* Amount of memory needed by process. */
    struct S_PB12_MemBlock* mem_block;  /* Block of memory used by process */
    unsigned int wait_time;  /* Number of ticks elapsed before process exec */
    char program[32];
} PB12_PCB;


typedef struct S_PB12_PCB_List {
    PB12_PCB *head;
    PB12_PCB *tail;
} PB12_PCB_List;


/**
    Initialize Process Control Block.

    @param PB12_PCB * pcb - Process Control Block.
    @param int pid - Process ID.
    @param const char* filename - filename associated with process.
    @param int mem_req - Required amount of memory for process.
*/
void pb12InitPcb(PB12_PCB *pcb, int pid, const char *filename, int mem_req);


/**
    Destroy Process Control Block.

    @param PB12_PCB * pcb - Process Control Block.
*/
void pb12DestroyPcb(PB12_PCB *pcb);


/**
    Associates a memory block with a PCB.

    @param PB12_PCB *pcb - Process Control Block.
    @param struct S_PB12_MemBlock *mem_block - Memory Block.
*/
void pb12SetPcbMem(PB12_PCB *pcb, struct S_PB12_MemBlock *mem_block);


/**
    Generate random instruction count number.

    @return int range - Random number of instructions.
*/
int pb12RandIc(int range);


/**
    Initialize PCB List.

    @param PB12_PCB_LIST *pcb_list - PCB List.
*/
void pb12InitPcbList(PB12_PCB_List *pcb_list);


/**
    Free the PCB List.  Destroys all PCBs in list.

    @param PB12_PCB_LIST *pcb_list - PCB List.
*/
void pb12FreePcbList(PB12_PCB_List *pcb_list);


/**
    Check if PCB list is empty.

    @param PB12_PCB_LIST *pcb_list - PCB List.

    @return bool - true if empty, else false
*/
bool pb12IsEmptyPcb(PB12_PCB_List *pcb_list);


/**
    Finds a PCB by its PID.

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param int pid - Finds a PCB by its pid.

    @return PB12_PCB* - PCB for process.
*/
PB12_PCB* pb12FindPcb(PB12_PCB_List *pcb_list, int pid);


/**
    Finds the PCB that comes before the PCB that has a PID.

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param int pid - Process ID.

    @param PB12_PCB* PCB before the one that has the pid.
*/
PB12_PCB* pb12FindPrevPcb(PB12_PCB_List *pcb_list, int pid);


/**
    Remove a Process Control Block.

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param int pid - ID of process to remove.

    @param PB12_PCB* - PCB that has been removed.
*/
PB12_PCB* pb12RemovePcb(PB12_PCB_List *pcb_list, int pid);


/**
    Push PCB onto the back of the PCB List (putting it on tail).

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param PB12_PCB *pcb - PCB to put on tail of list.
*/
void pb12PushBackPcb(PB12_PCB_List *pcb_list, PB12_PCB *pcb);


/**
    Push PCB onto the front of the PCB List (putting it at head).

    @param PB12_PCB_LIST *pcb_list - PCB List.
    @param PB12_PCB *pcb - PCB to put on head of list.
*/
void pb12PushFrontPcb(PB12_PCB_List *pcb_list, PB12_PCB *pcb);


/**
    Take PCB off of head of list.

    @param PB12_PCB_LIST *pcb_list - PCB List.

    @return PB12_PCB* PCB removed from front of list.
*/
PB12_PCB* pb12DequeuePcb(PB12_PCB_List *pcb_list);


/**
    Move PCB from head to tail of list.

    @param PB12_PCB_LIST *pcb_list - PCB List.
*/
void pb12MoveFirstToTopPcb(PB12_PCB_List *pcb_list);


/**
    Move PCB with a PID from one List to another List.

    @param PB12_PCB_LIST *source - PCB List to take PCB from.
    @param PB12_PCB_LIST *dest - PCB List to move PCB to.
    @param int pid - Process ID of PCB to move.
*/
void pb12MovePcb(PB12_PCB_List *source, PB12_PCB_List *dest, int pid);

#endif /* PB12_PCB_H */
