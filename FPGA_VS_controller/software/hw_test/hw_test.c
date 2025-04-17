#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "conf_port.h"
#include "shared_page.h"



int map_shared_structs(
        struct classic_vs_pl_conf_port **conf_port,
        struct shared_page **shared_page) {
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (!fd) {
        fprintf(stderr, "Error opening the file\n");
        return EXIT_FAILURE;
    }

    *conf_port = mmap(
      NULL, sizeof(struct classic_vs_pl_conf_port), PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_POPULATE, fd, 0x88000000);

    if (conf_port == NULL) {
        fprintf(stderr, "Error mapping structs\n");
        return EXIT_FAILURE;
    }
    
    *shared_page = mmap(
      NULL, sizeof(struct shared_page), PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_POPULATE, fd, 0x46d00000);

    

    if (shared_page == NULL) {
        fprintf(stderr, "Error mapping structs\n");
        return EXIT_FAILURE;
    }
    
    // close(fd);

    return EXIT_SUCCESS;
}

#define THRESHOLD 0.95

int similar(double a, double b) {
    double delta = a / b;
    // if (delta > 1)
    //     delta -= 1;

    // printf("%lf / %lf = %lf ~= %lf\n", a, b, a/b, delta);
    return delta > THRESHOLD && delta < (1 + THRESHOLD);
}

void quick_check(struct classic_vs_pl_conf_port *conf_port, struct shared_page *shared_page, double ivs3, double zdot, double res, double *x_old, int i) {
    double Af_val = 0.99940018;
    double Bf_val = 2.0;
    double Cf_val = -1.78393;
    double Df_val = -1051.78;
    double gain_val = -0.00001;

    double x_old_val = *x_old;
    double previous_x_old_hw = conf_port->x_old;

    double computed_res = ( (Cf_val) * (*x_old) ) + ( (Df_val) * ( (ivs3)*(gain_val) + (zdot) ) );
    
    *x_old = shared_page->marte_strategy ? ( (Af_val) * (*x_old) ) + ( (Bf_val) * ( (ivs3)*(gain_val) + (zdot) ) ) : 0;

    while (shared_page->input_counter != (conf_port->inner_counter % 0x100)) {}

    double hw_res = conf_port->res_val;
    
    double hw_x_old = conf_port->x_old;

    double delta_computed_res = computed_res / res - 1.0;
    double delta_hw_res = hw_res / res - 1.0;
    double delta_hw_computed_res = hw_res / computed_res - 1.0;
    double delta_x_old = hw_x_old / *x_old - 1.0;
    
    printf("%d) ivs3: %lf, zdot: %lf, read_res: %lf, x_old: %lf, computed_res: %lf (%.4lf%%), new_x_old: %lf, [previous_hw_x_old %lf], hw_res: %lf (%.4lf%%) (%.4lf%%), hw_x_old: %lf (%.4lf%%)\n",
        i, ivs3, zdot, res, x_old_val, computed_res, (delta_computed_res * 100), *x_old, previous_x_old_hw, hw_res, (delta_hw_res * 100), delta_hw_computed_res * 100, hw_x_old, delta_x_old * 100);

    if (isnan(hw_res) || isnan(hw_x_old) || delta_computed_res* delta_computed_res > 0.01 ||  delta_hw_res* delta_hw_res > 0.01 ||  delta_hw_computed_res* delta_hw_computed_res > 0.01 ||  delta_x_old* delta_x_old > 0.01 ) {
        printf("%lx\n", conf_port->raw_val);
        exit(EXIT_FAILURE);
    }
}

void wait_and_check(struct shared_page *shared_page, double res) {
    usleep(50000);

    
    if (!similar(shared_page->classic_output, res)) {
        printf("Wrong res! Expected: %lf found: %lf marte_strategy: %d\n",
            res,
            shared_page->classic_output,
            shared_page->marte_strategy);
        exit(EXIT_FAILURE);
    }

    // printf("Acceptable res. Expected: %lf found: %lf marte_strategy: %d\n",
    //         res,
    //         shared_page->classic_output,
    //         shared_page->marte_strategy);
}

int main(int argc, char **argv) {
    FILE *file;
    int i;
    double ivs3, zdot, res;
    double x_old, computed_res;
    struct classic_vs_pl_conf_port *conf_port;
    struct shared_page *shared_page;

    if (map_shared_structs(&conf_port, &shared_page) != EXIT_SUCCESS) {
        fprintf(stderr, "Error mapping structs\n");
        return EXIT_FAILURE;
    }

    conf_port->enabled = 0;

    /* Write computation constants */
    conf_port->Af_val = 0.99940018;
    conf_port->Bf_val = 2.0;
    conf_port->Cf_val = -1.78393;
    conf_port->Df_val = -1051.78;
    conf_port->gain_val = -0.00001;

    /* Shared page data */
    conf_port->shared_page_base = 0x46d00000;
    conf_port->counter_offset = 0x1D0;
    conf_port->ivs3_offset = 0x0B8;
    conf_port->zdot_offset = 0x0D0;
    
    // conf_port->marte_strategy_offset = 0x1D1;
    conf_port->safe_control_output_offset = 0x1C8;
    
    /* Finally, enable the module */
    conf_port->enabled = 1;

    file = fopen("new_values.mem", "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening the file\n");
        return EXIT_FAILURE;
    }

    i = 0;
    shared_page->marte_strategy = 0;
    x_old = 0;

    printf("Addr output_counter %p\n", &shared_page->output_counter);
    printf("Addr es_output %p\n", &shared_page->es_output);   
    // while (fscanf(file, "%lf %lf %lf", &ivs3, &zdot, &res) != EOF) {

    //     if (i == 3115) {
    //         printf("NEXT %d\n", getpid());
    //         getchar();
    //         // break;
    //     }

    //     shared_page->data.ivs3 = ivs3;
    //     shared_page->data.zdot = zdot;
    //     shared_page->input_counter++;

    //     for (; i < 10; i++) {

    //         quick_check(conf_port, shared_page, ivs3, zdot, res, &x_old, i);

    //         // wait_and_check(shared_page, res);

    //         shared_page->marte_strategy = i == 9;
    //         shared_page->input_counter++;
    //     }

    //     quick_check(conf_port, shared_page, ivs3, zdot, res, &x_old, i++);
    //     // wait_and_check(shared_page, res);
    // }

    fclose(file);

    return EXIT_SUCCESS;
}