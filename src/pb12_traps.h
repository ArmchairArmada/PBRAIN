#ifndef PB12_TRAPS_H
#define PB12_TRAPS_H

#define PB12_TRAP_WAIT      0
#define PB12_TRAP_SIGNAL    1
#define PB12_TRAP_PID       2
#define PB12_TRAP_DUMP      3
#define PB12_TRAP_MAX_TRAP  4

struct S_PB12_OS;

extern void (*pb12TrapVectors[PB12_TRAP_MAX_TRAP])(struct S_PB12_OS *os);

/**
    Wait on Semaphore trap.

    @param struct S_PB12_OS *os - Operating System
*/
void pb12TrapWait(struct S_PB12_OS *os);

/**
    Signal Semaphore trap.

    @param struct S_PB12_OS *os - Operating System
*/
void pb12TrapSignal(struct S_PB12_OS *os);

/**
    Get Process ID trap.

    @param struct S_PB12_OS *os - Operating System
*/
void pb12TrapPID(struct S_PB12_OS *os);


/**
    Trap added for debugging purposes.

    @param struct S_PB12_OS *os - Operating System
*/
void pb12TrapDump(struct S_PB12_OS *os);

#endif /* PB12_TRAPS_H */
