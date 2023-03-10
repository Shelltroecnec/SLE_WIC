#ifndef PWM_CLI_H
#define PWM_CLI_H

#define PWM_MIN_ARG_REQ                 3
#define PWM_MAX_ARG_REQ                 3


void pwm_help(void);
void pwm_cmd(int argc, char **argv, void *additional_arg);

#endif