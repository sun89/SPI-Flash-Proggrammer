#define WP    9
#define DI    11 
#define DO    12
#define CLK   13  
#define CS    10
#define HOLD  8

#define di_l()  digitalWrite(DI, LOW)
#define di_h()  digitalWrite(DI, HIGH)
#define clk_l()  digitalWrite(CLK, LOW)
#define clk_h()  digitalWrite(CLK, HIGH)
#define cs_l()  digitalWrite(CS, LOW)
#define cs_h()  digitalWrite(CS, HIGH)
#define hold_l()  digitalWrite(HOLD, LOW)
#define hold_h()  digitalWrite(HOLD, HIGH)
#define wp_l()  digitalWrite(WP, LOW)
#define wp_h()  digitalWrite(WP, HIGH)
#define do_value()  digitalRead(DO)

unsigned char spi_x(unsigned char d)
{
  int i;
  unsigned char dout = 0;
  for(i = 7;i >= 0; i--)
  {
    clk_l();
    //delay(10);
    if((d & (1 << i)))di_h();
    else di_l();
    if(do_value() == HIGH)dout = dout | (1 << i);
    clk_h();
    //delay(10);
  }
  return dout;
}

#define spi_write(xx)  spi_x(xx)
#define spi_read()  spi_x(0xff)
/*
void spi_write(unsigned char d)
{
  int i;
  for(i = 7;i >= 0; i--)
  {
    clk_l();
    //delay(10);
    if((d & (1 << i)))di_h();
    else di_l();
    clk_h();
    //delay(10);
  }
}
unsigned char spi_read()
{
  char d = 0,i;
  for(i = 7;i >= 0; i--)
  {
    clk_l();
    //delay(10);
    if(do_value() == HIGH)d = d | (1 << i);
    clk_h();
    //delay(10);
  }
  return d;
}
*/
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

int writeEnable()
{
  cs_l();
  di_l();
  delay(10); 
  spi_write(0x06); //write enable
  clk_l();
  cs_h();
  return 0;
}

int writeDisable()
{
  cs_l();
  di_l();
  delay(10); 
  spi_write(0x04); //write disable
  clk_l();
  cs_h();
  return 0;
}

unsigned char checkBlockProtect(long addr)
{
  cs_l();
  di_l();
  delay(10); 
  spi_write(0x3c); //block protect status
  spi_write((addr >> 16) & 0xff ); //Address 23:16 
  spi_write((addr >>  8) & 0xff); //Address 25:8
  spi_write(addr & 0xff); //Address  7:0
  unsigned char result = spi_read();
  clk_l();
  cs_h();
  return result;
}

unsigned char blockErase(long addr)
{
  cs_l();
  di_l();
  delay(10); 
  spi_write(0xD8); //block erase
  spi_write((addr >> 16) & 0xff ); //Address 23:16 
  spi_write((addr >>  8) & 0xff); //Address 25:8
  spi_write(addr & 0xff); //Address  7:0
  clk_l();
  cs_h();
  return 0;
}


int readStatusReg(unsigned char *buf)
{
  cs_l();
  di_l();
  delay(10); 
  spi_write(0x05); //Read Status register
  buf[0] = spi_read();
  clk_l();
  cs_h();
  return 2;
}

int readData2(unsigned char *buf, long addr, int br)
{
  cs_l();
  di_l();
  //delay(10); 
  spi_write(0x03); //Read Data
  spi_write((addr >> 16) & 0xff ); //Address 23:16 
  spi_write((addr >>  8) & 0xff); //Address 25:8
  spi_write(addr & 0xff); //Address  7:0
  int i = 0;
  while(br--)buf[i++] = spi_read();
  //buf[1] = spi_read();
  clk_l();
  cs_h();
  return i;
}
int read_data(unsigned char *buf, unsigned long addr) //read data block size 256 byte
{
  cs_l();
  di_l();
  //delay(10); 
  spi_write(0x03); //Read Data
  spi_write((addr >> 16) & 0xff ); //Address 23:16 
  spi_write((addr >>  8) & 0xff); //Address 15:8
  spi_write(addr & 0xff); //Address  7:0
  int i = 0;
  for(i = 0;i <= 255; i++)
  {
    buf[i] = spi_read();
  }
  clk_l();
  cs_h();
  return i;
}

int pageProgram(unsigned char *buf, unsigned long addr) //write data block size 256 byte
{
  cs_l();
  di_l();
  //delay(10); 
  spi_write(0x02); //write Data
  spi_write((addr >> 16) & 0xff ); //Address 23:16 
  spi_write((addr >>  8) & 0xff); //Address 15:8
  spi_write(addr & 0xff); //Address  7:0
  int i = 0;
  for(i = 0;i <= 255; i++)
  {
    spi_write(buf[i]);
  }
  clk_l();
  cs_h();
  return i;
}
boolean isWriteFlagClear()
{
  unsigned char b;
  readStatusReg(&b);
  if((b & 0x01) != 0)return false;
  else return true; 
}

unsigned char spi_buf[300] = {0};
unsigned char cmd_buf[30] = {0};
int cmd_idx = 0;

void setup() 
{ 
  Serial.begin(115200);
  //Serial.println("SPI Flash");  
  //pinMode(13, OUTPUT);     
  
  pinMode(DI, OUTPUT); 
  pinMode(DO, INPUT); 
  pinMode(CLK, OUTPUT); 
  pinMode(CS, OUTPUT); 
  pinMode(HOLD, OUTPUT); 
  pinMode(WP, OUTPUT); 
  
  //digitalWrite(HOLD, HIGH);
  //digitalWrite(WP, HIGH);
  
  cs_h();
  di_l();
  clk_l();
  hold_h();
  wp_h();
  /*
  delay(100);
  readID(spi_buf);
  Serial.print("Flash chip VID: 0x");
  Serial.print(spi_buf[0], HEX);
  Serial.print(" :PID 0x");
  Serial.print(spi_buf[1], HEX);
  Serial.println(spi_buf[2], HEX);  
  
  Serial.println("Data from 0x0 -> 0xff");
  read_data(spi_buf, 0x0);
  int i;
  for(i=0;i<256;i++)
  {
    Serial.print("0x");
    if(spi_buf[i] < 0x10)Serial.print("0");
    Serial.print(spi_buf[i], HEX);
    if(i % 16 == 0 && i != 0)Serial.println("");
    else Serial.print(", ");
  }
  Serial.println("");
  */
/* 
  int i;
  delay(500);
  readID(spi_buf);
  
  for(i=0;i<256;i++)
  {
    spi_buf[i] = i;
  }
  writeEnable();
  blockErase(0x0);
  delay(500);
  writeEnable();
  pageProgram(spi_buf, 0x0);
  
  Serial.println("\r\nData from 0x0 -> 0xff");
  delay(100);
  read_data(spi_buf, 0x0);
  for(i=0;i<256;i++)
  {
    if(i % 16 == 0)Serial.println("");
    Serial.print("0x");
    if(spi_buf[i] < 0x10)Serial.print("0");
    Serial.print(spi_buf[i], HEX);    
    Serial.print(", ");
  }
  Serial.println("");
  

//==================================  
   writeEnable();
  for(i=0;i<256;i++)
  {
    if(i % 2 == 0)spi_buf[i] = 0x33;
    else spi_buf[i] = 0x55;
  }
  pageProgram(spi_buf, 0x100);
  
  Serial.println("\r\nData from 0x100 -> 0x1ff");
  read_data(spi_buf, 0x100);
  for(i=0;i<256;i++)
  {
    if(i % 16 == 0)Serial.println("");
    Serial.print("0x");
    if(spi_buf[i] < 0x10)Serial.print("0");
    Serial.print(spi_buf[i], HEX);    
    Serial.print(", ");
  }
  Serial.println("");

//=========================  
   writeEnable();
  for(i=0;i<256;i++)
  {
    spi_buf[i] = ~i;
  }
  pageProgram(spi_buf, 0x200);
  
  Serial.println("\r\nData from 0x200 -> 0x2ff");
  read_data(spi_buf, 0x200);
  for(i=0;i<256;i++)
  {
    if(i % 16 == 0)Serial.println("");
    Serial.print("0x");
    if(spi_buf[i] < 0x10)Serial.print("0");
    Serial.print(spi_buf[i], HEX);    
    Serial.print(", ");
  }
  Serial.println("");
*/ 
/*  
  Serial.println("Enable Writing");
  writeEnable();
  
  readStatusReg(spi_buf);
  Serial.print("Status Reg: ");
  Serial.println(spi_buf[0], HEX);
  
  read_data(spi_buf, 0x20000);
  showBlockData(0x20000);
  
  while( Serial.read() != 'S');
  Serial.println("Erase block 2");
  blockErase(0x20000);
  showBlockData(0x20000);
  
  readStatusReg(spi_buf);
  Serial.print("Status Reg: ");
  Serial.println(spi_buf[0], HEX);
  
  delay(1000);
  
   readStatusReg(spi_buf);
  Serial.print("Status Reg: ");
  Serial.println(spi_buf[0], HEX);
  
  Serial.println("\nPage Program");
  Serial.println("Write Enable");
  writeEnable();
  for(int k=0;k<256;k++)
  {
    spi_buf[k] = millis() & 0xff;
  }
  pageProgram(spi_buf, 0x20000);
  Serial.println("Wait Write flag clear");
  readStatusReg(spi_buf);
  while(isWriteFlagClear() == false);
  
  showBlockData(0x20000);
*/ 
  
}

void showBlockData(long addr)
{
  Serial.print("Data at 0x");
  Serial.println(addr, HEX);
  read_data(spi_buf, addr);
  for(int k =0;k < 256;k++)
  {
    Serial.print(spi_buf[k], HEX);
    Serial.print(" ");
    if((k % 16 == 0) && (k != 0))Serial.println("");
  }
  Serial.println("");
}

unsigned int cal_crc(unsigned char *buf, int len)
{
  unsigned int crc = 0;
  int i;
  for(i = 0;i < len; i++)
  {
    crc = crc + buf[i];
  }
  return crc & 0xffff;
}
#define WAIT     0
#define PROCESS  1
int loop_state = WAIT;

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


#define CMD_FAIL    1
#define CMD_FINISH  2

unsigned char cmd_type = CMD_NONE;
unsigned long cmd_address = 0x0;
unsigned char cmd_len = 0;
unsigned int cmd_crc16 = 0,crc16;
unsigned char cmd_result = CMD_FAIL;
unsigned int tmp_crc ;
int i;

//#define WRITE_REQUEST  1
//#define WRITE_DATA     2
//int page_write_state = WRITE_REQUEST;
unsigned char inp;

//-------------------------------------------------------------------------------------------
int cmd_chip_info_response()
{
  spi_buf[0] = CMD_CHIP_INFO;
  i = 1;
  while(i < 262)
  {
    if(Serial.available() > 0)spi_buf[i++] = Serial.read();
  }    
  crc16 = cal_crc(spi_buf, 260);
  tmp_crc = (spi_buf[260] << 8) | spi_buf[261];
  if(crc16 != tmp_crc)
  {
    return -1;
  }
  readID(&spi_buf[1]);
        
  for(i = 4;i < 260;i++)spi_buf[i] = 0;
  crc16 = cal_crc(spi_buf, 260);
  spi_buf[260] = (crc16 >> 8) & 0xff;
  spi_buf[261] = crc16 & 0xff;
  Serial.write(spi_buf, 262);
  return 0;
}

//-------------------------------------------------------------------------------------------
int cmd_read_data_response() //read data 256 byte
{
  spi_buf[0] = CMD_READ_DATA;
  int i = 1;
  while(i < 262)
  {
    if(Serial.available() > 0)spi_buf[i++] = Serial.read();
  }  
  
  crc16 = cal_crc(spi_buf, 260);
  tmp_crc = (spi_buf[260] << 8) | spi_buf[261];
  if(crc16 != tmp_crc)
  {
    memset(&spi_buf[1], 0xff,259);
    crc16 = cal_crc(spi_buf, 260);
    spi_buf[260] = (crc16 >> 8) & 0xff;
    spi_buf[261] = crc16 & 0xff;
    Serial.write(spi_buf, 262);
    return -1;
  }
    
  unsigned long addr = ((unsigned long)spi_buf[1] << 16) | ((unsigned long)spi_buf[2] << 8) | ((unsigned long)spi_buf[3]);
  read_data(&spi_buf[4], addr);
  //int i;
  /*
  for(i=0;i<256;i++)
  {
    Serial.print("0x");
    if(spi_buf[i] < 0x10)Serial.print("0");
    Serial.print(spi_buf[i], HEX);
    if(i % 16 == 0 && i != 0)Serial.println("");
    else Serial.print(", ");
  }
  Serial.println("");
  */
  //readID(&spi_buf[1]);
        
  //for(i = 4;i < 260;i++)spi_buf[i] = 0;
  crc16 = cal_crc(spi_buf, 260);
  spi_buf[260] = (crc16 >> 8) & 0xff;
  spi_buf[261] = crc16 & 0xff;
  Serial.write(spi_buf, 262);
  return 0;
}

//------------------------------------------------------------------------------------------
int cmd_write_en_response()
{
  spi_buf[0] = CMD_WRITE_EN;
  int i = 1;
  while(i < 262)
  {
    if(Serial.available() > 0)spi_buf[i++] = Serial.read();
  }  
  
  crc16 = cal_crc(spi_buf, 260);
  tmp_crc = (spi_buf[260] << 8) | spi_buf[261];
  if(crc16 != tmp_crc)
  {
    memset(&spi_buf[1], 0xff,259);
    crc16 = cal_crc(spi_buf, 260);
    spi_buf[260] = (crc16 >> 8) & 0xff;
    spi_buf[261] = crc16 & 0xff;
    Serial.write(spi_buf, 262);
    return -1;
  }
  
  if(writeEnable() == 0)
  {
    spi_buf[1] = 0;
  }
  else
  {
    spi_buf[1] = 0xff;
  }
  crc16 = cal_crc(spi_buf, 260);
  spi_buf[260] = (crc16 >> 8) & 0xff;
  spi_buf[261] = crc16 & 0xff;
  Serial.write(spi_buf, 262);
}

//-------------------------------------------------------------------------------------------
void cmd_write_page_response()
{
  spi_buf[0] = CMD_PAGE_WRITE;
  int i = 1;
  while(i < 262)
  {
    if(Serial.available() > 0)spi_buf[i++] = Serial.read();
  }  
  
  crc16 = cal_crc(spi_buf, 260);
  tmp_crc = (spi_buf[260] << 8) | spi_buf[261];
  if(crc16 != tmp_crc)
  {
    memset(&spi_buf[1], 0xff,259);
    crc16 = cal_crc(spi_buf, 260);
    spi_buf[260] = (crc16 >> 8) & 0xff;
    spi_buf[261] = crc16 & 0xff;
    Serial.write(spi_buf, 262);
    return ;
  }
  unsigned long addr = ((unsigned long)spi_buf[1] << 16) | ((unsigned long)spi_buf[2] << 8) | ((unsigned long)spi_buf[3]);
  pageProgram(&spi_buf[4], addr);
  memset(&spi_buf[1], 0, 261); //fill zero to result packet
  
  crc16 = cal_crc(spi_buf, 260);
  spi_buf[260] = (crc16 >> 8) & 0xff;
  spi_buf[261] = crc16 & 0xff;
  Serial.write(spi_buf, 262);
}

//-------------------------------------------------------------------------------------------
void cmd_block_erase_response()
{
  spi_buf[0] = CMD_BLOCK_ERASE;
  int i = 1;
  while(i < 262)
  {
    if(Serial.available() > 0)spi_buf[i++] = Serial.read();
  }  
  
  crc16 = cal_crc(spi_buf, 260);
  tmp_crc = (spi_buf[260] << 8) | spi_buf[261];
  if(crc16 != tmp_crc)
  {
    memset(&spi_buf[1], 0xff,259);
    crc16 = cal_crc(spi_buf, 260);
    spi_buf[260] = (crc16 >> 8) & 0xff;
    spi_buf[261] = crc16 & 0xff;
    Serial.write(spi_buf, 262);
    return ;
  }
  unsigned long addr = ((unsigned long)spi_buf[1] << 16) | ((unsigned long)spi_buf[2] << 8) | ((unsigned long)spi_buf[3]);
  blockErase(addr);
  memset(&spi_buf[1], 0, 261); //fill zero to result packet
  
  crc16 = cal_crc(spi_buf, 260);
  spi_buf[260] = (crc16 >> 8) & 0xff;
  spi_buf[261] = crc16 & 0xff;
  Serial.write(spi_buf, 262);
}

//-------------------------------------------------------------------------------------------
void loop() {
  if(Serial.available() > 0)
  {
    inp = Serial.read();
    switch(inp)
    {
      case CMD_CHIP_INFO:
        cmd_chip_info_response();
        break;
      case CMD_READ_DATA:
        cmd_read_data_response();
        break;
      case CMD_WRITE_EN:
        cmd_write_en_response();
        break;
      case CMD_PAGE_WRITE:
        cmd_write_page_response();
        break;
      case CMD_BLOCK_ERASE:
        cmd_block_erase_response();
        break;
        
      default: Serial.println("Invalid cmd");
    }
    
    while(Serial.available() > 0)Serial.read();
  }
  


}




