#ifndef PB12_H
#define PB12_H

/* Return codes */
#define PB12_SUCCESS        0
#define PB12_FAILURE        1
#define PB12_TERMINATE      2

/* Options */
#define PB12_OPT_VERBOSE    1
#define PB12_OPT_MESSAGES   2
#define PB12_OPT_TIMESTEP   4
#define PB12_OPT_FIRST_FIT  8
#define PB12_OPT_BEST_FIT   16
#define PB12_OPT_WORST_FIT  32

#define PB12_MEM_SIZE       1000
#define PB12_PROC_SIZE      100
#define PB12_TIME_SLICE     10

#ifdef __WIN32__
    #define PB12_PATH_SEP "\\"
#else
    #define PB12_PATH_SEP "/"
#endif

/* Error messages */
typedef enum S_PB12_ERROR {
    PB12_ERROR_INVALID_PTR_1,
    PB12_ERROR_INVALID_PTR_2,
    PB12_ERROR_INVALID_REG_1,
    PB12_ERROR_INVALID_REG_2,
    PB12_ERROR_INVALID_ADDR,
    PB12_ERROR_INVALID_OPCODE,
    PB12_ERROR_ADDRESS_RANGE,
    PB12_ERROR_PROCESS_SPACE,
    PB12_ERORR_DESTROYING_PROCESS,
    PB12_ERROR_PROCESS_DONT_EXIST,
    PB12_ERROR_EXECUTING,
    PB12_ERROR_INIT_CPU,
    PB12_ERORR_INIT_MEM,
    PB12_ERROR_INIT_OS,
    PB12_ERROR_PCB_NOT_FOUND,
    PB12_ERROR_MOVING_PCB
} PB12_ERROR;


extern unsigned int pb12Options;
extern int pb12TimeStep;


/**
    Called when an error occurs.  Only displayed if using -m flag.

    @param const char *format - Format string, like printf
    @param ... - Variable arguments, like printf
*/
void pb12ErrorMsg(const char *format, ...);


#endif /* PB12_H */
