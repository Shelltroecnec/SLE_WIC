#include "headers.h"

int simcard_operation(int option)
{
    serial_t *serial;
    int ret;
    uint8_t simconnectivity[] = "AT+cpin?";
    uint8_t netconnectivity[] = "AT+CIPPING=\"www.google.com\"";
    uint8_t receive[128];
    serial = serial_new();
    if (option == SIM_CONNECTIVITY)
    {
        if (serial_open(serial, config.ser_simcard_dev, SIM_UART_BAUDRATE) < 0)
        {
            fprintf(stderr, "serial_open(): %s\n", serial_errmsg(serial));
            exit(1);
        }
        if (serial_write(serial, simconnectivity, sizeof(simconnectivity)) < 0)
        {
            fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
            exit(1);
        }
        if ((ret = serial_read(serial, receive, 256, 2000)) < 0)
        {
            fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
            exit(1);
        }

        CLOG("\nResponse received[%d]: ", ret);
        if (strcmp((const char *)receive, "+CPIN: READY") == 0)
        {
            CLOG("Result: Simcard connectivity Test Pass\n");
        }
        else
        {
            CLOG("Result: Simcard connectivity Test Fail\n");
        }
    }
    else if (option == NET_CONNECTIVITY)
    {
        if (serial_open(serial, config.ser_simcard_dev, SIM_UART_BAUDRATE) < 0)
        {
            fprintf(stderr, "serial_open(): %s\n", serial_errmsg(serial));
            exit(1);
        }
        if (serial_write(serial, netconnectivity, sizeof(netconnectivity)) < 0)
        {
            fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
            exit(1);
        }
        if ((ret = serial_read(serial, receive, 256, 2000)) < 0)
        {
            fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
            exit(1);
        }

        CLOG("\nResponse received[%d]: ", ret);
        if (strcmp((const char *)receive, "OK") == 0)
        {
            CLOG("Result: NET connectivity Test Pass\n");
        }
        else
        {
            CLOG("Result: NET connectivity Test Fail\n");
        }
    }
}

void simcard_help(void)
{
    CLOG("SIM command Usage:\n");
    CLOG("   %s -c SIM [Option]\n", exec_filename);
    CLOG("      Option: Select which operation to perform\n");
    CLOG("      0: Validate Sim connectivity\n");
    CLOG("      1: Validate Net connectivity\n");
}

void simcard_cmd(int argc, char **argv, void *additional_arg)
{
    if (argc < SIM_MIN_ARG_REQ)
    {
        CLOG("[SIM] Less number of argument/s provided for SER Cmd!!\n");
        mod_help();
        return;
    }

    int option = atoi(argv[1]);

    simcard_operation(option);
}