#include <xc.h>
#define _XTAL_FREQ 20000000 //Frecuencia del XTAL oscilador

#include <stdint.h>

#define TC_CLK  LATBbits.LATB1     //PIN_B1            
#define TC_CS   LATBbits.LATB2     //PIN_B2 
#define TC_DATA PORTBbits.RB0     //PIN_B0  

void init_TC(void);               
float read_TC(void);
uint8_t spiread(void);
//void shift_left(uint16_t *datos);