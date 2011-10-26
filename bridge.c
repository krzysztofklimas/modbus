#include <avr/io.h>
#include "bridge.h"
#include "bridge_usart.h"
#include "port.h"

unsigned long usTimerT35_50us;

unsigned char frame_ok(void)
{

return 1;
}

void modbus_execute(void)
{

}

void send_rf(void)
{

}

void poll(void)
{

switch(rx_status)
  {
  case IDLE:
  break;
  
  case LOAD:
  break;
  
  case LOAD_END:
  if(frame_ok())
    {
    if(main_address == in_rs_buffor[1])
      {
      modbus_execute();
      }
     else
      {
      send_rf();
      }
    }
	rx_status = IDLE;
  break;
  }
}

int main(void)
{
usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * 9600 );
xMBPortTimersInit(usTimerT35_50us);
//init_uart();
xMBPortSerialInit( 0, 9600, 8, MB_PAR_EVEN );
init_modbus();

while(1)
{
poll();
}

}








