#include <avr/io.h>
#include "bridge.h"
#include "bridge_usart.h"
#include "port.h"
#include "mbcrc.h"


unsigned long usTimerT35_50us;

unsigned char frame_ok(void)
{
if(usMBCRC16(&in_rs_buffor[0], (unsigned short)usRcvBufferPos)==0)
return 1;
else
return 0;
}

void modbus_execute(void)
{
unsigned char func_code;

func_code = in_rs_buffor[1];

switch(func_code)
	case 2:
	break;

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
