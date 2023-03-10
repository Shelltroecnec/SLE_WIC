#ifndef SERIAL_CLI_H
#define SERIAL_CLI_H

#define SER_MIN_ARG_REQ                 2
#define SER_MAX_ARG_REQ                 2

#define SER_RS232_MODE                  0
#define SER_RS485_MODE                  1

#define RS485_TRANSMIT_DIR              1
#define RS485_RECEIVE_DIR               0

#define RS232_SPEED_250KBPS             0
#define RS232_SPEED_1MBPS               1

#define SER_CHANNEL_1                   1
#define SER_CHANNEL_2                   2
#define SER_CHANNEL_3                   3


void ser_cmd(int argc, char **argv, void *additional_arg);
void ser_help(void);
void ser_enable_channel(char mode, char dir_speed);


#endif