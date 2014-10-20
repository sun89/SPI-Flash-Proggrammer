#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include <tchar.h>

#define CMD_CS_LOW  	0x30
#define CMD_CS_HIGH  	0x31
#define CMD_DI_LOW  	0x32
#define CMD_DI_HIGH  	0x33
#define CMD_CK_LOW  	0x34
#define CMD_CK_HIGH  	0x35

#define n	1024
HANDLE hSerial;

void serial_init(char *port)
{
	DCB dcbSerialParams = {0};
	hSerial = CreateFile(port,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
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

void uart_putchar(char ch)
{
	DWORD bw;
	WriteFile(hSerial, &ch, 1, &bw, NULL);	
}

char uart_getchar()
{
	DWORD br;
	char ch;
	do
	{
		ReadFile(hSerial, &ch, 1, &br, NULL);
	}
	while(br < 1);
	
	return ch;
}

int str_to_bytes(char *str, unsigned char *bytes)
{
	const char *pos = str;
    unsigned char *val = bytes;
    size_t count = 0;
	for(count = 0; count <= sizeof(val)/sizeof(val[0]); count++) {
        sscanf(pos, "%2hhx", &val[count]);
        pos += 2 * sizeof(char);
    }
    return count;
}
int is_hex(char ch)
{
	if(ch >= '0' && ch <= '9')return 1;
	if(ch >= 'a' && ch <= 'f')return 1;
	if(ch >= 'F' && ch <= 'F')return 1;
	return 0;
}
void process_cmd(char *sbuf)
{
	int i;
	unsigned char bytes[30] = {0};
	int bcount = 0;	
	char tmp[3] = {0}, *inptr;
	strncpy(tmp, sbuf, 2);
	if((strcmp(tmp, "cs") == 0) || (strcmp(tmp, "CS") == 0)) //set CS pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		return;
	}
	
	else if((strcmp(tmp, "di") == 0) || (strcmp(tmp, "DI") == 0)) //set CS pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		return;
	}
	
	else if((strcmp(tmp, "CK") == 0) || (strcmp(tmp, "CK") == 0)) //set CS pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		return;
	}
	else if(is_hex(tmp[0]) && is_hex(tmp[1])) //sent hex data
	{
	}
	else //invalid cmd
	{
	}
	
	bcount = str_to_bytes(sbuf, bytes);
	printf("%s\n", sbuf);
	
	for(i = 0;i < bcount;i++)
	{
		printf("0x%02x ", bytes[i]);
	}
	printf("\n");
}
int main()
{	
	char sbuf[100], ch;
	int nbyte = 0;
	printf("Serial Port Init\n");
	serial_init("COM6");
	printf("Wait 3 sec\n");
	Sleep(1000);
	printf("Ready\n");
	while(1)
	{
		if((nbyte % 2 == 0) && (nbyte != 0))putchar(' '); //seperate byte
		Sleep(100);
		
		ch = getch();
		//printf("0x%02X ", ch);
		putchar(ch);
		if(ch == '\r') //Process input cmd
		{
			putchar('\n');
			sbuf[nbyte] = '\0';
			process_cmd(sbuf);
			nbyte = 0;
		}
		else
		{
			sbuf[nbyte++] = ch;
			
		}
		//printf(".");		
	}
	return 0;	
}
