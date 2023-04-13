  
#include <xc.h> // include processor files - each processor file is guarded.  
#define _XTAL_FREQ 20000000

    /*for a 9600 Baud rate...
     * Desired Baud Rate = FOSC/(64 ([SPBRGH:SPBRG] + 1))  
     * solving for SPBRGH:SPBRG...
     * X = (FOSC/(64*DBR)) - 1
     * X = 77.125 =~ 77
     * It gives a baud rate of 9615 bits/s
     */


void uart_init(unsigned long baud_rate);
void uart_tx(char data);
void uart_send_string(char *s);

