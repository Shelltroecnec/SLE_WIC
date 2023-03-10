

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "headers.h"

/**
 * @brief Ram Cli helper text
 * 
 */
void imx_ram_help(void) {
    CLOG("RAM command Usage:\n");
    CLOG("   %s -c RAM \n", exec_filename);
    CLOG("       This CLI will write allocate and write 100 bytes in memory and read it back verify data integrity\n");
}

/**
 * @brief                   RAM CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void imx_ram_cmd(int argc, char **argv, void *additional_arg)
{
    if (argc < IMX_RAM_MIN_ARG_REQ) {
        imx_ram_help();
        return;
    }
    int* ptr = NULL;                                                // This pointer will hold the base address of the block created
    int index = 0;                                                  
    ptr = (int*)malloc(MAX_BYTE_DYNAMIC_ALLOCATE * sizeof(int));    // Dynamically allocate memory using malloc()
    if (ptr == NULL) {                                              // Check if the memory has been successfully allocated by malloc or not
        CLOG("Memory not allocated, RAM test Fail\n");
        return;
    } else {
        // Memory has been successfully allocated
        CLOG("Memory successfully allocated using malloc.\n");

        // populate memory with ascending numbers from 0 to 99
        for (index = 0; index < MAX_BYTE_DYNAMIC_ALLOCATE; ++index) {
            ptr[index] = index;
        }
 
        // read and compare memory
        for (index = 0; index < MAX_BYTE_DYNAMIC_ALLOCATE; ++index) {
            if (ptr[index] != index) {
                CLOG("Result: RAM Test Fail\n");
                return;
            }
        }
        CLOG("Result: RAM Test Pass\n");
    }
}
