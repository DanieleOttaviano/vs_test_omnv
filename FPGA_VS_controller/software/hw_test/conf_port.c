#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf_port.h"

struct classic_vs_pl_conf_port *conf_port;

int map_page(void) {
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("cannot open /dev/mem");
        return EXIT_FAILURE;
    }
    
    conf_port = mmap(
        NULL,
        sizeof(struct classic_vs_pl_conf_port),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_POPULATE,
        fd,
        0x80000000);

    close(fd);

    if (conf_port == MAP_FAILED) {
        perror("cannot mmap /dev/mem");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    map_page();

    if (argc > 1) {
        if (!(strcmp(argv[1], "reset"))) {
            conf_port->enabled = 1;

            conf_port->counter_offset = 0x1D0;
            conf_port->ivs3_offset = 0x0B8;
            conf_port->zdot_offset = 0x0D0;
            conf_port->marte_strategy_offset = 0;
            conf_port->safe_control_output_offset = 0x1C8;

            conf_port->shared_page_base = 0x46d00000;
            conf_port->Af_val = 0.99940018;
            conf_port->Bf_val = 2.0;
            conf_port->Cf_val = -1.78393;
            conf_port->Df_val = -1051.78;
            conf_port->gain_val = -0.00001;
        }
    }

    printf("%20s address %p value 0x%x\n", "enabled", &(conf_port->enabled), conf_port->enabled);
    printf("%20s address %p value 0x%x\n", "counter_offset", &(conf_port->counter_offset), conf_port->counter_offset);
    printf("%20s address %p value 0x%x\n", "ivs3_offset", &(conf_port->ivs3_offset), conf_port->ivs3_offset);
    printf("%20s address %p value 0x%x\n", "zdot_offset", &(conf_port->zdot_offset), conf_port->zdot_offset);
    printf("%20s address %p value 0x%x\n", "marte_strategy_offset", &(conf_port->marte_strategy_offset), conf_port->marte_strategy_offset);
    printf("%20s address %p value 0x%x\n", "safe_control_output_offset", &(conf_port->safe_control_output_offset), conf_port->safe_control_output_offset);
    printf("%20s address %p value 0x%x\n", "res_0_0", &(conf_port->res_0_0), conf_port->res_0_0);
    printf("%20s address %p value %lx\n", "shared_page_base", &(conf_port->shared_page_base), conf_port->shared_page_base);
    printf("%20s address %p value %.10f repr 0x%lx\n", "Af_val", &(conf_port->Af_val), conf_port->Af_val, conf_port->Af_val);
    printf("%20s address %p value %.10f repr 0x%lx\n", "Bf_val", &(conf_port->Bf_val), conf_port->Bf_val, conf_port->Bf_val);
    printf("%20s address %p value %.10f repr 0x%lx\n", "Cf_val", &(conf_port->Cf_val), conf_port->Cf_val, conf_port->Cf_val);
    printf("%20s address %p value %.10f repr 0x%lx\n", "Df_val", &(conf_port->Df_val), conf_port->Df_val, conf_port->Df_val);
    printf("%20s address %p value %.10f repr 0x%lx\n", "gain_val", &(conf_port->gain_val), conf_port->gain_val, conf_port->gain_val);
}