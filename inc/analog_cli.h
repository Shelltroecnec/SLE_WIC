/**
 * @file analog_cli.h
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef ANALOG_CLI_H
#define ANALOG_CLI_H

#define AIN_MIN_ARG_REQ                 7
#define AIN_MAX_ARG_REQ                 10

// #define PRESSURE                        0        
// #define V0_10_12V_supply                1                
// #define V0_10_24V_supply                2                
// #define VIBRATION_DC_BIAS               3                
// #define VIBRATION_AC_BIAS               4                
// #define MSUPPLY_VMON                    5        
// #define LOAD_CELL                       6        
// #define TDC_ANALOG                      7        
// #define MOTOR2_CS_B                     8        
// #define MOTOR1_CS_B                     9

//Redefining the types of sensor for ADC input
#define AI_420MA_PATH                   0        
#define AI_0_10V_PATH                   1                
#define AI_IEPE_PATH                    2                
#define AI_MSUPPLY_VMON                 3        
#define AI_LOAD_CELL                    4        
#define AI_TDC_ANALOG                   5        
#define AI_MOTOR2_CS_B                  6        
#define AI_MOTOR1_CS_B                  7

#define AI_IEPE_BIASE_NA                0
#define AI_IEPE_AC_BIASE                1
#define AI_IEPE_DC_BIASE                2


#define MAX_ANALOG_SENSORS_SUPP_CLI     7

#define MAX_POWER_SUPP_CLI     3
#define MIN_POWER_SUPP_CLI     0


#define AIN_MAX_ADC_SAMPLES_CNT         10000
#define AIN_ADC_DEF_SAMPLES_CNT         10000

#define DAC_DUAL_VIB_CH                 0
#define DAC_TDC_CH                      1

#define DUAL_VIB_MAX_DAC_OUT            10.0

#define AIN_MIN_SCAN_PERIOD             1
#define AIN_MAX_SCAN_PERIOD             60
#define AIN_MIN_SAMP_FREQ               0
#define AIN_MAX_SAMP_FREQ               100000

#define IEPE_DAC_OUT_AMP_GAIN            2

#define SHOW_RAW_SAMPLE_DATA             1
#define DONT_SHOW_RAW_SAMPLE_DATA        0


//

#define OUTPUT_POWER_SUPPLY_SELECT_NONE                    0
#define OUTPUT_POWER_SUPPLY_SELECT_420MA_24V               1
#define OUTPUT_POWER_SUPPLY_SELECT_12V                     2


#define SPI_FREQ_1M                     1000000
#define SPI_FREQ_2M                     2000000


#define MIN_VOLT_SCALE                  0
#define MAX_VOLT_SCALE                  5
#define MIN_CURRENT                     4.0f
#define MAX_CURRENT                     20.0f
#define MAX_ADC_CNT                     65535

#define AI_SCALING_FACTOR(t, v) ((t == AI_420MA_PATH)?  (float)(((float)v * ( MAX_VOLT_SCALE / MAX_ADC_CNT))):\
                         (t == AI_0_10V_PATH)? v:\
                         (t == AI_IEPE_PATH)? v: v\
                        )


typedef struct {
    //Flags
    uint8_t show_raw_samples;
    uint8_t send_raw_samples : 1;
    uint8_t request_tcp:1;

    //Sensor selection configuration
    uint8_t last_power_selected;
    uint8_t IO_channel;                 // Input and output channel selection option 1-4
    uint8_t sensor_type;                // Input sensor type connected to ADC for reading
    uint8_t ai_pwr_supply_src_sel;            // Output Power type select [0 - None, 1- 4-20mAm, 2. 24V, 3 - 12 V]
    uint8_t IEPE_bias_type;             // IEPE Bias type only in case sensor_type is selected as IEPE
    float IEPE_Vref;                    // IEPE Vref only in case of sensor_type is selected as IEPE

    uint16_t max_samp;
    uint16_t min_samp;
    uint16_t avg_cnt;
    int32_t scan_period;
    uint32_t samp_cnt;
    uint32_t sampling_rate;
    uint32_t settling_time_ms;
    float dv_dac_out;
} st_ain_sample_t;

extern st_ain_sample_t st_ain_sample;
void ain_help(void);
void ain_cmd(int argc, char **argv, void *additional_arg);
int ain_periodic_callback(char *ptr, void *args);
int read_analog_in(st_ain_sample_t *pst_ain_sample,  uint16_t *buff);
void set_tdc_vref(float iepe_Vref);

#endif