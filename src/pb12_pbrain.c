#include <stdio.h>
#include "pb12.h"
#include "pb12_pbrain.h"
#include "pb12_cpu.h"
#include "pb12_mem.h"
#include "pb12_strings.h"


/**
    Initializes PBrain Virtual Machine.

    @param PB12_PBRAIN *pbrain - PBrain12 VM.
    @param int mem_size - Size of memory in VM.

    @return int - PB12_SUCCESS or PB12_FAILURE
*/
int pb12InitPBrain(PB12_PBrain *pbrain, int mem_size) {
    if (pb12InitCpu(&pbrain->hw.cpu) == PB12_FAILURE) {
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INIT_CPU]);
        return PB12_FAILURE;
    }

    if (pb12InitMem(&pbrain->hw.mem, mem_size) == PB12_FAILURE) {
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INIT_CPU]);
        return PB12_FAILURE;
    }

    /* Trap instruction initializing. */
    pbrain->hw.trap_num = -1;
    pbrain->hw.trap_op = 0;

    if (pb12InitOs(&pbrain->os, &pbrain->hw) == PB12_FAILURE) {
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INIT_OS]);
        return PB12_FAILURE;
    }

    return PB12_SUCCESS;
}


/**
    Destroy PBrain Virtual Machine.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.
*/
void pb12DestroyPBrain(PB12_PBrain *pbrain) {
    pb12DestroyOs(&pbrain->os);
    pb12DestroyMem(&pbrain->hw.mem);
}


/**
    Dump Virtual Machine state to console.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.
*/
void pb12Dump(PB12_PBrain *pbrain) {
    pb12DumpCPU(&pbrain->hw.cpu);
    pb12DumpMemory(&pbrain->hw.mem);
}


/**
    Clock tick for virtual machine -- updates CPU and OS.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.

    @return int  PB12_SUCCESS, PB12_FAILURE, or PB12_TERMINATE
*/
int pb12Tick(PB12_PBrain *pbrain) {
    int ret_val;

    if (pb12Options & PB12_OPT_VERBOSE)
        printf("  PID=%d ", pb12CurrentPid(&pbrain->os));

    pb12Fetch(&pbrain->hw.cpu, &pbrain->hw.mem);
    ret_val = pb12Execute(&pbrain->hw.cpu, &pbrain->hw);

    /*
    if (ret_val == PB12_FAILURE)
        return ret_val;

    if (ret_val == PB12_TERMINATE)
        pb12TerminateProcess(&pbrain->os);
    */

    /* pb12DumpCPU(&pbrain->hw.cpu); */

    ret_val = pb12OsTick(&pbrain->os, ret_val);
    return ret_val;
}


/**
    Runs as long as executing is successful.

    @param PB12_PBRAIN *pbrain - PBrain Virtual Machine.

    @return int  PB12_SUCCESS, PB12_FAILURE, or PB12_TERMINATE
*/
int pb12Run(PB12_PBrain *pbrain) {
    int ret_val = 0;

    pb12StartOs(&pbrain->os);

    while (ret_val == 0) {
        ret_val = pb12Tick(pbrain);
    }
    return ret_val;
}
