

#ifndef _BRIDGE
#define _BRIDGE



/*       RS232/485      */
#define IN_RS_LEN 64
unsigned char in_rs_buffor[IN_RS_LEN];


/*       MODBUS     */
unsigned char main_address;
#define HOLDING_REG_LEN 10
#define HOLDING_REG_START 1000
unsigned char holding_reg[HOLDING_REG_LEN];





    typedef enum
{
    IDLE,                     /* OCZEKUJE */
    LOAD,                   /*trwa transmisja */
    LOAD_END                      /*koniec ramki*/
} rxstatus;

volatile rxstatus rx_status;

volatile unsigned short usRcvBufferPos;

void modbus_execute(void);

void send_radio(void);


unsigned char frame_ok(void);

void init_modbus(void);

#endif
