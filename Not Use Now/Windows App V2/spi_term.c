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
#define CMD_HO_LOW  	0x36
#define CMD_HO_HIGH  	0x37
#define CMD_WP_LOW  	0x38
#define CMD_WP_HIGH  	0x39
#define CMD_DO_READ  	0x40

#define SW_VER			2.0.0
#define CMD_FW_VER  	0x20
#define CMD_FW_DATE		0x21

#define n	1024
HANDLE hSerial;

int m_argc;
char **m_argv;

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

void uart_putchar(unsigned char ch)
{
	DWORD bw;
	WriteFile(hSerial, &ch, 1, &bw, NULL);	
}

void uart_putbytes(unsigned char *buf, int len)
{
	DWORD bw;
	WriteFile(hSerial, buf, len, &bw, NULL);
}
unsigned char uart_getchar()
{
	DWORD br;
	char str[100];
	do
	{
		ReadFile(hSerial, &str[0], 1, &br, NULL);
	}
	while(br < 1);
	
	return str[0];
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

unsigned char sent_packet(unsigned char cmd)
{
	char ocmd[3] = {0};
	ocmd[0] = cmd;
	ocmd[1] = cmd;
	uart_putbytes(ocmd, 2);
	return uart_getchar();
}

unsigned char get_fw_ver()
{
	return sent_packet(CMD_FW_VER);
}

unsigned char get_do_value()
{
	return sent_packet(CMD_DO_READ);
}

void usage()
{
	char *filename = &m_argv[0][strlen(m_argv[0]) - 1];
	while(*filename != '\\')
	{
		filename--;
	}
	filename++;
	printf("\tSPI Termial Application(Baudrate 115200)\n");
	printf("\tUsage: %s COM_Port\n",filename);
	printf("\tEx: %s COM1\n\n", filename);
}

void cmd_help()
{
}

void process_cmd(char *sbuf)
{
	int i;
	unsigned char bytes[30] = {0}, rx_spi;
	int bcount = 0;	
	char tmp[3] = {0}, *inptr,ocmd[100] = {0};
	strncpy(tmp, sbuf, 2);
	if((strcmp(tmp, "cs") == 0) || (strcmp(tmp, "CS") == 0)) //set CS pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		if(bytes[0] == 0)rx_spi = sent_packet(CMD_CS_LOW);
		else rx_spi = sent_packet(CMD_CS_HIGH);
		printf("%02X\n", rx_spi);
	}
	
	else if((strcmp(tmp, "di") == 0) || (strcmp(tmp, "DI") == 0)) //set DI pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		if(bytes[0] == 0)rx_spi = sent_packet(CMD_DI_LOW);
		else rx_spi = sent_packet(CMD_DI_HIGH);
		printf("%02X\n", rx_spi);
	}
	
	else if((strcmp(tmp, "ck") == 0) || (strcmp(tmp, "CK") == 0)) //set CLK pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		if(bytes[0] == 0)rx_spi = sent_packet(CMD_CK_LOW);
		else rx_spi = sent_packet(CMD_CK_HIGH);
		printf("%02X\n", rx_spi);
	}
	else if((strcmp(tmp, "ho") == 0) || (strcmp(tmp, "HO") == 0)) //set HOLD pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		if(bytes[0] == 0)rx_spi = sent_packet(CMD_HO_LOW);
		else rx_spi = sent_packet(CMD_HO_HIGH);
		printf("%02X\n", rx_spi);
	}
	else if((strcmp(tmp, "wp") == 0) || (strcmp(tmp, "WP") == 0)) //set WP pin CMD
	{
		inptr = &sbuf[2];
		bcount = str_to_bytes(inptr, bytes);
		if(bytes[0] == 0)rx_spi = sent_packet(CMD_WP_LOW);
		else rx_spi = sent_packet(CMD_WP_HIGH);
		printf("%02X\n", rx_spi);
	}
	else if(is_hex(tmp[0]) && is_hex(tmp[1])) //sent hex data
	{
		inptr = &sbuf[0];
		bcount = str_to_bytes(inptr, bytes);
		uart_putchar(bytes[0]); //sent data
		printf("%02X ", (unsigned char)uart_getchar());
		inptr = &sbuf[2];
		if(strlen(inptr) > 0)process_cmd(inptr); //recursion to next byte
		printf("\n");
	}
	else if((strcmp(tmp, "ve") == 0) || (strcmp(tmp, "VE") == 0)) //get version CMD
	{
		printf("%02X\n", (unsigned char)get_fw_ver());
	}
	else if((strcmp(tmp, "do") == 0) || (strcmp(tmp, "DO") == 0)) //get do pin CMD
	{
		printf("%02X\n", (unsigned char)get_do_value());
	}
	else if((strcmp(tmp, "ex") == 0) || (strcmp(tmp, "EX") == 0)) //exit app CMD
	{
		printf("\nClose COM Port\n");
		CloseHandle(hSerial);
		exit(0);
	}
	else //invalid cmd
	{
		printf("\n\tInvalid cmd\n");
		cmd_help();
	}
	PurgeComm(hSerial, PURGE_RXCLEAR);
}

int main(int argc, char *argv[])
{	
	char sbuf[100], ch;
	int nbyte = 0;
	
	m_argc = argc;
	m_argv = argv;
	
	if(argc != 3)
	{
		usage();
		return -1;
	}
	printf("Serial Port Init\n");
	serial_init(argv[1]);
	printf("Wait 1 sec\n");
	Sleep(1000);
	printf("Firmware version 0x%02X\n", get_fw_ver());
	printf("Ready\n");
	printf("# ");
	while(1)
	{
		if((nbyte % 2 == 0) && (nbyte != 0))putchar(' '); //seperate byte
		Sleep(100);
		
		ch = getch();
		putchar(ch);
		if(ch == '\r') //Process input cmd
		{
			putchar('\n');
			sbuf[nbyte] = '\0';
			printf("-> ");
			process_cmd(sbuf);
			nbyte = 0;
			printf("# ");
		}
		else
		{
			sbuf[nbyte++] = ch;
			
		}	
	}
	printf("\nClose COM Port\n");
	CloseHandle(hSerial);
	return 0;	
}
