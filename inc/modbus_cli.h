#ifndef MODBUS_CLI_H
#define MODBUS_CLI_H

#define MOD_MIN_ARG_REQ                 4
#define MOD_MAX_ARG_REQ                 4

// #define SER_RS232_MODE                  1
// #define SER_RS485_MODE                  2

#define RS485_TRANSMIT_DIR              1
#define RS485_RECEIVE_DIR               0

#define RX_TIMEOUT                      1000

// #define RS232_SPEED_250KBPS             0
// #define RS232_SPEED_1MBPS               1

#define SER_CHANNEL_1                   1
#define SER_CHANNEL_2                   2
#define SER_CHANNEL_3                   3

#define BAUDRATE_1                      9600
#define BAUDRATE_2                      115200

#define MODBUS_TIMEOUT                         2000

typedef struct {
    //Sensor selection configuration
    uint8_t Mod_channel;                 // Input and output channel selection option 1-4
    int tx_len;
    int rx_len;
    uint32_t baudrate;
    char TX_data[300]; 
    char RX_data[300]; 
} st_mod_sample_t;

void mod_cmd(int argc, char **argv, void *additional_arg);
void mod_help(void);
void mod_enable_channel(char channel, char dir_speed);
int read_modbus(st_mod_sample_t *st_mod_sample);

#endif