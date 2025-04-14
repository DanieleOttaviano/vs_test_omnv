#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "shared_page.h"

struct shared_page *sp;

int map_page(void) {
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("cannot open /dev/mem");
        return EXIT_FAILURE;
    }
    
    sp = mmap(
        NULL,
        sizeof(struct shared_page),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_POPULATE,
        fd,
        0x46d00000);

    close(fd);

    if (sp == MAP_FAILED) {
        perror("cannot mmap /dev/mem");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    printf("HELLO WORLD!\n");
    map_page();

    if (argc > 1) {
        if (!(strcmp(argv[1], "reset"))) {
            for (int i = 0; i < 11; i++) {
                sp->pf_voltage[i] = 0;
                sp->pf_currents[i] = 0;
            }
            
            sp->vs3 = 0;
            sp->ivs3 = 0;
            sp->ip = 0;
            sp->zc = 0;
            sp->zdot = 0;
            for (int i = 0; i < 29; i++)
                sp->gaps[i] = 0;
            sp->es_output = 0;
            sp->classic_output = 0;
            sp->input_counter = 0;
            sp->marte_strategy = 0;
            sp->oracle_decision = 0;
        } else if (!(strcmp(argv[1], "write_values"))) {
            if (argc < 3) {
                printf("It needs at least 3 values");
                exit(EXIT_FAILURE);
            }
            sp->ivs3 = strtod(argv[2], NULL);
            sp->zdot = strtod(argv[3], NULL);
            sp->input_counter++;
        }
    }

    printf("pf_voltage's offset: 0x%p \t\t value %f\n", 
        &(sp->pf_voltage), sp->pf_voltage[0]);
    printf("vs3's offset: 0x%p \t\t value %f\n", 
        &(sp->vs3), sp->vs3);
    printf("pf_currents's offset: 0x%p \t\t value %f\n", 
        &(sp->pf_currents), sp->pf_currents[0]);
    printf("ivs3's offset: 0x%p \t\t value %f\n", 
        &(sp->ivs3), sp->ivs3);
    printf("ip's offset: 0x%p \t\t value %f\n", 
        &(sp->ip), sp->ip);
    printf("zc's offset: 0x%p \t\t value %f\n", 
        &(sp->zc), sp->zc);
    printf("zdot's offset: 0x%p \t\t value %f\n", 
        &(sp->zdot), sp->zdot);
    printf("gaps's offset: 0x%p \t\t value %f\n", 
        &(sp->gaps), sp->gaps[0]);
    printf("es_output's offset: 0x%p \t\t value %f\n", 
        &(sp->es_output), sp->es_output);
    printf("classic_output's offset: 0x%p \t\t value %f\n", 
        &(sp->classic_output), sp->classic_output);
    printf("input_counter's offset: 0x%p \t\t value %d\n", 
        &(sp->input_counter), sp->input_counter);
    printf("marte_strategy's offset: 0x%p \t\t value %d\n", 
        &(sp->marte_strategy), sp->marte_strategy);
    printf("oracle_decision's offset: 0x%p \t\t value %d\n", 
        &(sp->oracle_decision), sp->oracle_decision);
    // printf("0x%lx");
}