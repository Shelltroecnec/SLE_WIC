#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "headers.h"


/**
 * @brief Write to check qspi flash memory
 * 
 * @param mtd_info 
 * @param fd File descriptor
 * @param buf character pointer to a buffer
 * @param size size of buffer
 * @param sector size of sector
 * @param offset offset address
 * @return bytes written
 */
int write_flash(mtd_info_t *mtd_info, int fd, char *buf, int size, int sector, int offset){

    int rbytes = 0;

    if(sector * mtd_info->erasesize > mtd_info->size && ((uint32_t)(offset + size) >= mtd_info->erasesize)){

       return rbytes; 
    }
    /* read something from last sector */
    // unsigned char buf[64];
    lseek(fd, sector * mtd_info->erasesize + offset, SEEK_SET);
    rbytes = write(fd, buf, size);
    return rbytes;
}

/**
 * @brief Read to check qspi flash memory
 * 
 * @param mtd_info 
 * @param fd File descriptor
 * @param buf character pointer to a buffer
 * @param size size of buffer
 * @param sector size of sector
 * @param offset offset address
 * @return bytes read
 */
int read_flash(mtd_info_t *mtd_info, int fd, char *buf, int size, int sector, int offset){

    int rbytes = 0;

    if(sector * mtd_info->erasesize > mtd_info->size && ((uint32_t)(offset + size) > mtd_info->erasesize)){

       return rbytes; 
    }
    /* read something from last sector */
    // unsigned char buf[64];
    lseek(fd, sector * mtd_info->erasesize + offset, SEEK_SET);
    rbytes = read(fd, buf, size);
    return rbytes;
}

/**
 * @brief Erase a sector in qspi flash memory
 * 
 * @param mtd_info structure
 * @param fd    file descriptor
 * @param sector sector size
 */
void erase_sector(mtd_info_t *mtd_info, int fd, uint16_t sector) {
    erase_info_t ei;
    ei.length = mtd_info->erasesize;
    ei.start = sector * mtd_info->erasesize;
    ioctl(fd, MEMUNLOCK, &ei);
    ioctl(fd, MEMERASE, &ei);
}

/**
 * @brief               Erase a partition in qspi flash memory
 * 
 * @param mtd_info 
 * @param fd            file descriptor
 */
void erase_partition(mtd_info_t *mtd_info, int fd) {
    erase_info_t ei;
    ei.length = mtd_info->erasesize;

    for(ei.start = 0; ei.start < mtd_info->size; ei.start += mtd_info->erasesize) {
        ioctl(fd, MEMUNLOCK, &ei);
        ioctl(fd, MEMERASE, &ei);
    }
}

/**
 * @brief                Qspi flash helper function
 * 
 */
void qspi_flash_help(void) {
    CLOG("QSPIF command Usage:\n");
    CLOG("   %s -c QSPIF [MODE]\n", exec_filename);
    CLOG("       MODE - Test mode\n");
    CLOG("            0 - Read only\n");
    CLOG("            1 - Write and Read QSPI flash\n");
    CLOG("       This CLI will write some data on QSPI flash and read it back to see if it is working\n");
}

/**
 * @brief                   Qspi flash CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void qspi_flash_cmd(int argc, char **argv, void *additional_arg)
{
    char buff[] = "smartIO QSPI flash Test";
    char rbuff[100] = {0};
    int read_only = 0;
    if (argc < QSPIF_MIN_ARG_REQ) {
        qspi_flash_help();
        return;
    }

    read_only = atoi(argv[1]);

    mtd_info_t mtd_info;
    int fd = open(config.qspi_flash_dev, O_RDWR);
    ioctl(fd, MEMGETINFO, &mtd_info);
 
    // ILOG("MTD type: %u\n", mtd_info.type);
    // ILOG("MTD total size : %u bytes\n", mtd_info.size);
    // ILOG("MTD erase size : %u bytes\n", mtd_info.erasesize);

    if(read_only) {
        erase_sector(&mtd_info, fd, 0);                             //Erase first sector
        write_flash(&mtd_info, fd, buff, strlen(buff), 0, 0);       //Write some dummy data
        DLOG("Write Data: %s\r\n", buff);
    }
    read_flash(&mtd_info, fd, rbuff, strlen(buff), 0, 0);           //Read dummy data

    DLOG("Read Data: %s\r\n", rbuff);

    if(strcmp(buff,rbuff) == 0){
        CLOG("Result: QSPI Test Pass\n");
    } else {
        CLOG("Result: QSPI Test Fail\n");
    }
}


