

#ifndef _BRIDGE
#define _BRIDGE




#define IN_RS_LEN 64


unsigned char main_address;
unsigned char in_rs_buffor[IN_RS_LEN];





    typedef enum
{
    IDLE,                     /* OCZEKUJE */
    LOAD,                   /*trwa transmisja */
    LOAD_END                      /*koniec ramki*/
} rxstatus;

volatile rxstatus rx_status;

volatile unsigned short usRcvBufferPos;

void modbus_execute(void);

void send_rf(void);


unsigned char frame_ok(void);

void init_modbus(void);

#endif
