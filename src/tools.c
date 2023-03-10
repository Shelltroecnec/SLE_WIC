/**
 * @file tools.c
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 *  
 */

#include "headers.h"

/**
 * @brief 			This function is used to execute a callback function periodically 
 * 					until there are no input enter from the keyboard or 'q'or 'Q' keypressed	
 * 
 * @param ptv 		struct timeval for setting the periodicity in sec and/or microseconds
 * @param callback  function to be call periodically
 * @param args 		variable to be passed to the callback function, passed from the caller
 * 
 * TODO: each CLI should create a thread and call this function in thread for processing multiple CLI simultenously
 */
void check_stdin_cb(struct timeval *ptv, int (*callback)(char *, void *), void *args) {
    fd_set rfds;
    // char isFlush = 0;
    int stop = 0;
    ;
    struct timeval tv, *psttv = ptv;
    int retval, num;
    int readfd = fileno(stdin);
    char buf[1024], *toCallback = NULL;

    do {
        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(readfd, &rfds); //Set STDIN

        //Below code is added because tv value is not valid after we call select function,
        // Hence created a local struct variable and pointer to save the passed struct
        // local pointer is used to have a feature of passing a NULL pointer to select function for infinitely waiting for STDIN
        if (ptv != NULL) {
            tv.tv_sec = ptv->tv_sec;
            tv.tv_usec = ptv->tv_usec;
            psttv = &tv;
            ILOG("Set Period: %ld.%ld\n", ptv->tv_sec, ptv->tv_usec);
        }

        retval = select(1, &rfds, NULL, NULL, psttv);
        /* Don't rely on the value of tv now! */

        if (retval == -1) {
            WLOG("select() fail\r\n");
            toCallback = NULL;
        } else if (retval) {
            num = read(readfd, buf, sizeof(buf));
            // ILOG_D"number of bytes read: %d\n", num );
            if (num > 0) {
                DLOG("Keyboard Input[%d]: %.*s\n", num, num, buf);
                toCallback = buf;
            }
        } else {
            DLOG("No Input/Event\n");
            toCallback = NULL;
        }
        // int cb_return;
        //callback here
        stop = (*callback)(toCallback, args);
        memset(buf, 0, sizeof(buf));
        DLOG("callback return %d\n", stop);
    } while (stop == 0);
}

/**
 * @brief 		conver hex string to int
 * 
 * @param s 	String pointer (it should be valid hex string)
 * @return int  return a integer 
 */
int htoi(char *s) {
    char *t = s;
    int x = 0, y = 1;
    if (*s == '0' && (s[1] == 'x' || s[1] == 'X'))
        t += 2;
    s += strlen(s);
    while (t <= --s) {
        if ('0' <= *s && *s <= '9')
            x += y * (*s - '0');
        else if ('a' <= *s && *s <= 'f')
            x += y * (*s - 'a' + 10);
        else if ('A' <= *s && *s <= 'F')
            x += y * (10 + *s - 'A');
        else
            return -1; /* invalid input! */
        y <<= 4;
    }
    return x;
}

/**
 * @brief 			Map a value in range of two value. i.e current mapping from 4-20mA to voltage range 0-25V
 * 
 * @param value		Input value to be mapped
 * @param in_min 	Minimum value for input unit
 * @param in_max 	Maximum value for input unit
 * @param out_min   Minimum value for output unit
 * @param out_max   Maximum value for output unit
 * @return float 	returns the mapped value in voltage or current 
 */
float map(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief  this function converts data(of any integral type) to binary
 * 
 * @param data 		pointer to data
 * @param len 		Number of bytes to be converted to binary
 * @param rgucbits 	array of char to store the binary data, it Must be greater than 40 bytes( or len*8 + len +3)
 */
void byte_to_bits(char *data, char len, char *rgucbits) {
    int i = 0, j = 0;
    int bitcount = 0;
    rgucbits[bitcount++] = 'b';
    rgucbits[bitcount++] = '\'';
    for (j = len - 1; j >= 0; j--) {
        for (i = 0; i < 8; i++) {
            rgucbits[bitcount++] = '0' + (((data[j] >> (7 - i)) & 1) > 0);
        }
        rgucbits[bitcount++] = ' ';
    }
    rgucbits[bitcount++] = '\0';
}

/**
 * @brief               average a float array/samples
 * 
 * @param samples       pointer to number of float samples 
 * @param sample_cnt    number of samples to be averaged
 * @return float        average float value
 */
float avg_samples_float(float *samples, uint8_t sample_cnt, float *max_samp, float *min_samp) {
    uint8_t sample_index = 0;
    float sample_sum = 0, sample_avg = 0;

    for (sample_index = 0; sample_index < sample_cnt; sample_index++) {
        sample_sum = samples[sample_index] + sample_sum;
        if (*max_samp < samples[sample_index])
            *max_samp = samples[sample_index];
        if (*min_samp > samples[sample_index])
            *min_samp = samples[sample_index];
    }
    sample_avg = sample_sum / sample_cnt;

    return sample_avg;
}

/**
 * @brief               Average the uint16_t samples
 * 
 * @param samples       pointer to sample array
 * @param sample_cnt    no. of samples to be averaged
 * @return uint16_t     return average value
 */
uint16_t avg_samples_integer(uint16_t *samples, uint16_t sample_cnt, uint16_t *max_samp, uint16_t *min_samp) {
    uint16_t sample_index = 0;
    uint32_t sample_sum = 0;
    uint16_t sample_avg = 0;

    *max_samp = 0;
    *min_samp = 0xffff;
    for (sample_index = 0; sample_index < sample_cnt; sample_index++) {
        sample_sum = samples[sample_index] + sample_sum;
        if (*max_samp < samples[sample_index])
            *max_samp = samples[sample_index];
        if (*min_samp > samples[sample_index])
            *min_samp = samples[sample_index];
    }
    sample_avg = sample_sum / sample_cnt;

    return sample_avg;
}

bool file_exists(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}


void loopfeature(int scn_prd){
    CLOG("       Use -l to Loop continously every %d seconds\n", scn_prd);
    CLOG("       Note: Use -l before -c\n");
}

int getRandom(int lower, int upper){
    
    int num = (rand() % (upper - lower + 1)) + lower;
    // DLOG("%d ", num);
    return num;
}

/**
 * @brief execute system commands and give the output of the command on pipe 
 * 
 * @param command  
 * @param result 
 * @return int 
 */
int exec_sys(const char *command, char *result) {
   char buffer[128];
   int total_rbytes = 0;

   // Open pipe to file
   FILE* pipe = popen(command, "r");
   if (!pipe) {
      return -1;
   }


   // read till end of process:
   while (!feof(pipe)) {
      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL){
        memcpy(&result[total_rbytes], buffer, strlen(buffer)+1);
        total_rbytes += (strlen(buffer));
      }
   }

   pclose(pipe);
   return total_rbytes;
}

uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

/**
 * @brief                   Print ascii in hex format
 * 
 * @param byte_array        character pointer to byte array
 * @param byte_array_size   size of byte array
 */
void print_byte_array(unsigned char *byte_array, int byte_array_size)
{
    int i = 0;
    CLOG("0x");
    for(; i < byte_array_size; i++)
    {
        CLOG("%02x", byte_array[i]);
    }
    CLOG("\n");
}

/**
 * @brief                   convert ascii to hex
 * 
 * @param hex_str           Ascii string to be converted to hex
 * @param byte_array        char pointer where the hex string will be stored
 * @param byte_array_max    max length of byte array
 * @return int              hex convered array length, -1 on failure
 */
int convert_byte_array(const char *hex_str, unsigned char *byte_array, int byte_array_max)
{
    int hex_str_len = strlen(hex_str);
    int i = 0, j = 0;

    // The output array size is half the hex_str length (rounded up)
    int byte_array_size = (hex_str_len+1)/2;

    if (byte_array_size > byte_array_max)
    {
        // Too big for the output array
        return -1;
    }

    if (hex_str_len % 2 == 1)
    {
        // hex_str is an odd length, so assume an implicit "0" prefix
        if (sscanf(&(hex_str[0]), "%1hhx", &(byte_array[0])) != 1)
        {
            return -1;
        }

        i = j = 1;
    }

    for (; i < hex_str_len; i+=2, j++)
    {
        if (sscanf(&(hex_str[i]), "%2hhx", &(byte_array[j])) != 1)
        {
            return -1;
        }
    }

    return byte_array_size;
}


// void main()
// {
//     char *examples[] = { "", "5", "D", "5D", "5Df", "deadbeef10203040b00b1e50", "02invalid55" };
//     unsigned char byte_array[128];
//     int i = 0;

//     for (; i < sizeof(examples)/sizeof(char *); i++)
//     {
//         int size = convert(examples[i], byte_array, 128);
//         if (size < 0)
//         {
//             DLOG("Failed to convert '%s'\n", examples[i]);
//         }
//         else if (size == 0)
//         {
//             DLOG("Nothing to convert for '%s'\n", examples[i]);
//         }
//         else
//         {
//             print(byte_array, size);
//         }
//     }
// }
// int64_t getCurrentTimeMs(void)
// {
// 	struct timeval tp;
// 	gettimeofday(&tp, NULL);
// 	// tp.tv_sec += 19800; // For IST timestamp
// 	int64_t now = (int64_t)tp.tv_sec * 1000LL + tp.tv_usec / 1000; //get current timestamp in milliseconds
// 	return now;
// }

// int64_t GetCurrentTimeUs(void)
// {
// 	struct timeval tp;
// 	gettimeofday(&tp, NULL);
// 	int64_t now = (int64_t)tp.tv_sec * 1000000LL + tp.tv_usec; //get current timestamp in microseconds
// 	return now;
// }


// //char ts[30];

// void getCurrentTimeMsStr(char *str)
// {
//     int64_t epochTimeMs = getCurrentTimeMs();
//     time_t rawtime = epochTimeMs/1000; // time to seconds
//     struct tm  ts;
//     char buf[35];

//     // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
//     ts = *localtime(&rawtime);
//     strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &ts);
//     sprintf(str,"%s.%03dZ",buf,epochTimeMs%1000); //Add milisecond time as well
// }

