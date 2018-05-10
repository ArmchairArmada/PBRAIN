#ifndef PB12_SEMAPHORE_H
#define PB12_SEMAPHORE_H

#include "pb12_pcb.h"

struct S_PB12_OS;

typedef struct S_PB12_Semaphore {
    int count;
    PB12_PCB_List sem_q;
} PB12_Semaphore;


/**
    Initialize a Semaphore.

    @param PB12_Semaphore *sem - Semaphore to initialize.
    @param int count - Initial count.
*/
void pb12SemInit(PB12_Semaphore *sem, int count);


/**
    Waits a semaphore.

    @param PB12_Semaphore *sem - Semaphore.
    @param struct S_PB12_OS *os - Operating System.
*/
void pb12SemWait(PB12_Semaphore *sem, struct S_PB12_OS *os);


/**
    Signals a semaphore.

    @param PB12_Semaphore *sem - Semaphore.
    @param struct S_PB12_OS *os - Operating System.
*/
void pb12SemSignal(PB12_Semaphore *sem, struct S_PB12_OS *os);

#endif /* PB12_SEMAPHORE_H */
