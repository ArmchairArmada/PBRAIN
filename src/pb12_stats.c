#include <stdio.h>
#include "pb12_stats.h"

/**
    Prints a report on process stats.
*/
void pb12PrintStats(PB12_ProcStat *stats, int count) {
    int i;
    int start_sum;
    int duration_sum;
    int zeros;

    start_sum = 0;
    duration_sum = 0;
    zeros = 0;

    for(i=0; i<count; i++) {
        if(stats[i].start_time == 0) {
            ++zeros;
        }
        start_sum += stats[i].start_time;
        duration_sum += stats[i].end_time - stats[i].start_time;

        printf("Process %d: start = %d, end = %d, duration = %d\n",
               i, stats[i].start_time, stats[i].end_time,
               stats[i].end_time - stats[i].start_time);
    }

    printf("\n");
    printf("Average wait time: %f\n", start_sum / ((float)count));
    printf("Average non-zero wait time: %f\n", start_sum / ((float)(count - zeros)));
    printf("Average duration: %f\n", duration_sum/ ((float)count));
}
