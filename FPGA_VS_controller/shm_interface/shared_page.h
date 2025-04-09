#include <stdint.h>

struct shared_page {
    /*
        DATA Structure
    */
    struct data {
        /* 0x000 */
        double pf_voltage[11];
        /* 0x058 */
        double vs3;
        /* 0x060 */
        double pf_currents[11];
        /* 0x0B8 */
        double ivs3;
        /* 0x0C0 */
        double ip;
        /* 0x0C8 */
        double zc;
        /* 0x0D0 */
        double zdot;
        /* 0x1B8 */
        double gaps[29];
    };
    
    /* 0x1C0 */
    double es_output;
    /* 0x1C8 */
    double classic_output;

    struct marte_status {
        /* 0x1D0 */
        uint8_t input_counter;
        /* 0x1D1 */
        uint8_t marte_strategy;
    };
    
    /* 0x1D2 */
    uint8_t oracle_decision;
    /* 0x1D3 */
    uint8_t output_counter;
};