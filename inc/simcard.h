#ifndef SIMCARD_CLI_H
#define SIMCARD_CLI_H

#define SIM_MIN_ARG_REQ                 2
#define SIM_MAX_ARG_REQ                 2

#define SIM_CONNECTIVITY                0
#define NET_CONNECTIVITY                1

#define SIM_UART_BAUDRATE               115200

int simcard_operation(int option);
void simcard_help(void);
void simcard_cmd(int argc, char **argv, void *additional_arg);

#endif