#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include <tchar.h>

#define CMD_CS_LOW  	0x33
#define CMD_CS_HIGH  	0x55
#define CMD_DI_LOW  	0x66
#define CMD_DI_HIGH  	0x77
#define CMD_CK_LOW  	0x88
#define CMD_CK_HIGH  	0x99

#define n	1024
HANDLE hSerial;

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

int readID(unsigned char *buf)
{
  cs_l();
  di_l();
  delay(10); 
  spi_write(0x9f); //Read Identification
  buf[0] = spi_read();
  buf[1] = spi_read();
  buf[2] = spi_read();
  clk_l();
  cs_h();
  return 3;
}

int main()
{	
	printf("Serial Port Init\n");
	serial_init();
	printf("Wait 3 sec\n");
	Sleep(3000);
	printf("Get Flash ID\n");
	chip_id = get_chip_id();
	printf("Flash Chip ID: %x\n", chip_id);
	return 0;	
}
