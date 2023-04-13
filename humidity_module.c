
#include "humidity_module.h"

float get_humidity(void){
	uint16_t adc0;
	float humidity=0.0;

	
	adc0 =read_adc(0); //retornar el valor medido del Canal 0 del adc a la variable adc0
	
	humidity = m*((float)adc0) - (260.0*m) + 100.0; //obtener % de humedad
	
	return(humidity);
}

