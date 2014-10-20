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


void setup() 
{ 
  Serial.begin(115200);   
  
  pinMode(DI, OUTPUT); 
  pinMode(DO, INPUT); 
  pinMode(CLK, OUTPUT); 
  pinMode(CS, OUTPUT); 
  pinMode(HOLD, OUTPUT); 
  pinMode(WP, OUTPUT); 
  
  cs_h();
  di_l();
  clk_l();
  hold_h();
  wp_h();
  
}

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




