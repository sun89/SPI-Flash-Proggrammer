#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<tchar.h>
#include"serial.h"

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
HANDLE *hSerial;

char app_name[255] = {0};

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

unsigned char sent_cmd(unsigned char cmd)
{
	char ocmd[3] = {0};
	ocmd[0] = cmd;
	ocmd[1] = cmd;
	serial_putbytes(ocmd, 2);
	return serial_getchar();
}

unsigned char sent_data(unsigned char data)
{
	serial_putchar(data);
	return serial_getchar();
}

unsigned char get_fw_ver()
{
	return sent_cmd(CMD_FW_VER);
}

unsigned char get_do_value()
{
	return sent_cmd(CMD_DO_READ);
}

int read_flash_id(unsigned char *buf)
{
  sent_cmd(CMD_CS_LOW); //cs_l();
  sent_cmd(CMD_DI_LOW); //di_l();
  Sleep(10); 
  sent_data(0x9f); //spi_write(0x9f); //Read Identification
  buf[0] = sent_data(0xff); //spi_read();
  buf[1] = sent_data(0xff); //spi_read();
  buf[2] = sent_data(0xff); //spi_read();
  sent_cmd(CMD_CK_LOW); //clk_l();
  sent_cmd(CMD_CS_HIGH); //cs_h();
  return 3;
}

int main(int argc, char *argv[])
{
    char tbuf[1000] = {0};
    printf("Initial COM6\n");
    serial_init(hSerial, "COM6");
    
    printf("Board Firmware Version: %02X\n", get_fw_ver());
    read_flash_id(tbuf);
    printf("Flash ID: %02X %02X %02X\n", tbuf[0], tbuf[1], tbuf[2]);
    
    system("PAUSE");	
    return 0;
}
