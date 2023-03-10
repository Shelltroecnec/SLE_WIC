

#include "headers.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c.h"

#include <linux/i2c.h>
// I2C3_SCL gpio5.IO[18]
// I2C3_SDA gpio5.IO[19]

// i2cdetect -l
// i2c-1   i2c             30a30000.i2c                            I2C adapter
// i2c-2   i2c             30a40000.i2c                            I2C adapter
// i2c-0   i2c             30a20000.i2c                            I2C adapter
// root@imx8mnevk:~# ^C
// root@imx8mnevk:~#

// Start_Address End_Address NIC_Port   Size
// 30A5_0000       30A5_FFFF   I2C4     64KB
// 30A4_0000       30A4_FFFF   I2C3     64KB
// 30A3_0000       30A3_FFFF   I2C2     64KB
// 30A2_0000       30A2_FFFF   I2C1     64KB

// void I2CCmd(int argc, char **argv, void *additional_arg)
// {

//   if (argc < 5)
//   {
//     I2CHelper();
//     return;
//   }

//   int adapter_nr = atoi(argv[1]); //i2c number
//   int addr = atoi(argv[2]);       //slave address
//   int reg = atoi(argv[3]);        //register number
//   int value = atoi(argv[4]);      //value

//   char buf[2];
//   int file;
//   char filename[20];

//   sprintf(filename, "/dev/i2c-%d", adapter_nr);
//   file = open(filename, O_RDWR);
//   if (file < 0)
//   {
//     ILOG("File opening errror\n\r");
//     return;
//   }

//   /*Set the Slave address */
//   if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0)
//   {
//     perror("ioctl(I2C_RDWR) in i2c_write");
//   }

//   buf[0] = reg;
//   buf[1] = value;

//   /*Write the register and value into the I2C*/
//   if (write(file, buf, 2) != 2)
//   {
//     ILOG("Write error\n\r");
//   }
// }

/****
 * 
 * 
 * 
 */

/*
 * Start the I2C device.
 *
 * @param dev points to the I2C device to be started, must have filename and addr populated
 *
 * @return - 0 if the starting procedure succeeded
 *         - negative if the starting procedure failed
 */
int i2c_start(struct I2cDevice *dev) {
    int fd;
    int rc;

    /*
	 * Open the given I2C bus filename.
	 */
    fd = open(dev->filename, O_RDWR);
    if (fd < 0) {
        rc = fd;
        ILOG("Fail to open [%s] device\n", dev->filename);
        return rc;
    }

    // set i2c-1 retries time
    if (ioctl(fd, I2C_RETRIES, 1) < 0) {
        close(fd);
        fd = 0;
        ILOG("set i2c retry fail!\n");
        return -1;
    }

    // set i2c-1 timeout time, 10ms as unit
    if (ioctl(fd, I2C_TIMEOUT, 1) < 0) {
        close(fd);
        fd = 0;
        ILOG("set i2c timeout fail!\n");
        return -1;
    }
    /*
	 * Set the given I2C slave address.
	 */
    //NOTE: no need to add the below line of code in case we are using i2cWriteBuff and i2cReadBuff(as these functions are using i2c_rdwr_ioctl_data structure)
    // rc = ioctl(fd, I2C_SLAVE_FORCE, dev->addr); //As I2C_SLAVE flag was not affecting the address hence added I2C_SLAVE_FORCE flag which will set the ADDR
    // if (rc < 0)
    // {
    //   ILOG("Fail to SET I2C_SLAVE flag on [%s] device\n", dev->filename);
    //   close(fd);
    // }

    dev->fd = fd;

    return 0;
}

/*
 * Read data from the I2C device.
 *
 * @param dev points to the I2C device to be read from
 * @param buf points to the start of buffer to be read into
 * @param buf_len length of the buffer to be read
 *
 * @return - number of bytes read if the read procedure succeeded
 *         - 0 if no bytes were read
 *         - negative if the read procedure failed
 */
int i2c_read(struct I2cDevice *dev, uint8_t *buf, size_t buf_len) {
    return read(dev->fd, buf, buf_len);
}

/*
 * Write data to the I2C device.
 *
 * @param dev points to the I2C device to be write to
 * @param buf points to the start of buffer to be written from
 * @param buf_len length of the buffer to be written
 * @return - number of bytes written if the write procedure succeeded
 *         - 0 if no bytes were written
 *         - negative if the read procedure failed
 */
int i2c_write(struct I2cDevice *dev, uint8_t *buf, size_t buf_len) {
    return write(dev->fd, buf, buf_len);
}

/*
 * Read data from a register of the I2C device.
 *
 * @param dev points to the I2C device to be read from
 * @param reg the register to read from
 * @param buf points to the start of buffer to be read into
 * @param buf_len length of the buffer to be read
 *
 * @return - number of bytes read if the read procedure succeeded
 *         - 0 if no bytes were read
 *         - negative if the read procedure failed
 */
int i2c_readn_reg(struct I2cDevice *dev, uint8_t reg, uint8_t *buf, size_t buf_len) {
    int rc;

    /*
	 * Write the I2C register address.
	 */
    rc = i2c_write(dev, &reg, 1);
    if (rc <= 0) {
        ILOG("%s: failed to write i2c register address\r\n", __func__);
        return rc;
    }

    /*
	 * Read the I2C register data.
	 */
    rc = i2c_read(dev, buf, buf_len);
    if (rc <= 0) {
        ILOG("%s: failed to read i2c register data\r\n", __func__);
        return rc;
    }

    return rc;
}

/*
 * Write data to the register of the I2C device.
 *
 * @param dev points to the I2C device to be written to
 * @param reg the register to write to
 * @param buf points to the start of buffer to be written from
 * @param buf_len length of the buffer to be written
 *
 * @return - number of bytes written if the write procedure succeeded
 *         - 0 if no bytes were written
 *         - negative if the write procedure failed
 */
int i2c_writen_reg(struct I2cDevice *dev, uint8_t reg, uint8_t *buf, size_t buf_len) {
    uint8_t *full_buf;
    int full_buf_len;
    int rc;
    int i;

    /*
	 * Allocate a buffer that also contains the register address as
	 * the first element.
	 */
    full_buf_len = buf_len + 1;
    full_buf = malloc(sizeof(uint8_t) * full_buf_len);

    full_buf[0] = reg;
    for (i = 0; i < buf_len; i++) {
        full_buf[i + 1] = buf[i];
    }

    /*
	 * Write the I2C register address and data.
	 */
    rc = i2c_write(dev, full_buf, full_buf_len);
    if (rc <= 0) {
        ILOG("%s: failed to write i2c register address and data\r\n", __func__);
        goto fail_send;
    }

    free(full_buf);
    return 0;

fail_send:
    free(full_buf);
    return rc;
}

/*
 * Read value from a register of the I2C device.
 *
 * @param dev points to the I2C device to be read from
 * @param reg the register to read from
 *
 * @return - the value read from the register
 *         - 0 if the read procedure failed
 */
uint8_t i2c_read_reg(struct I2cDevice *dev, uint8_t reg) {
    uint8_t value = 0;
    i2c_readn_reg(dev, reg, &value, 1);
    return value;
}

/*
 * Write value to the register of the I2C device.
 *
 * @param dev points to the I2C device to be written to
 * @param reg the register to write to
 * @param value the value to write to the register
 *
 * @return - number of bytes written if the write procedure succeeded
 *         - 0 if no bytes were written
 *         - negative if the write procedure failed
 */
int i2c_write_reg(struct I2cDevice *dev, uint8_t reg, uint8_t value) {
    return i2c_writen_reg(dev, reg, &value, 1);
}

/*
 * Mask value of to a register of the I2C device.
 *
 * @param dev points to the I2C device to be written to
 * @param reg the register to write to
 * @param mask the mask to apply to the register
 *
 * @return - number of bytes written if the write procedure succeeded
 *         - 0 if no bytes were written
 *         - negative if the write procedure failed
 */
int i2c_mask_reg(struct I2cDevice *dev, uint8_t reg, uint8_t mask) {
    uint8_t value = 0;
    int rc;

    value = i2c_read_reg(dev, reg);
    value |= mask;

    rc = i2c_write_reg(dev, reg, value);
    if (rc <= 0) {
        return rc;
    }

    return 0;
}

/**
 * @brief This function writes the n bytes on I2C bus
 * 
 * @param dev       I2C device
 * @param sendbuf   Array of uint8_t, please note first byte should be register address,
 *                  as this buffer will be sent as it is after Command/Slave address (i.e first byte on I2C bus)
 * @param buf_len   Number of bytes to be transfer over I2C bus Exxluding Slave Address (or command byte)
 * @return int 
 */
int i2cWriteBuff(struct I2cDevice *dev, uint8_t reg, uint8_t *sendbuf, size_t buf_len) {

    struct i2c_rdwr_ioctl_data msgset;
    struct i2c_msg msgs[1];
    uint8_t rgucWrBuff[256] = {0};
    uint8_t rc = 0;
    rgucWrBuff[0] = reg;
    memcpy((uint8_t *)&rgucWrBuff[1], sendbuf, buf_len);
    //write I2C reg
    msgs[0].len = buf_len + 1; //Number of bytes transfered on I2C bus +1 for register address
    msgs[0].addr = dev->addr;  //i2c device address
    msgs[0].flags = 0;         //flags is 0: indicates write; 1: indicates read.
    msgs[0].buf = rgucWrBuff;  //first byte should be the register address (based on the datasheet of the device)
    msgset.nmsgs = 1;          //number of messages
    msgset.msgs = msgs;        //messages

    if (rc = ioctl(dev->fd, I2C_RDWR, (unsigned long)&msgset) < 0) {
        ILOG("Fail to Write I2C register");
        return rc;
    }
}

/*
 * Read multiple data from a register of the I2C device.
 *
 * @param dev       points to the I2C device to be read from
 * @param reg       the register to read from
 * @param recvbuf   points to the start of buffer to be read into
 * @param buf_len   length of the buffer to be read
 *
 * @return - number of bytes read if the read procedure succeeded
 *         - 0 if no bytes were read
 *         - negative if the read procedure failed
 */
int i2cReadBuff(struct I2cDevice *dev, uint8_t reg, uint8_t *recvbuf, size_t buf_len) {
    struct i2c_rdwr_ioctl_data msgset;
    struct i2c_msg msgs[2];
    uint8_t rc = 0;

    //2.read reg value
    msgs[0].len = 1;               //The length of the address of the target reg
    msgs[0].addr = dev->addr;      //i2c device address
    msgs[0].flags = 0;             //write flag
    msgs[0].buf = (uint8_t *)&reg; //target reg address

    msgs[1].len = buf_len;    //the length of the address of the target reg
    msgs[1].addr = dev->addr; //i2c device address
    msgs[1].flags = 1;        //read flag
    msgs[1].buf = recvbuf;    // allocate memory

    msgset.nmsgs = 2;   //number of messages
    msgset.msgs = msgs; //messa

    if (rc = ioctl(dev->fd, I2C_RDWR, (unsigned long)&msgset) < 0) {
        ILOG("Fail to Read I2C register");
        return rc;
    }
    // ILOG("rc %x\n", rc);
    return rc;
}
/*
 * Stop the I2C device.
 *
 * @param dev points to the I2C device to be stopped
 */
void i2c_stop(struct I2cDevice *dev) {
    /*
	 * Close the I2C bus file descriptor.
	 */
    close(dev->fd);
}