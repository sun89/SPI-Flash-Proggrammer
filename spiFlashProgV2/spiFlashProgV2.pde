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

#define CMD_CS_LOW  0x33
#define CMD_CS_HIGH  0x55
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
void spi_init()
{
  pinMode(DI, OUTPUT); 
  pinMode(DO, INPUT); 
  pinMode(CLK, OUTPUT); 
  pinMode(CS, OUTPUT); 
  pinMode(HOLD, OUTPUT); 
  pinMode(WP, OUTPUT); 
  
  cs_h();
  di_l();
  clk_l();
}

void setup() 
{ 
  Serial.begin(115200);   
  spi_init();
  hold_h();
  wp_h();
  
}
char ch,spi_byte;
void loop() {
  if(Serial.available() > 0)
  {
    ch = Serial.read();
    if(ch == CMD_CS_LOW) //may be cs low cmd
    {
      if(Serial.available() > 0)
      {
        ch = Serial.read();
        cs_l();
      }
      spi_byte = spi_x(ch);      
    }
    else if(ch == CMD_CS_HIGH) //may be cs high cmd
    {
      if(Serial.available() > 0)
      {
        ch = Serial.read();
        cs_h();
      }
      spi_byte = spi_x(ch); 
    }
    else //normal byte
    {
      spi_byte = spi_x(ch); 
    }
    while(Serial.available() > 0)Serial.read(); //clear buffer
    Serial.write(spi_byte);
  }
  


}




