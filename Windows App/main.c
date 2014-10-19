#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include <tchar.h>

#define CMD_NONE        0x00
#define CMD_READ_DATA   0x01 
#define CMD_PAGE_WRITE  0x02 
#define CMD_CHIP_INFO   0x03 
#define CMD_READ_REG    0x04
#define CMD_READ_STATUS 0x05 
#define CMD_WRITE_EN    0x06 
#define CMD_WRITE_DIS   0x07 
#define CMD_BLOCK_ERASE 0x08 
#define CMD_WRITE_FLAG  0x09

#define n	1024
HANDLE hSerial;
unsigned short cal_crc(unsigned char *buf, int len)
{
  unsigned short crc = 0;
  int i;
  for(i = 0;i < len; i++)
  {
    crc = crc + buf[i];
  }
  return crc & 0xffff;
}
unsigned int get_chip_id()
{
	unsigned char packet[262] = {0};
	int i;
	DWORD bw,br;
	unsigned short crc16;
	
	packet[0] = CMD_CHIP_INFO;
	crc16 = cal_crc(packet, 260);
	packet[260] = (crc16 >> 8) & 0xff;
	packet[261] = crc16 & 0xff;
	WriteFile(hSerial, packet, sizeof(packet), &bw, NULL);
	//printf("Byte to Write %d\n", bw);
	
	Sleep(1000);
	if(!ReadFile(hSerial, packet, sizeof(packet), &br, NULL))
	{
		//error occurred. Report to user.
		printf("Read Error\n");
	}
	/*
	for(i=0;i<262;i++)
	{
		if(i % 16 == 0)printf("\n");
		printf("0x%02X ",packet[i]);
		
	}
	*/
	PurgeComm(hSerial, PURGE_RXCLEAR);
	return (packet[1] << 16) | (packet[2] << 8) | (packet[3]);
	
}
int read_response(unsigned char *packet)
{
	int count =0;
	DWORD br;
	while(count < 262)
	{
		if(!(ReadFile(hSerial, &packet[count], 262, &br, NULL)))
		{
			//error occurred. Report to user.
			printf("Read Error\n");
		}
		//printf("Count: %d      \r", count);
		count = count + br;
	}
	//printf("\n");
	return count;
}
unsigned int get_chip_data(unsigned int address, unsigned char *obuff)
{
	unsigned char packet[262] = {0};
	int i,loop;
	DWORD bw,br,tbr,cc;
	unsigned short crc16,tcrc;
	
	
	//printf("Byte to Write %d\n", bw);
	
	//Sleep(1000);
	br = 0;
	tbr=0;
	loop = 0;

	packet[0] = CMD_READ_DATA;
	packet[1] = (address >> 16) & 0xff;
	packet[2] = (address >> 8) & 0xff;
	packet[3] = address & 0xff;
	crc16 = cal_crc(packet, 260);
	packet[260] = (crc16 >> 8) & 0xff;
	packet[261] = crc16 & 0xff;
	//printf("Write cmd\n");
	WriteFile(hSerial, packet, sizeof(packet), &bw, NULL);
	
	//printf("Write cmd: Read response\n");	
	read_response(packet);
	memcpy(obuff, &packet[4], 256);
	/*
	printf("Data\n");
	for(i=0;i<262;i++)
	{
		if(i % 16 == 0)printf("\n");
		printf("0x%02X ",packet[i]);
		
	}*/
	crc16 = cal_crc(packet, 260);
	tcrc = (packet[260] << 8) | packet[261];
	PurgeComm(hSerial, PURGE_RXCLEAR);
	if(crc16 != tcrc)
	{
		//printf("\ncrc error %d, %d\n", crc16, tcrc);
		return -1;
	}
	else
	{
		//printf("\ncrc ok");
		return 0;
	}	
}

unsigned int set_chip_en()
{
	unsigned char packet[262] = {0};
	int i;
	DWORD bw,br;
	unsigned short crc16;
	
	packet[0] = CMD_WRITE_EN;
	crc16 = cal_crc(packet, 260);
	packet[260] = (crc16 >> 8) & 0xff;
	packet[261] = crc16 & 0xff;
	WriteFile(hSerial, packet, sizeof(packet), &bw, NULL);
	//printf("Byte to Write %d\n", bw);
	
	//Sleep(1000);
	//if(!ReadFile(hSerial, packet, sizeof(packet), &br, NULL))
	//{
		//error occurred. Report to user.
	//	printf("Read Error\n");
	//}
	/*
	for(i=0;i<262;i++)
	{
		if(i % 16 == 0)printf("\n");
		printf("0x%02X ",packet[i]);
		
	}
	*/
	read_response(packet);
	PurgeComm(hSerial, PURGE_RXCLEAR);
	return packet[1];
	
}

void serial_init()
{
	DCB dcbSerialParams = {0};
	hSerial = CreateFile("COM6",GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hSerial==INVALID_HANDLE_VALUE)
	{
		//some other error occurred. Inform user.
		printf("Open COM port fail.\n");
		if(GetLastError()==ERROR_FILE_NOT_FOUND)
		{
			//serial port does not exist. Inform user.
			printf("COM port not found.\n");
		}		
	}
	

	
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) 
	{
		//error getting state
		printf("GetCommState Error\n");
	}
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams))
	{
		printf("SetCommState Error\n");
	}
	if (!SetupComm(hSerial,2048,2048))
	{
		printf("Set Buffer size Error\n");
	}
	//SetupComm(hSerial, 1024, 1024);
}

unsigned int page_program(unsigned int address, unsigned char *ibuff)
{
	unsigned char packet[262] = {0};
	int i,loop;
	DWORD bw,br,tbr,cc;
	unsigned short crc16,tcrc;
	
	
	//printf("Byte to Write %d\n", bw);
	
	//Sleep(1000);
	br = 0;
	tbr=0;
	loop = 0;

	packet[0] = CMD_PAGE_WRITE;
	packet[1] = (address >> 16) & 0xff;
	packet[2] = (address >> 8) & 0xff;
	packet[3] = address & 0xff;
	
	memcpy(&packet[4], ibuff, 256);
	
	crc16 = cal_crc(packet, 260);
	packet[260] = (crc16 >> 8) & 0xff;
	packet[261] = crc16 & 0xff;
	//printf("Write cmd\n");
	WriteFile(hSerial, packet, sizeof(packet), &bw, NULL);
	
	//printf("Write cmd: Read response\n");	
	read_response(packet);
	//memcpy(ibuff, packet, 262);
	
	/*
	printf("Response Data\n");
	for(i=0;i<262;i++)
	{
		if(i % 16 == 0)printf("\n");
		printf("0x%02X ",packet[i]);
		
	}
	*/
	crc16 = cal_crc(packet, 260);
	tcrc = (packet[260] << 8) | packet[261];
	PurgeComm(hSerial, PURGE_RXCLEAR);
	if(crc16 != tcrc)
	{
		//printf("\ncrc error %d, %d\n", crc16, tcrc);
		return -1;
	}
	else
	{
		//printf("\ncrc ok");
		return 0;
	}	
}

unsigned int erase_block(unsigned int address)
{
	unsigned char packet[262] = {0};
	int i,loop;
	DWORD bw,br,tbr,cc;
	unsigned short crc16,tcrc;
	
	
	//printf("Byte to Write %d\n", bw);
	
	//Sleep(1000);
	br = 0;
	tbr=0;
	loop = 0;

	packet[0] = CMD_BLOCK_ERASE;
	packet[1] = (address >> 16) & 0xff;
	packet[2] = (address >> 8) & 0xff;
	packet[3] = address & 0xff;

	crc16 = cal_crc(packet, 260);
	packet[260] = (crc16 >> 8) & 0xff;
	packet[261] = crc16 & 0xff;
	WriteFile(hSerial, packet, sizeof(packet), &bw, NULL);
	
	read_response(packet);
	
	/*
	printf("Response Data\n");
	for(i=0;i<262;i++)
	{
		if(i % 16 == 0)printf("\n");
		printf("0x%02X ",packet[i]);
		
	}
	*/
	crc16 = cal_crc(packet, 260);
	tcrc = (packet[260] << 8) | packet[261];
	PurgeComm(hSerial, PURGE_RXCLEAR);
	if(crc16 != tcrc)
	{
		//printf("\ncrc error %d, %d\n", crc16, tcrc);
		return -1;
	}
	else
	{
		//printf("\ncrc ok");
		return 0;
	}	
}

int main()
{	
	unsigned char szBuff[n + 1] = {0};
	DWORD dwBytesRead = 0;
	DWORD dwBytesWrite = 0;
	unsigned short crc16;
	int i;
	unsigned int chip_id = 0,addr;
	FILE *fp;
	
	printf("Serial Port Init\n");
	serial_init();
	printf("Wait 3 sec\n");
	Sleep(3000);
	printf("Get Flash ID\n");
	chip_id = get_chip_id();
	printf("Flash Chip ID: %x\n", chip_id);
	return 0;	
	printf("\nErase Boot Block CMD\n");	
	set_chip_en();
	erase_block(0x0);
	Sleep(1000);
	printf("Write BootLoader\n");
	fp = fopen("mr3020.bin", "rb");
	for(addr = 0x00000; addr < 0x200;addr=addr+0x100)
	{
		printf("Address: 0x%05X       \r", addr);
		set_chip_en();
		fread(szBuff,sizeof(char),256,fp);
		page_program(addr,szBuff);
		Sleep(1000);
	}
	fclose(fp);
	
	fp = fopen("boot_dump.bin","wb");
	
	printf("\nDump bootloader to boot_dump.bin\n");	
	for(addr = 0x00000; addr < 0x200;addr=addr+0x100)
	{
		printf("Address: 0x%05X       \r", addr);
		//PurgeComm(hSerial, PURGE_RXCLEAR); //flush rx buf
		get_chip_data(addr, szBuff);
		fwrite(szBuff,sizeof(char),256,fp);	
		Sleep(1000);	
	}
	
	fclose(fp);
	
	
	printf("\nClose COM Port\n");
	CloseHandle(hSerial);
	return 0;
}
