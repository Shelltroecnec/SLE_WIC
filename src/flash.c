/**
 * @file flash.c
 * @author Shahid Hashmi
 * @brief
 * @version 0.1
 * @date 2023-02-10
 *
 * @copyright Copyright (c) 2023
 *
 */

/* Library */
#include "headers.h"

/* Flach Help Functions */
void flash_help(void)
{
	CLOG("FSH command Usage:\n");
	CLOG("   %s -c FSH [Option] \n", exec_filename);
	CLOG("      Option - Flash Storage Operation selection:\n");
	CLOG("          0 - Test All 3 Erase/Write/Read Flash\n");
	CLOG("          3 - Erase Sector \n");
	CLOG("          1 - Read Flash\n");
	CLOG("          2 - Write Flash\n");
}

void flash_cmd(int argc, char **argv, char *additionalArgv)
{
	if (argc < FLASH_MIN_ARG_REQ)
	{
		flash_help();
		return;
	}
	// Variables
	int len = 0;
	uint8_t rebuf[256];

	// Parsing the inputs from CLI terminals
	int iflashOpr = 0;
	if (argc > 1)
		iflashOpr = atoi(argv[1]);

	if (iflashOpr > 4 || iflashOpr < 0)
	{
		flash_help();
		return;
	}
	CLOG("[FLASH] Operation %d Slected \n", iflashOpr);

	// spi initialization Phase
	spi_t *spi_flash = NULL;
	struct timeval tv;

	if ((spi_flash = W25Q32_init(config.W25Q32_spi_device, SPI_MODE_3, W25Q32_SPI_FREQ)) == NULL)
	{
		WLOG("[W25Q32] Flash Initialization failed !! \r\n");
		return;
	}

	/* Get Status Register 1 Section */
	uint8_t reg1;
	uint8_t Mret;
	// calling for Staus Register 1 reading
	Mret = W25Q32_readStatusReg1(spi_flash, &reg1);
	if (Mret != 0)
	{
		WLOG("[W25Q32] Read Status Reg1 Failed, ret: %d\r\n", Mret);
		return;
	}
	CLOG("[W25Q32] Read Status Reg1 Successful\r\n");

	/* Get Status Register 2 Section */
	uint8_t reg2;
	uint8_t Mret2;
	// calling the Status Register 2 reading
	Mret2 = W25Q32_readStatusReg2(spi_flash, &reg2);
	if (Mret2 != 0)
	{
		WLOG("[W25Q32] Read Status Reg2 Failed, ret: %d\r\n", Mret2);
		return;
	}
	CLOG("[W25Q32] Read Status Reg2 Successful\r\n");

	/* Get Unique ID Section */
	uint8_t uid[8] ={0};
	uint8_t uRet = 0;
	uRet = W25Q32_readuniqueID(spi_flash, uid);
	if (uRet != 0)
	{
		WLOG("[W25Q32] read Unique ID fail %d\r\n", uRet);
		return;
	}
	CLOG("[W25Q32] read Unique ID : %x-%x-%x-%x-%x-%x-%x-%x \r\n",
		 uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uid[7]);

	/* Get JEDEC ID i.e. Manufacturer ID section */
	uint8_t jid[3] = {0};
	uint8_t jRet = 0;
	jRet = W25Q32_readManufacturer(spi_flash, 0, jid);
	if (jRet != 0)
	{
		WLOG("[W25Q32] read Manufacturer failed, ret: %d\r\n", jRet);
		usleep(1000);
	}
	CLOG("[W25Q32] readManufacturer : %x-%x-%x \r\n",
		 jid[0], jid[1], jid[2]);

	/* If Selected Erase Sector Operation/Option */
	if (iflashOpr == 3)
	{
		// Erase data by Sector
		uint8_t rbuf[256];
		int erlen;

		bool flag = W25Q32_eraseSector(spi_flash, 0, true);
		if (flag == false)
		{
			WLOG("[W25Q32] Erase Sector fail ! \r\n");
			usleep(1000);
		}
		memset(rbuf, 0, 256);
		erlen = W25Q_read(spi_flash, 0, rbuf, 256);
		if (erlen != 256)
		{
			WLOG("[W25Q32] Read fail ! \r\n");
			usleep(1000);
		}
		CLOG("[W25Q32] Read Data: len=%d \r\n", erlen);
		// dump(rbuf, 256);
	}

	/* If Selected Read Operation */

	if (iflashOpr == 1)
	{

		// Read 256 byte data from Address=0

		int rlen;
		memset(rebuf, 0, 256);

		rlen = W25Q_fastread(spi_flash, 0, rebuf, 256);
		if (rlen != 256)
		{
			WLOG("[W25Q32] Fastread fail ! \r\n");
			usleep(1000);
		}
		CLOG("[W25Q32] Fast Read Data: len=%d \r\n", rlen);
		CLOG("[W25Q32] Successfull Read Operation !! \r\n");
		// dump(rebuf,256);
	}

	/* If Selected Write Operation */

	if (iflashOpr == 2)
	{
		// Write data to Sector=0 Address=0
		uint8_t wdata[26];
		for (int i = 0; i < 26; i++)
		{
			wdata[i] = 'A' + i; // A-Z
		}
		wdata[26] = '\0';

		len = W25Q_pageWrite(spi_flash, 0, 0, wdata, 26);
		if (len != 26)
		{
			WLOG("[W25Q32] PageWrite fail ! \r\n");
			usleep(1000);
		}
		CLOG("[W25Q32] Page Write(Sector=0 Address=0) len=%d \r\n", len);
		CLOG("[W25Q32] Successfull Write Operation !! \r\n");
	}

	/* Here we Test All Conditions
	 i.e. 1) Fast Read the given sector 0
		  2) Then Erase Sector 0
		  3) Then again Read back using Normal Read
		  4) Write 26Byte Data to Sector 0 / Address 10
		  5) Then Read back using Fast Read
		  6) Write 10Byte Data to Sector 0 Address 0
		  7) Then Read back using Fast Read
		  */

	if (iflashOpr == 0)
	{
		// Read 256 byte data from Address=0
		uint8_t rbuf[256];
		int len;
		
		// Erase data by Sector
		CLOG("[W25Q32] Step 1: Sector 0 Erase\r\n");
		bool flag = W25Q32_eraseSector(spi_flash, 0, true);
		if (flag == false)
		{
			CLOG("[W25Q32] Test Erase Sector fail\r\n");
			return -1;
		}
		memset(rbuf, 0, 256);
		CLOG("[W25Q32] Step 2: Reading Sector 0-----------\r\n");
		len = W25Q32_read(spi_flash, 0, rbuf, 27);
	
		CLOG("[W25Q32] Test Read Done: len = %d\r\n", len);
		// dump(rbuf, 256);

		// Write data to Sector=0 Address=10
		uint8_t wdata[27] = {0};
		int i, j;
		for (i = 0, j=65; i < 26; i++, j++)
		{
			wdata[i] = j; // A-Z
		}
		
		CLOG("[W25Q32] Step 3:  Writing Sector 0-----------\r\n");
		
		len = W25Q32_pageWrite(spi_flash, 0, 10, wdata, 27);
		
		CLOG("[W25Q32] Page Write(sector=0 Address=0) len=%d, done \r\n", len);

		// First read 256 byte data from Address=0
		memset(rbuf, 0, 256);
		
		CLOG("[W25Q32] Step 4: Reading Sector 0-----------\r\n");
		
		len = W25Q32_fastread(spi_flash, 0, rbuf, 27);
		
		CLOG("[W25Q32] Test Fast Read Data: len=%d \r\n", len);

		W25Q32_deinit(spi_flash); // Deinit SPI
	}

	/* String */
	if (iflashOpr == 4)
	{
		uint8_t mybuf[256];
		int len;

		// memset(mybuf, 0, 256);
		// len = W25Q32_fastread(spi_flash, 0, mybuf, 256);
		// if (len != 256)
		// {
		// 	CLOG("[W25Q32] Test fastread fail \r\n");
		// 	usleep(1000);
		// }
		// CLOG("[W25Q32] Test Fast Read Data: len=%d \r\n", len);
		// // dump(rbuf, 256);

		// // Erase data by Sector
		// bool flag = W25Q32_eraseSector(spi_flash, 0, true);
		// if (flag == false)
		// {
		// 	CLOG("[W25Q32] Test Erase Sector fail %d \r\n");
		// 	usleep(1000);
		// }
		// memset(mybuf, 0, 256);
		// len = W25Q32_read(spi_flash, 0, mybuf, 256);
		// if (len != 256)
		// {
		// 	CLOG("[W25Q32] Test read fail \r\n");
		// 	usleep(1000);
		// }
		// CLOG("[W25Q32] Test Read Data: len=%d \r\n", len);
		// usleep(1000);

		bool flag = W25Q32_eraseSector(spi_flash, 0, true);
		if (flag == false)
		{
			WLOG("[W25Q32] Erase Sector fail ! \r\n");
			usleep(1000);
		}
		memset(mybuf, 0, 256);
		len = W25Q32_read(spi_flash, 0, mybuf, 256);
		if (len != 256)
		{
			WLOG("[W25Q32] Read fail ! \r\n");
			usleep(1000);
		}
		CLOG("[W25Q32] Read Data: len=%d \r\n", len);

		// Write data to Sector=0 Address=10
		uint8_t wdata[10];
		memset(wdata, 0, 10);
		for (int i = 0; i < 10; i++)
		{
			wdata[i] = '0' + i; // 0-9
		}
		len = W25Q32_pageWrite(spi_flash, 0, 10, wdata, 10);
		if (len != 10)
		{
			CLOG("[W25Q32] Test pageWrite fail \r\n");
			usleep(1000);
		}
		CLOG("[W25Q32] Test Page Write(Sector=0 Address=10) len=%d \r\n", len);

		// First read 256 byte data from Address=0
		memset(mybuf, 0, 256);
		len = W25Q30_fastread(spi_flash, 0, mybuf, 256);
		if (len != 256)
		{
			CLOG("[W25Q32] Test fastread fail \r\n");
			usleep(1000);
		}
		CLOG("[W25Q32] Test Fast Read Data: len=%d \r\n", len);
		W25Q32_deinit(spi_flash); // Deinit SPI
	}
}

/* This Function is Used for 256 Bytes Memory Print */
void dump(uint8_t *dt, int n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint8_t sum;
	uint8_t vsum[16];
	uint8_t total = 0;
	uint32_t saddr = 0;
	uint32_t eaddr = n - 1;

	printf("----------------------------------------------------------\n");
	uint16_t i;
	for (i = 0; i < 16; i++)
		vsum[i] = 0;
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++)
	{
		data = dt[addr];
		if (clm == 0)
		{
			sum = 0;
			printf("%05x: ", addr);
		}

		sum += data;
		vsum[addr % 16] += data;

		printf("%02x ", data);
		clm++;
		if (clm == 16)
		{
			printf("|%02x \n", sum);
			clm = 0;
		}
	}
	printf("----------------------------------------------------------\n");
	printf("       ");
	for (i = 0; i < 16; i++)
	{
		total += vsum[i];
		printf("%02x ", vsum[i]);
	}
	printf("|%02x \n\n", total);
}