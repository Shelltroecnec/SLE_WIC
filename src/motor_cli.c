#include "headers.h"

extern bool loop;
/**
 * @brief       Motor driver helper function
 * 
 */
void motor_help(void) {
    CLOG("MOT command Usage:\n");
    CLOG("    %s -c MOT [MOT_SEL] [DIR]\n", exec_filename);
    CLOG("       MOT_SEL - Motor selection\n");
    CLOG("            1 - Select Motor 1\n");
    CLOG("            2 - Select Motor 2\n");
    CLOG("       DIR - MOT output channel selection:\n");
    CLOG("            0 - Clockwise\n");
    CLOG("            1 - Counter Clockwise\n");
    CLOG("            2 - Motor Off\n");
}

#define CH_UNKNOWN 0xFF


int motor_monitoring_cb(char *ptr, void *args) {
    uint8_t sense_ENA = 2;
    uint8_t sense_ENB = 2;
    // float read_current = 0.0f;
    // uint16_t adc_rx_buff[10];
    float motor_volt = 0, motor_current = 0;

    if (ptr != NULL) {
        CLOG("exiting from callback %s\n", ptr);
        return 1;
    }
    char motor_sel = *(char *)args;
    st_ain_sample_t st_ain_samples;
    //CLOG("continue... %ld\n", time(NULL));
    if (motor_sel == MOTOR_1) {
        sense_ENA = digital_pin_read(config.motor_nSLEEP1); //Read feedback to detect fault
        sense_ENB = digital_pin_read(config.motor_nSLEEP2); //Read feedback to detect fault
        st_ain_samples.sensor_type = (uint8_t)AI_MOTOR1_CS_B;
        //TODO: change the motor API as per the new implementation, commenting as of now
        //Read analog input current sense for motor 1
        // read_analog_in(pst_motor_samp, adc_rx_buff);
    } else if (motor_sel == MOTOR_2) {
        sense_ENA = digital_pin_read(config.motor_nSLEEP1); //Read feedback to detect fault
        sense_ENB = digital_pin_read(config.motor_nSLEEP2); //Read feedback to detect fault
        st_ain_samples.sensor_type = (uint8_t)AI_MOTOR2_CS_B;
        //Read analog input current sense for motor 2
        // read_analog_in(, CH_UNKNOWN, 1, SPI_FREQ_2M, adc_rx_buff);
    } else {
        CLOG("Invalid Motor selection in motor_monitoring_cb callback\n");
        return -1;
    }
    

    //Set up the structure for reading the motor current.....
    st_ain_samples.IO_channel = (uint8_t)CH_UNKNOWN;
    st_ain_samples.IEPE_bias_type = (uint8_t)AI_IEPE_BIASE_NA;
    st_ain_samples.IEPE_Vref = (float)0.0f;
    st_ain_samples.ai_pwr_supply_src_sel = (uint8_t)OUTPUT_POWER_SUPPLY_SELECT_NONE; 
    st_ain_samples.samp_cnt = 10;
    st_ain_samples.scan_period = (uint32_t)10;
    st_ain_samples.sampling_rate = 10000; // not yet used
    st_ain_samples.show_raw_samples = SHOW_RAW_SAMPLE_DATA;


    ain_periodic_callback(NULL, &st_ain_samples); 
    CLOG("ENA: %d,ENB: %d\n", sense_ENA, sense_ENB);
    motor_volt = (((float)(st_ain_samples.avg_cnt) / 65535) * 5);
    motor_current = motor_volt * config.mot_cli_iconst_mul;
    CLOG("Motor %d Bit Count = %d, Volt: %0.3f Volts, Current = %0.3f Amp\n", motor_sel, st_ain_samples.avg_cnt, motor_volt, motor_current);
    if (sense_ENA == 0) {
        CLOG("Motor %d Half Bridge A fault detected!!\n", motor_sel);
    }

    if (sense_ENB == 0) {
        CLOG("Motor %d Half Bridge B fault detected!!\n", motor_sel);
    }
    return 0;
}

/**
 * @brief           Change motor driver rotation
 * 
 * @param motor     Select motor number
 * @param dir       Direction to set
 */
void motor_rotation_change(uint8_t motor, uint8_t dir) {
    if (motor == MOTOR_1) {
        if (dir == CLOCKWISE_DIR) {
            digital_pin_out_write(config.motor_EN1, SET); //Enable the motor
            // digital_pin_out_write(config.motor_ENB1, SET); //Enable the motor

            //start clockwise rotation
            digital_pin_out_write(config.motor_PWMA1, SET);   //A HI
            digital_pin_out_write(config.motor_PWMB1, RESET); //B LO for clock wise rotation
        } else if (dir == COUNTER_CLOCKWISE_DIR) {
            digital_pin_out_write(config.motor_EN1, SET); //Enable the motor
            // digital_pin_out_write(config.motor_ENB1, SET); //Enable the motor

            //start  counter clockwise  rotation
            digital_pin_out_write(config.motor_PWMA1, RESET); //A LO
            digital_pin_out_write(config.motor_PWMA1, SET);   //B HI for clock wise rotation
        } else if (dir ==  STOP_MOTOR_ROTATION){                                            // stop
            digital_pin_out_write(config.motor_EN1, RESET); //Disable the motor
            // digital_pin_out_write(config.motor_ENB1, RESET); //Disable the motor

            digital_pin_out_write(config.motor_PWMA1, RESET); //A LO
            digital_pin_out_write(config.motor_PWMB1, RESET); //B HI for clock wise rotation
        }
    } else if (motor == MOTOR_2) {
        if (dir == CLOCKWISE_DIR) {
            // digital_pin_out_write(config.motor_ENA2, SET); //Enable the motor
            digital_pin_out_write(config.motor_EN2, SET); //Enable the motor

            //start clockwise rotation
            digital_pin_out_write(config.motor_PWMA2, SET);   //A HI
            digital_pin_out_write(config.motor_PWMB2, RESET); //B LO for clock wise rotation
        } else if (dir == COUNTER_CLOCKWISE_DIR) {
            // digital_pin_out_write(config.motor_ENA2, SET); //Enable the motor
            digital_pin_out_write(config.motor_EN2, SET); //Enable the motor

            //start counter clockwise  rotation
            digital_pin_out_write(config.motor_PWMA2, RESET); //A LO
            digital_pin_out_write(config.motor_PWMB2, SET);   //B HI for clock wise rotation
        } else if (dir == STOP_MOTOR_ROTATION) {                                             // stop
            // digital_pin_out_write(config.motor_ENA2, RESET); //Disable the motor
            digital_pin_out_write(config.motor_EN2, RESET); //Disable the motor

            digital_pin_out_write(config.motor_PWMA2, RESET); //A LO
            digital_pin_out_write(config.motor_PWMB2, RESET); //B HI for clock wise rotation
        }
    }
}

/**
 * @brief                   Motor driver CLI command
 * 
 * @param argc              (ARGument Count) is in  t and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void motor_cmd(int argc, char **argv, void *additional_arg) {
    // char ch = 0, ch1 = 0;
    if (argc < MOT_MIN_ARG_REQ) {
        // CLOG("[MOT] Less number of argument/s provided for MOT Cmd!!\n");
        motor_help();
        return;
    }

    //parsing the inputs
    int motor_sel = atoi(argv[1]);
    int direction = atoi(argv[2]);

    if ((motor_sel > MOTOR_2) || (motor_sel < MOTOR_1)) {
        CLOG("Invalid Motor selection!!!\n");
        motor_help();
        return;
    }

    if (direction < CLOCKWISE_DIR || direction > STOP_MOTOR_ROTATION) {
        CLOG("Invalid direction!!!\n");
        motor_help();
        return;
    }

    // CLOG("Need to implement the Motor CLI as per the new analog input\r\n");
   
    char Dir[50];
    if (direction == CLOCKWISE_DIR){
        strcpy(Dir,"Clockwise");
    }  else if (direction == COUNTER_CLOCKWISE_DIR){
        strcpy(Dir,"Counter-Clockwise");
    }   else if (direction == STOP_MOTOR_ROTATION){
        strcpy(Dir,"Stop Rotation");
    }
   
    CLOG("selected motor [%d] in %s direction\n", motor_sel, Dir);

    struct timeval tv;

   
    motor_rotation_change(motor_sel, direction);

   
    if (loop == true) {
        tv.tv_sec = config.cli_motor_scan_prd;
        tv.tv_usec = 0;
        check_stdin_cb(&tv, &motor_monitoring_cb, &motor_sel);
    }
    
    // motor_rotation_change(motor_sel, STOP_MOTOR_ROTATION);
}