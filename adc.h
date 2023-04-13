#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#define _XTAL_FREQ 20000000 //Frecuencia del XTAL oscilador

void adc_init(void);
uint16_t read_adc(uint8_t channel);