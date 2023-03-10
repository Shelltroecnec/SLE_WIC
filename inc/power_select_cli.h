#ifndef POWER_SELECT_CLI_H
#define POWER_SELECT_CLI_H

#define PWR_GPIO                    128
#define PWRSEL_MIN_ARG_REQ          2
#define PWRSEL_MAX_ARG_REQ          2

int pwr_select_cb(int pwr_sel);
void pwrsel_help(void);
void pwrsel_cmd(int argc, char **argv, void *additional_arg);

#endif