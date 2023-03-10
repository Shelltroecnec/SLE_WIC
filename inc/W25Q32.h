/**
 * @file W25Q32.h
 * @author Shahid Hashmi
 * @brief 
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef W25Q32_H
#define W25Q32_H

/* General */
#define W25Q32_SPI_FREQ     1000000
#define DUMMY_BYTE          0xA5
/* Registers*/
#define CMD_WRITE_ENABLE      0x06
#define CMD_WRITE_DISABLE     0x04
#define CMD_READ_STATUS_R1    0x05
#define CMD_READ_STATUS_R2    0x35
#define CMD_WRITE_STATUS_R1   0x01
#define CMD_PAGE_PROGRAM      0x02
#define CMD_QUAD_PAGE_PROGRAM 0x32 // Unimplemented
#define CMD_BLOCK_ERASE64KB   0xd8
#define CMD_BLOCK_ERASE32KB   0x52
#define CMD_SECTOR_ERASE      0x20 //4KB
#define CMD_CHIP_ERASE        0xC7 //or 0x60
#define CMD_ERASE_SUPPEND     0x75 // Unimplemented
#define CMD_ERASE_RESUME      0x7A // Unimplemented
#define CMD_POWER_DOWN        0xB9
#define CMD_HIGH_PERFORM_MODE 0xA3 // Unimplemented
#define CMD_CNT_READ_MODE_RST 0xFF // Unimplemented
#define CMD_RELEASE_PDOWN_ID  0xAB // Unimplemented
#define CMD_MANUFACURER_ID    0x90
#define CMD_READ_UNIQUE_ID    0x4B
#define CMD_JEDEC_ID          0x9F
#define CMD_ENABLE_RESET	  0x66 //myself
#define CMD_RESET_DEVICE	  0x99 //myself

#define CMD_READ_DATA         0x03
#define CMD_READ_DATA4B       0x13
#define CMD_FAST_READ         0x0B
#define CMD_FAST_READ4B       0x0C
#define CMD_READ_DUAL_OUTPUT  0x3B // Unimplemented
#define CMD_READ_DUAL_IO      0xBB // Unimplemented
#define CMD_READ_QUAD_OUTPUT  0x6B // Unimplemented
#define CMD_READ_QUAD_IO      0xEB // Unimplemented
#define CMD_WORD_READ         0xE3 // Unimplemented

#define SR1_BUSY_MASK	0x01
#define SR1_WEN_MASK	0x02
#define _4bmode  0

/* Functions / API */
spi_t *W25Q32_init(spi_t *spi_Dev, char spi_Mode, int spi_Freq);
int W25Q32_deinit(spi_t *spi_f);
uint8_t W25Q32_readStatusReg1(spi_t * dev, uint8_t * reg1);
uint8_t W25Q32_readStatusReg2(spi_t * dev, uint8_t * reg2);
uint8_t W25Q32_readuniqueID(spi_t * dev, uint8_t * id);
void W25Q32_dump(char *id, int ret, uint8_t *data, int len);
uint8_t W25Q32_readManufacturer(spi_t * dev,uint32_t addr, uint8_t * id);
uint8_t W25Q32_writeEnable(spi_t * dev);
uint8_t W25Q32_writeDisable(spi_t * dev);

uint16_t W25Q32_read(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
uint16_t W25Q32_fastread(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
bool W25Q32_eraseSector(spi_t * dev, uint16_t sect_no, bool flgwait);
bool W25Q32_erase64Block(spi_t * dev, uint16_t blk_no, bool flgwait);
bool W25Q32_erase32Block(spi_t * dev, uint16_t blk_no, bool flgwait);
bool W25Q32_eraseAll(spi_t * dev, bool flgwait);
int16_t W25Q32_pageWrite(spi_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n);
bool W25Q64_IsBusy(spi_t * dev);
uint8_t W25Q32_powerDown(spi_t * dev);
int16_t W25Q_pageWrite(spi_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n);
uint16_t W25Q_fastread(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
uint16_t W25Q30_fastread(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
uint16_t W25Q_read(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
#endif //W25Q32_H