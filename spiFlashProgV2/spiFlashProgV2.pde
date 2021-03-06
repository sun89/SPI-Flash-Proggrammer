

#include <SPI.h>

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

#define CMD_CS_LOW  0x30
#define CMD_CS_HIGH  0x31
#define CMD_DI_LOW  0x32
#define CMD_DI_HIGH  0x33
#define CMD_CK_LOW  0x34
#define CMD_CK_HIGH  0x35
#define CMD_HO_LOW  0x36
#define CMD_HO_HIGH  0x37
#define CMD_WP_LOW  0x38
#define CMD_WP_HIGH  0x39
#define CMD_DO_READ  0x40

#define UART_DELAY_US  10

char *date = __DATE__;
char *time = __TIME__;
char fw_ver = 0x20;

#define CMD_FW_VER  	0x20
#define CMD_FW_DATE	0x21
/*
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
*/
#define spi_x(xr)  SPI.transfer(xr)
void spi_init()
{
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  //pinMode(DI, OUTPUT); 
  //pinMode(DO, INPUT); 
  //pinMode(CLK, OUTPUT); 
  pinMode(CS, OUTPUT); 
  pinMode(HOLD, OUTPUT); 
  pinMode(WP, OUTPUT); 
  
  cs_h();
  di_l();
  clk_l();
}

void setup() 
{ 
  Serial.begin(500000);   
  //Serial.println("Hello");
  spi_init();
  hold_h();
  wp_h();
  
}
char ch,spi_byte;
void loop() {
  if(Serial.available() > 0)
  {
    ch = Serial.read();
    if(ch == CMD_FW_VER) //may be version chk cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = fw_ver;                
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_CS_LOW) //may be cs low cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_CS_LOW;
        cs_l();         
      }
      else
      {
        spi_byte = spi_x(ch);
      }          
    }
    else if(ch == CMD_CS_HIGH) //may be cs high cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_CS_HIGH;
        cs_h();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_DI_HIGH) //may be DI high cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_DI_HIGH;
        di_h();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_DI_LOW) //may be DI low cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_DI_LOW;
        di_l();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_CK_HIGH) //may be CK high cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_CK_HIGH;
        clk_h();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_CK_LOW) //may be CK LOW cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_CK_LOW;
        clk_l();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_HO_HIGH) //may be hold high cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_HO_HIGH;
        hold_h();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_HO_LOW) //may be Hold LOW cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_HO_LOW;
        hold_l();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_WP_HIGH) //may be WP high cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_WP_HIGH;
        wp_h();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_WP_LOW) //may be WP LOW cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = CMD_WP_LOW;
        wp_l();                 
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    else if(ch == CMD_DO_READ) //may be read DO cmd
    {
      delayMicroseconds(UART_DELAY_US);
      if(Serial.available() > 0)
      {
        spi_byte = do_value();                
      }
      else
      {
        spi_byte = spi_x(ch);
      } 
    }
    
    else //normal byte
    {
      spi_byte = spi_x(ch); 
    }
    while(Serial.available() > 0)Serial.read(); //clear buffer
    Serial.write(spi_byte);
  }
  


}




