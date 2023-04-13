
#include "termopar.h"

/*
  TC_CLK     PIN_B1            
  TC_CS      PIN_B2 
  TC_DATA    PIN_B0  
 */

void init_TC(void) {

//configurar los pines definidos como CLK, DATA y CS como salidas digitales
    TRISBbits.RB1=0;
    TRISBbits.RB2=0;
    TRISBbits.RB0=1;

   TC_CLK=0;    //cero lógico la salida de este pin 
   TC_CS=1;     //uno lógico la salida de este pin
} 

float read_TC(void)               //le toma 200ms al MAX6675 hacer una conversión válida
{ 
  
    uint16_t data; 
    TC_CS=0;            //habilitar la comunicación con el sensor
    __delay_ms(1);  

    data = spiread();
    data <<= 8;
    data |= spiread();

    //__delay_ms(1);     
    TC_CS=1;  //deshabilitar la comunicación con el sensor
    
    	//los 16 bits de info lucen así:
	/*
	dummy bit(1bit)	  12 bits = temperatura	  Estado de Termocupla(1bit) 	ID	  state 
		}*/	

	data = (0x0FFF & (data>>3));      //desplaza a la derecha 3 bits, para que sólo queden los bits de temperatura

    return (((float)data-44.0)/2.021142857);
   //return(((float)data-64.0)/2.021142857); //ecuación generada después de aplicar una regresión lineal
   //return((float)data);
 
} 

uint8_t spiread(void) {
  int8_t j;
  uint8_t d = 0;

    for (j = 7; j >= 0; j--) {
        TC_CLK=0;					  
        __delay_us(10);
    
        if (TC_DATA) {
            d |= (1 << j);
        }
        TC_CLK=1;					  
        __delay_us(10);
    }

  return d;
}


