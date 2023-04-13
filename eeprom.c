#include "eeprom.h"

void EEPROM_Write(int dir, char data);
unsigned char EEPROM_Read(int dir);

void EEPROM_Write(int dir, char data){
    EEADR = dir;
    EEDATA = data;

    EECON1bits.EEPGD=0;    //Acceder a la memoria EEPROM
    EECON1bits.CFGS=0;     //Acceder a los datos de memoria EEPROM
    EECON1bits.WREN=1;     //Permite escritura 
    INTCONbits.GIE=0;      //deshabilitar interrupciones
    EECON2 = 0x55;         //De acuerdo al fabricante
    EECON2 = 0x0AA;        //De acuerdo al fabricante
    EECON1bits.WR=1;       //inicializa la escritura
    INTCONbits.GIE=1;
    while(!PIR2bits.EEIF); //mientras bandera de INT sea 0
    PIR2bits.EEIF=0;       //limpiar bandera de interrupción
    EECON1bits.WREN=0;
}

unsigned char EEPROM_Read(int dir){
    
    EEADR = dir;
    EECON1bits.EEPGD=0;    //Acceder a la memoria EEPROM
    EECON1bits.CFGS=0;     //Acceder a los datos de memoria EEPROM
    EECON1bits.RD=1;       //Leer EEPROM
    
    return EEDATA;         //Retorna el valor alojado en dir
}
