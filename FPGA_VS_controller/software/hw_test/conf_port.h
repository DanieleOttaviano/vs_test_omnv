#include <stdint.h>



struct classic_vs_pl_conf_port {
    /*0x00*/ 
    uint16_t enabled; // 1
    /* Shared page ref */
    /*0x02*/ 
    uint16_t counter_offset;
    /*0x04*/ 
    uint16_t ivs3_offset;
    /*0x06*/ 
    uint16_t zdot_offset;
    /*0x08*/ 
    uint16_t marte_strategy_offset;
    /*0x0A*/ 
    uint16_t safe_control_output_offset;
    /*0x0C*/ 
    uint32_t res_0_0;
    /*0x10*/
    uint64_t shared_page_base;
    /* Constant values */
    /*0x18*/
    double Af_val;
    /*0x20*/
    double Bf_val;
    /*0x28*/
    double Cf_val;
    /*0x30*/
    double Df_val;
    /*0x38*/
    double gain_val;
    /*0x40*/
    uint8_t res_0_1[0xC0];
    /*0x100*/
    double x_old;
    uint64_t res_0_2;
    /*0x110*/
    uint64_t inner_counter;
    uint64_t res_0_3;
    /*0x120*/
    // double res_val;
    union {
        double res_val;
        uint64_t raw_val;
    };
    uint64_t res_0_4;
};