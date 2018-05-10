#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "pb12.h"
#include "pb12_pbrain.h"
#include "pb12_stats.h"


int main(int argc, char **argv) {
    PB12_PBrain pbrain;
    int i;
    int flag_count;
    bool folder_loader;
    DIR *d;
    struct dirent *dir;
    char buffer[256];
    struct stat s;

    folder_loader = false;

    flag_count = 0;

    if (argc < 2) {
        printf("ERROR: Must provide arguments.\n");
        return EXIT_FAILURE;
    }

    /* Set option flags */
    for (i=1; i<argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            ++flag_count;
            puts("Options:");
            puts(" -h    This help information");
            puts(" -v    Verbose output");
            puts(" -m    Display messages");
            puts(" -t N  Set time step to N instructions");
            puts(" -ff   First fit allocation");
            puts(" -bf   Best fit allocation");
            puts(" -wf   Worst fit allocation");
            puts(" -d D  Load all programs that are in directory D");
            return EXIT_SUCCESS;
        }

        else if (strcmp(argv[i], "-v") == 0) {
            ++flag_count;
            pb12Options |= PB12_OPT_VERBOSE;
        }

        else if (strcmp(argv[i], "-m") == 0) {
            ++flag_count;
            pb12Options |= PB12_OPT_MESSAGES;
        }

        else if (strcmp(argv[i], "-t") == 0) {
            flag_count += 2;
            pb12Options |= PB12_OPT_TIMESTEP;

            i++;
            sscanf(argv[i], "%d", &pb12TimeStep);
        }

        else if (strcmp(argv[i], "-ff") == 0) {
            ++flag_count;
            pb12Options |= PB12_OPT_FIRST_FIT;
        }

        else if (strcmp(argv[i], "-bf") == 0) {
            ++flag_count;
            pb12Options |= PB12_OPT_BEST_FIT;
        }

        else if (strcmp(argv[i], "-wf") == 0) {
            ++flag_count;
            pb12Options |= PB12_OPT_WORST_FIT;
        }

        else if (strcmp(argv[i], "-d") == 0) {
            ++flag_count;
            folder_loader = true;
        }
    }

    if (pb12InitPBrain(&pbrain, PB12_MEM_SIZE) == PB12_FAILURE) {
        puts("Error initializing PBrain12");
        return EXIT_FAILURE;
    }

    /* Load programs -- they must come after the flags */
    if (folder_loader) {
        d = opendir(argv[flag_count+1]);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                sprintf(buffer, "%s%s%s",
                         argv[flag_count+1], PB12_PATH_SEP, dir->d_name);
                         
                if (stat(buffer, &s) == 0) {
                	if (S_ISREG(s.st_mode)) {
	                    pb12QueueProgram(&pbrain.os, buffer);
	                }
	            }
            }
            closedir(d);
        }
    }
    else {
        for (i=flag_count+1; i<argc; i++) {
            pb12QueueProgram(&pbrain.os, argv[i]);
        }
    }

    pb12Run(&pbrain);

    /* This is only here for project 1 */
    /*
    printf("\n");
    pb12PrintMem(&pbrain.hw.mem, 50, 12);
    */

    printf("\n");
    pb12Dump(&pbrain);

    printf("\n");
    pb12PrintStats(pbrain.os.stats, pbrain.os.next_pid);

    pb12DestroyPBrain(&pbrain);

    return EXIT_SUCCESS;
}
