#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<tchar.h>
#include<stdint.h>
#include <time.h>
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

int read_data(unsigned char *buf, uint32_t addr) //read data block size 256 byte
{
    int i = 0;
    sent_cmd(CMD_CS_LOW); //cs_l();
    sent_cmd(CMD_DI_LOW); //di_l();
    //delay(10); 
    sent_data(0x03); //spi_write(0x03); //Read Data
    sent_data((addr >> 16) & 0xff ); //Address 23:16 
    sent_data((addr >>  8) & 0xff); //Address 15:8
    sent_data(addr & 0xff); //Address  7:0
    //printf("Address: %02X %02X %02X\n",(addr >> 16) & 0xff, (addr >> 8) & 0xff, addr & 0xff);
    for(i = 0;i <= 0xff; i++)
    {
          buf[i] = sent_data(0xff);
    }
    sent_cmd(CMD_CK_LOW); //clk_l();
    sent_cmd(CMD_CS_HIGH); //cs_h();
    return i;
}

void print_buf(char *buf, int size)
{
     int i;
     for(i = 0;i < size;i++)
     {
         if(i % 16 == 0 && i != 0)printf("\n");
         printf("0x%02X ", (unsigned char)buf[i]);
     }
     printf("\n");
}
FILE *fp;
int main(int argc, char *argv[])
{
    char tbuf[100000] = {0};
    int i, br;
    clock_t start, diff;
    int msec;
    printf("Initial COM6\n");
    serial_init(hSerial, "COM6");
    
    printf("Board Firmware Version: %02X\n", get_fw_ver());
    read_flash_id(tbuf);
    printf("Flash ID: %02X %02X %02X\n", tbuf[0], tbuf[1], tbuf[2]);
    
    printf("Dump Bootloader(0x0 - 0xFFFF) to File(Bootldr.bin)\n");
    fp = fopen("Bootldr.bin", "wb");
    start = clock();
    for(i = 0;i < 0xffff;i = i+0x100)
    {
        printf("Read 0x%08X to 0x%08X\n", i, i+0xff);
        br = read_data(tbuf, i);
        fwrite(tbuf,sizeof(char), br, fp);
        //print_buf(tbuf, 256);
    }
    diff = clock() - start;
    printf("Dump Complete.\n");
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
    fclose(fp);
    system("PAUSE");	
    return 0;
}
