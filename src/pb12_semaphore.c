#include <stdio.h>
#include "pb12.h"
#include "pb12_semaphore.h"
#include "pb12_os.h"

/**
    Initialize a Semaphore.

    @param PB12_Semaphore *sem - Semaphore to initialize.
    @param int count - Initial count.
*/
void pb12SemInit(PB12_Semaphore *sem, int count) {
    sem->count = count;
    pb12InitPcbList(&sem->sem_q);
}


/**
    Waits a semaphore.

    @param PB12_Semaphore *sem - Semaphore.
    @param struct S_PB12_OS *os - Operating System.
*/
void pb12SemWait(PB12_Semaphore *sem, PB12_OS *os) {
    --sem->count;
    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Wait sem count: %d\n", sem->count);
    }
    if (sem->count < 0) {
        if (pb12Options & PB12_OPT_VERBOSE) {
            printf("Semaphore blocking process (%d)\n", pb12CurrentPid(os));
        }

        pb12MoveFromReady(os, &sem->sem_q, pb12CurrentPid(os));
    }
}


/**
    Signals a semaphore.

    @param PB12_Semaphore *sem - Semaphore.
    @param struct S_PB12_OS *os - Operating System.
*/
void pb12SemSignal(PB12_Semaphore *sem, PB12_OS *os) {
    ++sem->count;
    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Signal sem count: %d\n", sem->count);
    }
    if (sem->count <= 0) {
        if (pb12Options & PB12_OPT_VERBOSE) {
            printf("Semaphore unblocking process (%d)\n", sem->sem_q.head->pid);
        }

        pb12MoveToReady(os, &sem->sem_q, sem->sem_q.head->pid);
    }
}
