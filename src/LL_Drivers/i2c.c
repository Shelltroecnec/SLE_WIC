

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"
#include "headers.h"




static int _i2c_error(sti2c_t *i2c, int code, int c_errno, const char *fmt, ...)
{
    va_list ap;

    i2c->error.c_errno = c_errno;

    va_start(ap, fmt);
    vsnprintf(i2c->error.errmsg, sizeof(i2c->error.errmsg), fmt, ap);
    va_end(ap);

    /* Tack on strerror() and errno */
    if (c_errno)
    {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(i2c->error.errmsg + strlen(i2c->error.errmsg), sizeof(i2c->error.errmsg) - strlen(i2c->error.errmsg), ": %s [errno %d]", buf, c_errno);
    }

    return code;
}

sti2c_t *i2c_new(void)
{
    sti2c_t *i2c = calloc(1, sizeof(sti2c_t));
    if (i2c == NULL)
        return NULL;

    i2c->fd = -1;

    return i2c;
}

void i2c_free(sti2c_t *i2c)
{
    free(i2c);
}

int i2c_open(sti2c_t *i2c, const char *path)
{
    unsigned long supported_funcs;

    memset(i2c, 0, sizeof(sti2c_t));

    /* Open device */
    if ((i2c->fd = open(path, O_RDWR)) < 0)
        return _i2c_error(i2c, I2C_ERROR_OPEN, errno, "Opening I2C device \"%s\"", path);

    /* Query supported functions */
    if (ioctl(i2c->fd, I2C_FUNCS, &supported_funcs) < 0)
    {
        int errsv = errno;
        close(i2c->fd);
        i2c->fd = -1;
        return _i2c_error(i2c, I2C_ERROR_QUERY, errsv, "Querying I2C functions");
    }

    if (!(supported_funcs & I2C_FUNC_I2C))
    {
        close(i2c->fd);
        i2c->fd = -1;
        return _i2c_error(i2c, I2C_ERROR_NOT_SUPPORTED, 0, "I2C not supported on %s", path);
    }

    return 0;
}

int i2c_transfer(sti2c_t *i2c, struct i2c_msg *msgs, size_t count)
{
    struct i2c_rdwr_ioctl_data i2c_rdwr_data;
    if (i2c == NULL || i2c->fd <= 0)
    {
        ILOG("I2C device is not initialize...");
        return -1;
    }

    /* Prepare I2C transfer structure */
    memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));
    i2c_rdwr_data.msgs = msgs;
    i2c_rdwr_data.nmsgs = count;

    /* Transfer */
    if (ioctl(i2c->fd, I2C_RDWR, &i2c_rdwr_data) < 0)
        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");

    return 0;
}

int i2c_close(sti2c_t *i2c)
{
    if (i2c->fd < 0)
        return 0;

    /* Close fd */
    if (close(i2c->fd) < 0)
        return _i2c_error(i2c, I2C_ERROR_CLOSE, errno, "Closing I2C device");

    i2c->fd = -1;

    return 0;
}

int i2c_tostring(sti2c_t *i2c, char *str, size_t len)
{
    return snprintf(str, len, "I2C (fd=%d)", i2c->fd);
}

const char *i2c_errmsg(sti2c_t *i2c)
{
    return i2c->error.errmsg;
}

int i2c_errno(sti2c_t *i2c)
{
    return i2c->error.c_errno;
}

int i2c_fd(sti2c_t *i2c)
{
    return i2c->fd;
}

/**
 * @brief This function writes multiple bytes to the register(More specifically on I2C bus) of I2C slave device
 * 
 * @param i2c       I2C devices tructure
 * @param sendbuf   Array of Bytes to be transfer over I2C bus, please note first byte should be register address,
 *                  as this buffer will be sent as it is after Command/Slave address (i.e first byte on I2C bus)
 * @param buf_len   Number of bytes to be transfer over I2C bus Exxluding Slave Address (or command byte)
 * @return int 
 */
int i2c_write_bytes(sti2c_t *i2c, uint8_t reg, uint8_t *sendbuf, size_t buf_len)
{

    struct i2c_msg msgs[1];
    uint8_t rgucWrBuff[256] = {0}; //taken 256 bytes for safer side, it may not be used ever
    rgucWrBuff[0] = reg; //adding register address to the first byte
    memcpy((uint8_t *)&rgucWrBuff[1], sendbuf, buf_len);
    //write I2C reg
    msgs[0].len = buf_len + 1; //set Number of bytes to be transfered on I2C bus +1 Byte for register address
    msgs[0].addr = i2c->addr;  //set i2c device address
    msgs[0].flags = 0;         //Set R/W flags, 0: indicates Write; 1: indicates Read.
    msgs[0].buf = rgucWrBuff;  //register address and data should be based on the datasheet of the end-device

    /* Transfer a transaction with two I2C messages */
    if (i2c_transfer(i2c, msgs, 1) < 0)
    {
        fprintf(stderr, "i2c_transfer(): %s\n", i2c_errmsg(i2c));
        return -1;
    }

    // if (rc = ioctl(dev->fd, I2C_RDWR, (unsigned long)&msgset) < 0)
    // {
    //     ILOG("Fail to Write I2C register");
    //     return rc;
    // }
    return 0;
}

/*
 * Read multiple bytes from a register of the I2C slave device
 *
 * @param i2c       pointer to the I2C device structure
 * @param recvbuf   points to the start of buffer to be read into
 * @param buf_len   length of the buffer to be read
 *
 * @return - number of bytes read if the read procedure succeeded
 *         - 0 if no bytes were read
 *         - negative if the read procedure failed
 */
int i2c_read_bytes(sti2c_t *i2c, uint8_t reg, uint8_t *recvbuf, size_t buf_len)
{
    struct i2c_msg msgs[2];

    //2.read reg value
    msgs[0].len = 1;                //The length of the address of the target reg
    msgs[0].addr = i2c->addr;       //i2c device address
    msgs[0].flags = 0;              //write flag
    msgs[0].buf = (uint8_t *)&reg;  //target reg address

    msgs[1].len = buf_len;          //the length of the address of the target reg
    msgs[1].addr = i2c->addr;       //i2c device address
    msgs[1].flags = I2C_M_RD;       //read flag
    msgs[1].buf = recvbuf;          // allocate memory

    /* Transfer a transaction with two I2C messages */
    if (i2c_transfer(i2c, msgs, 2) < 0)
    {
        fprintf(stderr, "i2c_transfer(): %s\n", i2c_errmsg(i2c));
        return -1;
    }

    // if (rc = ioctl(dev->fd, I2C_RDWR, (unsigned long)&msgset) < 0)
    // {
    //     ILOG("Fail to Read I2C register");
    //     return rc;
    // }
    // ILOG("rc %x\n", rc);
    return 0;
}
