#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>

#include "conf_port.h"
#include "shared_page.h"

#define SHARED_PAGE_ADDR 0x46d00000

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
	  MAP_SHARED | MAP_POPULATE, fd, SHARED_PAGE_ADDR);

	

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

typedef struct {
	char values_filename[256];
	int log_level;
	int iterations;
} hw_test_config_t;

void print_help() {
	printf("Usage: hw_test [--values <file>] [--log-level <level>] [--iterations <num>] [--help]\n");
	printf("Options:\n");
	printf("  --values <file>      Path to the file containing test values (default: new_values.mem)\n");
	printf("  --log-level <level>  Logging level (0: none, 1: errors, 2: info, 3: verbose) (default: 1)\n");
	printf("  --iterations <num>      Number of iterations to run (default: 10; 0 for entire file)\n");
	printf("  --help               Print this help message\n");
}

/**
 * Parse command line arguments
 * --values: Path to the file containing test values, default "new_values.mem"
 * --log-level: Logging level (0: none, 1: errors, 2: info, 3: verbose), default 1
 * --iterations: Number of iterations to run, default 10; 0 for entire file
 * --help: Print this help message
 * @param config Pointer to the configuration structure to fill
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, -1 on failure
 */
int parse_args(hw_test_config_t *config, int argc, char **argv) {
	// Set default values
	snprintf(config->values_filename, sizeof(config->values_filename), "new_values.mem");
	config->log_level = 1;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--values") == 0 && i + 1 < argc) {
			snprintf(config->values_filename, sizeof(config->values_filename), "%s", argv[++i]);
		} else if (strcmp(argv[i], "--log-level") == 0 && i + 1 < argc) {
			config->log_level = atoi(argv[++i]);
			// Check validity
			if (config->log_level < 0 || config->log_level > 3) {
				fprintf(stderr, "Invalid log level: %d\n", config->log_level);
				print_help();
				return -1;
			}
		} else if (strcmp(argv[i], "--iterations") == 0) {
			if (i + 1 < argc) {
				config->iterations = atoi(argv[++i]);
			} else {
				fprintf(stderr, "Missing value for --iterations\n");
				print_help();
				return -1;
			}
		} else if (strcmp(argv[i], "--help") == 0) {
			print_help();
			exit(0);
		} else {
			fprintf(stderr, "Unknown argument: %s\n", argv[i]);
			print_help();
			return -1;
		}
	}

	return 0;
}

struct classic_vs_pl_conf_port *conf_port;

void exit_handler() {
	conf_port->enabled = 0;
	printf("Disabled HW module\n");
}

void signal_handler() {
	exit(EXIT_FAILURE);
}

void configure_signals_hooks() {
	// Use sigaction to catch SIGINT and SIGTERM
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

void configure_atexit_handler() {
	atexit(exit_handler);
	configure_signals_hooks(conf_port);
}

int main(int argc, char **argv) {
	FILE *file;
	int i;
	double ivs3, zdot, res;
	double x_old, computed_res;
	// struct classic_vs_pl_conf_port *conf_port;
	struct shared_page *shared_page;

	hw_test_config_t config;
	if (parse_args(&config, argc, argv) != 0) {
		return EXIT_FAILURE;
	}

	file = fopen(config.values_filename, "r");

	if (file == NULL) {
		fprintf(stderr, "Error opening the file %s\n", config.values_filename);
		return EXIT_FAILURE;
	}
	
	if (map_shared_structs(&conf_port, &shared_page) != EXIT_SUCCESS) {
		fprintf(stderr, "Error mapping structs\n");
		return EXIT_FAILURE;
	}

	configure_atexit_handler();

	conf_port->enabled = 0;

	/* Write computation constants */
	conf_port->Af_val 	= 0.99940018;
	conf_port->Bf_val 	= 2.0;
	conf_port->Cf_val 	= -1.78393;
	conf_port->Df_val 	= -1051.78;
	conf_port->gain_val = -0.00001;

	/* Shared page data */
	conf_port->shared_page_base 			= SHARED_PAGE_ADDR;//(uint64_t)(uintptr_t)shared_page;//
	printf("Shared page base: %lx\n", conf_port->shared_page_base);

	conf_port->counter_offset 				= ((uint64_t) &(shared_page->input_counter)) - ((uint64_t)(uintptr_t)shared_page);//0x1D0;//
	printf("Counter offset: %lx, computed: %lx\n", conf_port->counter_offset, ((uint64_t) &(shared_page->input_counter)) - ((uint64_t)(uintptr_t)shared_page));
	//0x1D0;
	conf_port->ivs3_offset 					= ((uint64_t) &(shared_page->data.ivs3)) - ((uint64_t)(uintptr_t)shared_page);//0x0B8;//
	printf("IVS3 offset: %lx, computed: %lx\n", conf_port->ivs3_offset, ((uint64_t) &(shared_page->data.ivs3)) - ((uint64_t)(uintptr_t)shared_page));
	//0x0B8;
	conf_port->zdot_offset 					= ((uint64_t) &(shared_page->data.zdot)) - ((uint64_t)(uintptr_t)shared_page);//0x0D0;//
	printf("ZDOT offset: %lx, computed: %lx\n", conf_port->zdot_offset, ((uint64_t) &(shared_page->data.zdot)) - ((uint64_t)(uintptr_t)shared_page));
	//0x0D0;
	conf_port->safe_control_output_offset 	= ((uint64_t) &(shared_page->classic_output)) - ((uint64_t)(uintptr_t)shared_page);//0x1C8;//
	printf("Safe control output offset: %lx, computed: %lx\n", conf_port->safe_control_output_offset, ((uint64_t) &(shared_page->classic_output)) - ((uint64_t)(uintptr_t)shared_page));
	//0x1C8;
	conf_port->marte_strategy_offset 		= ((uint64_t) &(shared_page->marte_strategy)) - ((uint64_t)(uintptr_t)shared_page);//0x1D1;//
	printf("Marte strategy offset: %lx, computed: %lx\n", conf_port->marte_strategy_offset, ((uint64_t) &(shared_page->marte_strategy)) - ((uint64_t)(uintptr_t)shared_page));
	//0x1D1;
	
	/* Finally, enable the module */
	conf_port->enabled = 1;


	i = 0;
	shared_page->marte_strategy = 0;
	x_old = 0;

	while (fscanf(file, "%lf %lf %lf", &ivs3, &zdot, &res) != EOF) {

		// if (i == 3115) {
		//     printf("NEXT %d\n", getpid());
		//     getchar();
		//     // break;
		// }

		shared_page->data.ivs3 = ivs3;
		shared_page->data.zdot = zdot;
		shared_page->input_counter++;

		for (; i < 10; i++) {

			quick_check(conf_port, shared_page, ivs3, zdot, res, &x_old, i);

			// wait_and_check(shared_page, res);

			shared_page->marte_strategy = i == 9;
			shared_page->input_counter++;
		}

		quick_check(conf_port, shared_page, ivs3, zdot, res, &x_old, i++);
		// wait_and_check(shared_page, res);
	}

	fclose(file);

	return EXIT_SUCCESS;
}