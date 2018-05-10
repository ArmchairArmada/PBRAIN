#ifndef PB12_PBRAIN_H
#define PB12_PBRAIN_H

#include "pb12_hw.h"
#include "pb12_os.h"

typedef struct S_PB12_PBrain {
    PB12_HW hw;
    PB12_OS os;
} PB12_PBrain;


/* Prototypes */

/**
    Initializes PBrain Virtual Machine.

    @param PB12_PBRAIN *pbrain - PBrain12 VM.
    @param int mem_size - Size of memory in VM.

    @return int - PB12_SUCCESS or PB12_FAILURE
*/
int pb12InitPBrain(PB12_PBrain *pbrain, int mem_size);


/**
    Destroy PBrain Virtual Machine.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.
*/
void pb12DestroyPBrain(PB12_PBrain *pbrain);


/**
    Dump Virtual Machine state to console.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.
*/
void pb12Dump(PB12_PBrain *pbrain);


/**
    Clock tick for virtual machine -- updates CPU and OS.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.

    @return int  PB12_SUCCESS, PB12_FAILURE, or PB12_TERMINATE
*/
int pb12Tick(PB12_PBrain *pbrain);


/**
    Runs as long as executing is successful.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.

    @return int  PB12_SUCCESS, PB12_FAILURE, or PB12_TERMINATE
*/
int pb12Run(PB12_PBrain *state);

#endif /* PB12_PBRAIN_H */
