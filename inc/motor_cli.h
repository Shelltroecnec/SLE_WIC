#ifndef MOTOR_CLI_H
#define MOTOR_CLI_H

#define MIN_MOT_SCAN_PRD                       1
#define MAX_MOT_SCAN_PRD                       60

// #define MOTOR2_CS_B                            6        
// #define MOTOR1_CS_B                            7

#define MIN_MOT_CONST_CURR_MUL_FACT            1.0
#define MAX_MOT_CONST_CURR_MUL_FACT            10.0

#define MOT_MIN_ARG_REQ                         3
#define MOT_MAX_ARG_REQ                         3

#define CLOCKWISE_DIR                           0
#define COUNTER_CLOCKWISE_DIR                   1
#define STOP_MOTOR_ROTATION                     2

#define MOTOR_1                                 1
#define MOTOR_2                                 2

void motor_help(void);
void motor_cmd(int argc, char **argv, void *additional_arg);
void motor_rotation_change(uint8_t motor, uint8_t dir);
#endif