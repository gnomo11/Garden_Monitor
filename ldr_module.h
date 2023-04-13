#include <xc.h>
#include <stdint.h>
#include "adc.h"

#define m2 -0.118			//segunda pendiente negativa de ecuación para obtener
				  			//el % de luz, a menor voltaje de salida, mayor % de luz 


float get_light(void);