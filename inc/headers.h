/**
 * @file headers.h
 * @author your name (you@domain.com)
 * @brief   This file has global headers includes and MACRO definations
 * @version 0.1
 * @date 2021-03-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef HEADERS_H
#define HEADERS_H
#include <getopt.h>
#include <sys/stat.h>   // stat
#include <stdint.h>         /* C99 types */
#include <stdbool.h>        /* bool type */
#include <stdio.h>          /* ILOG, fprintf, snprintf, fopen, fputs */
// #include <inttypes.h>       /* PRIx64, PRIu64... */
#include <string.h>         /* memset */
#include <signal.h>         /* sigaction */
#include <time.h>           /* time, clock_gettime, strftime, gmtime */
#include <sys/time.h>       /* timeval */
#include <unistd.h>         /* getopt, access */
#include <stdlib.h>         /* atoi, exit */
#include <errno.h>          /* error messages */
#include <math.h>           /* modf */
#include <dirent.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>


# include <sys/ipc.h> 
# include <sys/shm.h> 
# include <sys/types.h> 
#include <fcntl.h> 
#include <sys/mman.h>

#define SHARED_MEMORY_OBJECT  "/smartio_cli_shm" 
#define SIZE_SHARED_MEMORY    512 //Took randomly  
# define IPC_KEY 999
#define READ_SHM_MEMORY                 1  
#define WRITE_SHM_MEMORY                 2  

//Local header
#include "cli_opt_parser.h"
#include "vector.h"
#include "cJSON.h"
#include "config.h"
#include "gpio_cli.h"
#include "ioexpander.h"
#include "i2c.h"
#include "spi.h"
#include "serial.h"
#include "analog_cli.h"
#include "rtd_cli.h"
#include "timer.h"
#include "digital_in.h"
#include "tools.h"
#include "relay_cli.h"
#include "dac_cli.h"
#include "sd_card_cli.h"
#include "serial_cli.h"
#include "modbus_cli.h"
#include "nordic.h"
#include "motor_cli.h"
#include "analog_out_cli.h"
#include "version.h"
#include "qspi_flash.h"
#include "emmc_cli.h"
#include "ram_cli.h"
#include "usb_cli.h"
#include "tdc_cli.h"
#include "tcp_server.h"
#include "frame_parser.h"
#include "max31865.h"
#include "mqtt_app.h"
#include "shared_mem.h"
#include "timer_event.h"
#include "pwm_cli.h"
//#include "ltc2634.h"
#include "mcp48fvb22.h"
#include "power_select_cli.h"
#include "simcard.h"
#include "lvds.h"
#include "digital_out.h"
#include "rtc.h"
#include "flash.h"
#include "W25Q32.h"
//#include "ADS7038.h"

/* GPIO Defination */
#define SET                             1
#define RESET                           0


enum log_levels{
    CLI_LOG,
    ERROR_LOG,
    WARN_LOG,
    INFO_LOG,
    DEBUG_LOG
};

// #define DEBUG

#define MAX_LOG_LEVELS 10

#ifdef DEBUG




#define CLOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= CLI_LOG) fprintf(stderr, "[%s] " fmt, ts, ##__VA_ARGS__); } while (0)
#define ELOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= ERROR_LOG) fprintf(stderr, "[%s] " fmt, ts, ##__VA_ARGS__); } while (0)                                
#define WLOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= WARN_LOG) fprintf(stdout, "[%s] " fmt, ts, ##__VA_ARGS__); } while (0)

#define ILOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= INFO_LOG) fprintf(stdout, "[%s] " fmt, ts, ##__VA_ARGS__); } while (0)
#define DLOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= DEBUG_LOG) fprintf(stdout, "[%s] " fmt, ts, ##__VA_ARGS__); } while (0)  

/*
#define CLOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= CLI_LOG) fprintf(stderr, "[%s] %s:%d:%s(): " fmt, ts, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
#define ELOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= ERROR_LOG) fprintf(stderr, "[%s] %s:%d:%s(): " fmt, ts, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)                                
#define WLOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= WARN_LOG) fprintf(stdout, "[%s] %s:%d:%s(): " fmt, ts, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define ILOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= INFO_LOG) fprintf(stdout, "[%s] %s:%d:%s(): " fmt, ts, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
#define DLOG(fmt, ...) \
        do { getCurrentTimeMsStr(ts); if (config.log_level >= DEBUG_LOG) fprintf(stdout, "[%s] %s:%d:%s(): " fmt, ts, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)                                
                                */
#else
#define CLOG(fmt, ...)      do { if (config.log_level >= CLI_LOG  ) \
                                fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)
#define ELOG(fmt, ...)      do { if (config.log_level >= ERROR_LOG) \
                                fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)
#define WLOG(fmt, ...)      do { if (config.log_level >= WARN_LOG ) \
                                fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)
#define ILOG(fmt, ...)      do { if (config.log_level >= INFO_LOG ) \
                                fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)
#define DLOG(fmt, ...)      do { if (config.log_level >= DEBUG_LOG) \
                                fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)

#endif


extern char *exec_filename;

#endif