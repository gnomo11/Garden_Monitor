#include "ldr_module.h"


float get_light(void){
	uint16_t adc1;
	float luz;
    
	adc1=read_adc(1);//retornar el valor medido del Canal 1 del adc a la variable adc1
	
	luz = m2*((float)adc1)- (30.0*m2) + 100.0; //obtener % de luz
	
	return(luz);
} 

