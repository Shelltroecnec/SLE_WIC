/**
 * @file W25Q32.c
 * @author Shahid Hashmi
 * @brief 
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* Library */
#include "headers.h"

/* Flash - W25Q32 SPI Initilization */
spi_t *W25Q32_init(spi_t *spi_Dev, char spi_Mode, int spi_Freq)
{
    spi_t *spi_f = spi_new();
    CLOG("[W25Q32] W25Q32_init: SPI Dev: %s, mode: %d, freq: %d\n", spi_Dev, spi_Mode, spi_Freq);
    if (spi_open(spi_f, spi_Dev, spi_Mode, spi_Freq)) {
        ILOG("[W25Q32] W25Q32 SPI Print error Code Here\r\n");
        return NULL;
    }
    return spi_f;
}

/* Flash - W25Q32 SPI De-Initization */
int W25Q32_deinit(spi_t *spi_f) {
    if (spi_close(spi_f)) {
        ILOG("[W25Q32] Fail to close the Flash - W25Q32 SPI device\r\n");
        return -1;
    }
    return 0;
}


/* Read the Status Register 1 */
uint8_t W25Q32_readStatusReg1(spi_t * dev, uint8_t * reg1)
{
	uint8_t Txdata[1] = {0};
	uint8_t Rxdata[1] = {0};
	//Parsing into the buffer    
	Txdata[0] = CMD_READ_STATUS_R1;
	
    // CLOG("[W25Q32] Read Status1 length = %d \r\n", 1 * 8);
	CLOG("[W25Q32] Read Status1 Tx Data = %x \r\n",Txdata[0]);
	//Tx & RX through SPI
    if(spi_transfer(dev, Txdata, Rxdata, 1)) {
        WLOG("SPI Read error: %s", dev->error.errmsg);
        return dev->error.c_errno;
    }

    *reg1 = Rxdata[0];
    CLOG("[W25Q32] Status Register1: Data = %x \r\n",Rxdata[0]);
    return 0;
}


/* Read the Status Register 2 */
uint8_t W25Q32_readStatusReg2(spi_t * dev, uint8_t * reg2)
{
	uint8_t Txdata[1] = {0};
	uint8_t Rxdata[1] = {0};
	//Parsing into the Tx buffer for transmitting CMD
    Txdata[0] = CMD_READ_STATUS_R2;
	
	CLOG("[W25Q32] Read Status2 length = %d \r\n", 1 * 8);
	CLOG("[W25Q32] Read Status2 Tx Data = %x \r\n",Txdata[0]);
	//Tx & Rx through SPI
    if(spi_transfer(dev, Txdata, Rxdata,1)) {
        WLOG("SPI Read error: %s \r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

    *reg2 = Rxdata[0];
    CLOG("[W25Q32] Status Register2: Data = %x \r\n",Rxdata[0]);
    return 0;
}

/* This is Used to Read Unique ID */
uint8_t W25Q32_readuniqueID(spi_t * dev, uint8_t * id)
{
	uint8_t Txdata[5] = {0};
	uint8_t Rxdata[8] = {0};
	//memset(Txdata, 0, 5);
	//memset(Rxdata, 0, 8);

	//Parsing the Tx buffer
    Txdata[0] = CMD_READ_UNIQUE_ID;
	Txdata[1] = DUMMY_BYTE; //Dummy
	Txdata[2] = DUMMY_BYTE; //Dummy
	Txdata[3] = DUMMY_BYTE; //Dummy
	Txdata[4] = DUMMY_BYTE; //Dummy
	
	// CLOG("[W25Q32] UniqueID length = %d \r\n", 8 * 8);
	CLOG("[W25Q32] Tx Read UniqueID = %x %x %x %x %x \r\n",Txdata[0],Txdata[1],
	Txdata[2],Txdata[3],Txdata[4]);
	//SPI Transactions

    if(spi_transfer(dev, Txdata, Rxdata, 8)) {
        WLOG("SPI Read error: %s \r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	memcpy(id, &Rxdata[0], 8);
	
    return 0;
}


/* This Function is Used to Print */
void W25Q32_dump(char *id, int ret, uint8_t *data, int len)
{
	int i;
	printf("[%s] = %d\n",id, ret);
	for(i=0;i<len;i++) {
		printf("%0x ",data[i]);
		if ( (i % 10) == 9) printf("\n");
	}
	printf("\n");
}

// Get JEDEC ID(Manufacture, Memory Type,Capacity)
// d(out):Stores 3 bytes of Manufacture, Memory Type, Capacity
//
uint8_t W25Q32_readManufacturer(spi_t * dev, uint32_t addr, uint8_t * id)
{
	uint8_t Txdata[4] = {0};
	uint8_t Rxdata[3] = {0};
	//Parsing the Address 000000h for Reading the
	//Manufacturer ID
    Txdata[0] = CMD_JEDEC_ID;
	Txdata[1] = (addr>>16) & 0xFF; // A23-A16
	Txdata[2] = (addr>>8) & 0xFF; // A15-A08
	Txdata[3] = addr & 0xFF; // A07-A00

	CLOG("[W25Q32] ManufacturerID length = %d \r\n", 4 * 8);
	CLOG("[W25Q32] Tx Data = %x %x %x %x \r\n",Txdata[0],Txdata[1],Txdata[2],Txdata[3]);

	//SPI Transaction
    if(spi_transfer(dev,Txdata, Rxdata, 4)) {
        WLOG("SPI Read error: %s \r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

	// uint8_t ret =1;
	// W25Q32_dump("readManufacturer", ret, Rxdata, 3);
	
	memcpy(id, &Rxdata[0], 3);
	
	return 0 ;
}

/* Function for Write Enable - As it enables Write Opearion
into Flash.
NOTE :- This Function is Must be called prior 
-Page Program 
-Quad Page Program
-Sector Erase
-Block Erase
-Chip Erase
-Write Status Register
-Erase/Program Security Register
*/
uint8_t W25Q32_writeEnable(spi_t * dev)
{
	uint8_t txdata[1]= {0};
	uint8_t rxdata[1]= {0};

    txdata[0] = CMD_WRITE_ENABLE;
	
	CLOG("[W25Q32] length = %d \r\n", 1 * 8);
	CLOG("[W25Q32] Tx Data = %x \r\n",txdata[0]);
	//SPI Transaction
    if(spi_transfer(dev, txdata, rxdata, 1)) {
        CLOG("SPI Read error: %s \r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	// if(rxdata[0] == 255){
	// CLOG("[W25Q32] Write Enable = %d \r\n",rxdata[0]);
    // CLOG("[W25Q32] Write Enable Successfull !\r\n");
    // return 0; //success
	// } 
	// return -1; //failure
	return 0;
}

/* Fuction to Disable Write Operation */
uint8_t W25Q32_writeDisable(spi_t * dev)
{
	uint8_t txdata[1]= {0};
	uint8_t rxdata[1]= {0};

    txdata[0] = CMD_WRITE_DISABLE;
	CLOG("[W25Q32] length = %d \r\n", 1 * 8);
	CLOG("[W25Q32] Tx Data = %x \r\n",txdata[0]);
	//SPI Transaction
    if(spi_transfer(dev, txdata, rxdata, 1)) {
        CLOG("SPI Read error: %s \r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	CLOG("[W25Q32] Write Disable Successfull !\r\n");
    return 0;
}


/* This Function is for Write Status Register R1 */
uint8_t W25Q32_writeStatusReg1(spi_t * dev, uint8_t *reg1)
{
    uint8_t txdata[1] = {0};
	uint8_t rxdata[1] = {0};
    
	txdata[0] = CMD_WRITE_STATUS_R1;

    CLOG("[W25Q32] length = %d \r\n", 1 * 8);
	CLOG("[W25Q32] Tx Data = %x \r\n",txdata[0]);
	//SPI Transactions
    if(spi_transfer(dev,txdata, rxdata, 1)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

    *reg1 = rxdata[1];
    CLOG("[W25Q] Status Register1 Read Sucess = %x \r\n",rxdata[1]);
    
    return 0;
}


/* This Function is Used For the Read opterion from the given sector */
uint16_t W25Q32_read(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{ 
	uint8_t *data;
	data = (uint8_t *)malloc(n+4);
	size_t offset;
	//Parsing into the Tx buffer
	data[0] = CMD_READ_DATA;
	data[1] = (addr>>16) & 0xFF; // A23-A16
	data[2] = (addr>>8) & 0xFF; // A15-A08
	data[3] = addr & 0xFF; // A07-A00
	offset = 4;

	if(spi_transfer(dev, data, buf, n + offset)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	dump(buf,n+offset);
	free(data);
	return n;

}

/* This Function is used to Read Operation by using the Databuffers */
uint16_t W25Q_read(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{
	uint8_t Readtx[30] = {0};
	uint8_t Readrx[30] = {0};
	size_t offset;
	//Parsing the Tx Buffer with
	//command and Address
	Readtx[0] = CMD_READ_DATA;
	Readtx[1] = (addr>>16) & 0xFF; // A23-A16
	Readtx[2] = (addr>>8) & 0xFF; // A15-A08
	Readtx[3] = addr & 0xFF; // A07-A00
	offset = 4;
	
	CLOG("[W25Q32] Read Length = %d \r\n", (n+offset)*8);
	CLOG("[W25Q32] Read Tx Data = %x %x %x %x\r\n", Readtx[0],Readtx[1],Readtx[2],Readtx[3]);
	//SPI Transactions
	if(spi_transfer(dev, Readtx, Readrx, n)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

	memcpy(buf, &Readrx[offset],n);
	dump(buf,n);
	//free(Txdata);
	//free(Rxdata);
	
	return n;

}

//
// Fast read data
// addr(in):Read start address
//          3 Bytes Address Mode : 24 Bits 0x000000 - 0xFFFFFF
//          4 Bytes Address Mode : 32 Bits 0x00000000 - 0xFFFFFFFF
// n(in):Number of read data
//
uint16_t W25Q32_fastread(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{
	uint8_t *TxData;
	//uint8_t *RxData=NULL;
	TxData = (uint8_t *)malloc(n+5);
	//RxData = (uint8_t *)malloc(n+5);
	size_t offset;
	//Parsing into the Tx buffer
	TxData[0] = CMD_FAST_READ;	
	TxData[1] = (addr>>16) & 0xFF; // A23-A16
	TxData[2] = (addr>>8) & 0xFF; // A15-A08
	TxData[3] = addr & 0xFF; // A07-A00
	TxData[4] = DUMMY_BYTE; // Dummy
	offset = 5;

	//SPI Transactions
	if(spi_transfer(dev, TxData, &TxData[offset], n+offset)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	
	memcpy(buf, &TxData[offset], n);
	//CLOG("[W25Q32] Fast Read : Data = %x %x %x \r\n",buf[0],buf[1],buf[2]);
	dump(buf,n+offset);
	
	free(TxData);
	//free(RxData);
	return n;
}


/* String Read */
uint16_t W25Q30_fastread(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{
	uint8_t *TxData;
	uint8_t *RxData;
	int ans=0;
	uint8_t rdata[11];
	memset(rdata, 0, 11);
	int i;
	for (i = 0; i < 10; i++)
	{
		rdata[i] = '0' + i; // 0-9
	}
	rdata[i+1] = '\0';

	TxData = (uint8_t *)malloc(5);
	RxData = (uint8_t *)malloc(n);
	size_t offset;

	//Parsing
	TxData[0] = CMD_FAST_READ;	
	TxData[1] = (addr>>16) & 0xFF; // A23-A16
	TxData[2] = (addr>>8) & 0xFF; // A15-A08
	TxData[3] = addr & 0xFF; // A07-A00
	TxData[4] = 0; // Dummy
	offset = 5;

	CLOG("[W25Q32] Length = %d \r\n", (n+offset)*8);
	CLOG("[W25Q32] Tx Data = %x %x %x %x %x \r\n",TxData[0],TxData[1],TxData[2],TxData[3],TxData[4]);
	if(spi_transfer(dev, TxData, RxData, 15)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	
	memcpy(buf, &RxData[0], 11);
	//comapring String
	if(strncmp(rdata,&RxData[0],11) == 0)
	{
		CLOG("!! Successful Comparision !! \r\n");
		dump(buf,20);
		return n; //success
	}

	free(TxData);
	free(RxData);

	dump(buf,256);
	return -1; //failure
}


/* This is same Fast Read Function but using the Buffer */
uint16_t W25Q_fastread(spi_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{
	uint8_t tx[5] = {0};
	uint8_t rx[30] = {0};
	
	size_t offset;
	//Parsing
	tx[0] = CMD_FAST_READ;
	tx[1] = (addr>>16) & 0xFF; // A23-A16
	tx[2] = (addr>>8) & 0xFF; // A15-A08
	tx[3] = addr & 0xFF; // A07-A00
	tx[4] = 0; // Dummy
	offset = 5;

	CLOG("[W25Q32] Read Length = %d \r\n", (n+offset)*8);
	CLOG("[W25Q32] Tx Data = %x %x %x %x %x \r\n",tx[0],tx[1],tx[2],tx[3],tx[4]);
	
	if(spi_transfer(dev, tx, rx, n+offset)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	memcpy(buf,&rx[0], n);
	dump(buf,256);
	
	return n;
}

//
// Erasing data in 4kb space units
// sect_no(in):Sector number(0 - 2048)
// flgwait(in):true:Wait for complete / false:No wait for complete
// Return value: true:success false:fail
//
// Note:
// The data sheet states that erasing usually takes 30ms and up to 400ms.
// The upper 11 bits of the 23 bits of the address correspond to the sector number.
// The lower 12 bits are the intra-sectoral address.
//
bool W25Q32_eraseSector(spi_t * dev, uint16_t sect_no, bool flgwait)
{
	uint8_t txdata[4] = {0};
	uint8_t rxdata[4] = {0};
	uint32_t addr = sect_no; //Sector Number, using sector 0
	addr<<=12; 

	// Write permission setting
	uint8_t ret=0;
	ret = W25Q32_writeEnable(dev);
	if (ret != 0 ) return false;

	//Parsing the Data buffer
	txdata[0] = CMD_SECTOR_ERASE;
	txdata[1] = (addr>>16) & 0xff;
	txdata[2] = (addr>>8) & 0xff;
	txdata[3] = addr & 0xff;

	CLOG("[W25Q32] Sector Erase Length = %d \r\n", 4 * 8);
	CLOG("[W25Q32] Tx Data = %x %x %x %x \r\n",txdata[0],txdata[1],txdata[2],txdata[3]);
	//SPI Transactions
	if(spi_transfer(dev, txdata, rxdata, 4)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	
	// if(rxdata[0] >= 0 || rxdata[0] <=255 ){
	// 	CLOG("[W25Q32] Sector Erase Succesful !! \r\n");
	// 	return true;
	// }
	return true;
}

//
// Erasing data in 64kb space units
// blk_no(in):Block number(0 - 127)
// flgwait(in):true:Wait for complete / false:No wait for complete
// Return value: true:success false:fail
//
// Note:
// The data sheet states that erasing usually takes 150ms and up to 1000ms.
// The upper 7 bits of the 23 bits of the address correspond to the block.
// The lower 16 bits are the address in the block.
//
bool W25Q32_erase64Block(spi_t * dev, uint16_t blk_no, bool flgwait)
{
	uint8_t data[4];
	uint32_t addr = blk_no;
	addr<<=16;

	// Write permission setting
	uint8_t ret = 0;
	ret = W25Q32_writeEnable(dev);
	if (ret != 0) return false;

	//Parsing into buffer
	data[0] = CMD_BLOCK_ERASE64KB;
	data[1] = (addr>>16) & 0xff;
	data[2] = (addr>>8) & 0xff;
	data[3] = addr & 0xff;
	
	CLOG("[W25Q32] Bloack Erase 64 Length = %d \r\n", 4 * 8);
	CLOG("[W25Q32] Tx Data = %x %x %x %x \r\n",data[0],data[1], data[2], data[3]);
	
	//SPI Transactions
	if(spi_transfer(dev, data, data, 4)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	
	// Busy check
	while( W25Q64_IsBusy(dev) & flgwait) {  //ToDo
		usleep(1000);
	}

	return true;
}

//
// Erasing data in 32kb space units
// blk_no(in):Block number(0 - 255)
// flgwait(in):true:Wait for complete / false:No wait for complete
// Return value: true:success false:fail
//
// Note:
// The data sheet states that erasing usually takes 120ms and up to 800ms.
// The upper 8 bits of the 23 bits of the address correspond to the block.
// The lower 15 bits are the in-block address.
//
bool W25Q32_erase32Block(spi_t * dev, uint16_t blk_no, bool flgwait)
{
	//spi_transaction_t SPITransaction;
	uint8_t data[4];
	uint32_t addr = blk_no;
	addr<<=15;

	// Write permission setting
	uint8_t ret = 0;
	ret = W25Q32_writeEnable(dev);
	if (ret != 0) return false;

	//Parsing Block Erase TX Buffer
	data[0] = CMD_BLOCK_ERASE32KB;
	data[1] = (addr>>16) & 0xff;
	data[2] = (addr>>8) & 0xff;
	data[3] = addr & 0xff;

	CLOG("[W25Q32] Block Erase 32 Length = %d \r\n", 4*8);
	CLOG("[W25Q32] Tx Data = %x %x %x %x \r\n",data[0],data[1], data[2], data[3]);

	//SPI Transactions
	if(spi_transfer(dev, data, data, 4)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	
	// Busy check
	while( W25Q64_IsBusy(dev) & flgwait) {  //ToDo
		usleep(1000);
	}

	return true;
}


//
// Erase all data
// flgwait(in):true:Wait for complete / false:No wait for complete
// Return value: true:success false:fail
//
// Note:
// The data sheet states that erasing usually takes 15s and up to 30s.
//
bool W25Q32_eraseAll(spi_t * dev, bool flgwait)
{
	uint8_t data[1];

	// Write permission setting
	uint8_t ret = 0;
	ret = W25Q32_writeEnable(dev);
	if (ret != 0) return false;

	data[0] = CMD_CHIP_ERASE;

	CLOG("[W25Q32] Length = %d \r\n", 1*8);
	CLOG("[W25Q32] Tx Data = %x \r\n",data[0]);

	if(spi_transfer(dev, data, data, 1)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	
	// Busy check
	while( W25Q64_IsBusy(dev) & flgwait) {  //ToDo
		usleep(1000);
	}
	return true;
}

//
// Page write
// sect_no(in):Sector number(0x00 - 0x7FF) 
// inaddr(in):In-sector address(0x00-0xFFF)
// data(in):Write data
// n(in):Number of bytes to write(0～256)
//

// page size - 256 bytes - 16K pages, 4 K sector
// sector size - 4096 bytes - 4K/32K/64K
// block size: 32 K - 8 * sectors	
// page(256Bytes) -> Sectors(16 pages) -> Block(16 sectors) -> Chip (64 blocks)

int16_t W25Q32_pageWrite(spi_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n)
{
	if (n > 256) return 0;

	uint8_t *txdata, *rxdata;
	
	uint32_t addr = sect_no;
	addr <<= 12;
	addr += inaddr;

	// Write permission setting
	int ret;
	ret = W25Q32_writeEnable(dev);
	if (ret != 0) return 0;

	printf("Writing to: %d\r\n", addr);
	//Parsing
	txdata = (unsigned char *)malloc(n+4);
	rxdata = (unsigned char *)malloc(n+4);
	txdata[0] = CMD_PAGE_PROGRAM;
	txdata[1] = (addr>>16) & 0xff;
	txdata[2] = (addr>>8) & 0xff;
	txdata[3] = addr & 0xFF;
	memcpy( &txdata[4], buf, n );

	CLOG("[W25Q32] Page Write Length = %d \r\n", (n+4));
	
	if(spi_transfer(dev, txdata, rxdata, n+4)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }
	CLOG("[W25Q32] Page Write Rx = %x %x %x %x %x \r\n",rxdata[0],rxdata[1],
	rxdata[2],rxdata[3],rxdata[4],)
	CLOG("[W25Q32] Successful Page Write Operation ! \r\n");
	dump(txdata,n+4);

	free(txdata);
	free(rxdata);
	return n;
}

/* This Function does the Page Write Operation
Using the TX  & RX Buffer */
int16_t W25Q_pageWrite(spi_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n)
{
	if (n > 256) return 0;
	
	uint8_t txw[30] = {0};
	//uint8_t rxw[5] = {0};

	uint32_t addr = sect_no;
	addr<<=12;
	addr += inaddr;

	// Write permission setting
	int ret = 0;
	ret = W25Q32_writeEnable(dev);
	if (ret != 0) return 0;

	//Page Write Buffer Tx Parsing
	txw[0] = CMD_PAGE_PROGRAM;
	txw[1] = (addr>>16) & 0xff;
	txw[2] = (addr>>8) & 0xff;
	txw[3] = addr & 0xFF;
	memcpy( &txw[4], buf, n );

	CLOG("[W25Q32] Page Write Length = %d \r\n", (n+4)*8);
	
	if(spi_transfer(dev, txw, NULL, 30)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

	CLOG("[W25Q32] Successful Page Write Operation ! \r\n");
	dump(txw, 30);
	
	return n;
}

// Check during processing such as writing
// Return value: true:processing false:idle
//
bool W25Q64_IsBusy(spi_t *dev)
{
	uint8_t Txfdata[1]={0};
	uint8_t Rxfdata[1]={0};
	Txfdata[0] = CMD_READ_STATUS_R1;

	CLOG("[W25Q32] Busy Check Length = %d \r\n", 2*8);
	CLOG("[W25Q32] Tx Data = %x %x \r\n",Txfdata[0]);

	if(spi_transfer(dev, Txfdata, Rxfdata , 1)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

	CLOG("[W25Q32] Busy CMD SPI Transfer Success \n");
	if((Rxfdata[0] & SR1_BUSY_MASK) != 0) return true;

	return false;
}


//
// Power down 
//
uint8_t W25Q32_powerDown(spi_t *dev)
{
	uint8_t data[1] = {0};

	data[0] = CMD_POWER_DOWN;

	CLOG("[W25Q32] Busy Check Length = %d \r\n", 1*8);
	CLOG("[W25Q32] Tx Data = %x %x \r\n",data[0]);
	//SPI Transactions
	if(spi_transfer(dev, data, data, 1)) {
        CLOG("SPI Read error: %s\r\n", dev->error.errmsg);
        return dev->error.c_errno;
    }

	return 0;
}
