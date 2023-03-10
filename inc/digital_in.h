#ifndef DIGITAL_IN_H
#define DIGITAL_IN_H


#define DIN_MIN_ARG_REQ         2
#define DIN_MAX_ARG_REQ         2

#define MAX_CLI_DIN_OP_PRD      60
#define MIN_CLI_DIN_OP_PRD      1

void din_help(void);
void din_cmd(int argc, char **argv, void *additional_arg);
void *digital_monitoring_thread(void *vargp);
int din_periodic_callback(char *ptr, void *args);
void loopfeature(int scn_prd);
#endif