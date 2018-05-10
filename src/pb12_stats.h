#ifndef PB12_STATS_H
#define PB12_STATS_H

#define PB12_STAT_COUNT 50

/* This is simply used for keeping track of process statistics */
typedef struct S_PB12_ProcStat {
    int start_time;
    int end_time;
} PB12_ProcStat;


/**
    Prints a report on process stats.
*/
void pb12PrintStats(PB12_ProcStat *stats, int count);

#endif /* PB12_STATS_H */
