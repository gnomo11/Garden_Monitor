#include <xc.h>
#include <stdint.h>
#include "adc.h"

#define m -0.13  			//pendiente negativa de ecuación para obtener
				  			//el % de humedad, a menor voltaje de salida, mayor humedad

float get_humidity(void);