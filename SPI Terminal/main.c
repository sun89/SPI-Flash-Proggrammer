#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#ifdef linux
#include<unistd.h>
#define mSleep(x)	usleep(x*1000UL)
//typedef unsigned long	DWORD
#else
#include<windows.h>
#include<tchar.h>
#define mSleep(x)	Sleep(x)
#endif

#include "rs232.h"

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

#ifndef getch
/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}


#endif

char app_name[255] = {0};
int cport_nr=5;        /* (COM5 on windows) */

#define uart_putbytes(xx, yy)  RS232_SendBuf(cport_nr, xx, yy)
#define uart_putchar(xxx) RS232_SendByte(cport_nr, xxx)
unsigned char uart_getchar()
{
	unsigned long br;
	char str[100];
	do
	{
		br = RS232_PollComport(cport_nr, str, 1);
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
	char *filename = &app_name[strlen(app_name) - 1];
	while(*filename != '\\')
	{
		filename--;
	}
	filename++;
	printf("\tSPI Termial Application(Baudrate 115200)\n");
	printf("\tUsage: %s COM_Port\n",app_name);
	printf("\tEx: %s COM1\n\n", app_name);
}

void cmd_help()
{
     printf("\t\tCommand Help\n");
     printf("\t\t------------\n");
     printf("\t%%Hex Number%% : Sent 1 Byte to Device\n");
     printf("\tCS [0 | 1] : Set CS   Pin\n");
     printf("\tDI [0 | 1] : Set DI   Pin\n");
     printf("\tCK [0 | 1] : Set CLK  Pin\n");
     printf("\tHO [0 | 1] : Set HOLD Pin\n");
     printf("\tWP [0 | 1] : Set WP   Pin\n");
     printf("\tDO : Read DO Pin\n");
     printf("\tVE : Read Board Firmware Version\n");
     printf("\tX : Exit App\n");
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
		else printf("\n");
	}
	else if((strcmp(tmp, "ve") == 0) || (strcmp(tmp, "VE") == 0)) //get version CMD
	{
		printf("%02X\n", (unsigned char)get_fw_ver());
	}
	else if((strcmp(tmp, "do") == 0) || (strcmp(tmp, "DO") == 0)) //get do pin CMD
	{
		printf("%02X\n", (unsigned char)get_do_value());
	}
	else if((strcmp(tmp, "x") == 0) || (strcmp(tmp, "X") == 0)) //exit app CMD
	{
		printf("\nClose COM Port\nExit App\n");
		RS232_CloseComport(cport_nr); //CloseHandle(hSerial);
		exit(0);
	}
	else //invalid cmd
	{
		printf("Invalid cmd\n\n");
		cmd_help();
	}
	//PurgeComm(hSerial, PURGE_RXCLEAR);
}
int find_first_num(char *str)
{
    char *ptr = str;
    int i=0;
    while(!isdigit(*ptr))
    {
        i++;
        ptr++;
    }
    return i;
}

int main(int argc, char *argv[])
{
	char sbuf[100], ch;
	int nbyte = 0;
	int bdrate=500000;       /* 9600 baud */
	char com_name[20] = {0};

    char mode[]={'8','N','1',0},  str[2][512];
    strcpy(app_name, argv[0]);

	if(argc != 2)
	{
		usage();
		return 0;
	}
	printf("\n\n\t\t\tSPI Termial Application\n");
	printf("\t\t\t-----------------------\n");
	strcpy(com_name, argv[1]);
	cport_nr = atoi(&com_name[find_first_num(com_name)]);
	cport_nr--;
	sprintf(sbuf, "COM%d", cport_nr + 1);
	printf("Serial Port %s Initial...\n", sbuf);
	if(RS232_OpenComport(cport_nr, bdrate, mode))
    {
        printf("Can not open comport\n");
        return(0);
    }
    mSleep(3000);

    printf("Board Firmware Version: ");
    printf("%02X\n", get_fw_ver());
	printf("Ready\n");
	printf("# ");
	while(1)
	{
		if((nbyte % 2 == 0) && (nbyte != 0))putchar(' '); //seperate byte
		mSleep(100);

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
	RS232_CloseComport(cport_nr); //CloseHandle(hSerial);
	return 0;
}
