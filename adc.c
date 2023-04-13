#include "adc.h"

void adc_init(void){
    ADCON1=0x0D;        //Configurar PIN A0 y A1 como entradas analógicas
    TRISAbits.RA0=1;    //Configurar PIN A0 como entrada
    TRISAbits.RA1=1;    //Configurar PIN A1 como entrada
    ADCON0 = 0;         //Se selecciona el canal 0 del ADC (PIN A0)
    
    
    /*      CONFIGURACIÓN INICIAL 
     *20MHz --> Tosc = 1/48MHz = 0.00000005s = 50ns
     * TAD(mín) = 0.8 us (según fabricante)
     * entonces: TAD(min)< xTosc (donde x = 2,4,8,16,32 y 64)
     * 0.8us <= 16 Tosc (= 0.8us) 
     * entonces 1 TAD = 16 TOSC = 0.8us
     * EL TIEMPO DE ADQUISICIÓN MÍNIMO PARA EL ADC ES 2.45us
     * entonces 2.45 us < xTAD (donde x = 0 a 20)
     * 2.45us < (4 *TAD) = 4*0.8us = 3.2 us--> 010
     */
    
    ADCON2bits.ACQT=0b010;  //4TAD
    ADCON2bits.ADCS=0b101;  //de acuerdo a tabla del DataSheet T21-1
    ADCON2bits.ADFM=1;      //justificado a la derecha
    
}

uint16_t read_adc(uint8_t channel){
    
        ADCON0bits.CHS = channel;   //seleccionar canal del ADC
        __delay_us(50);
        
        ADCON0bits.ADON=1;          //encender adc
        ADCON0bits.GO_DONE=1;
        while(ADCON0bits.GO_DONE);  //esperar a que la conversión termine
        ADCON0bits.ADON=0;          //apagar adc
        
        return ADRES;
}