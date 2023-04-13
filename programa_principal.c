
#include <xc.h>
#include <pic18f4550.h>
#include "Fuses.h"
#include "uart_lib.h"
#include "eeprom.h"
#include "timer0.h"
#include "adc.h"
#include "ldr_module.h"
#include "humidity_module.h"
#include "termopar.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>



#define _XTAL_FREQ 20000000 //Frecuencia del XTAL oscilador
#define LED LATDbits.LATD6  //PIN donde se conecta el LED  
#define bomba LATDbits.LATD7//PIN donde se conecta la bomba
#define buffer 60           //Espacio de memoria para los mensajes enviados al modulo
#define inicio 0            //Inicio de los mensajes enviados al modulo
#define LADA "+52"          //Lada de Mexico = prefijo telefonico
#define cel 10              //Espacio de memoria para el num de cel (sin lada)
#define cel_lada 13         //Espacio de memoria para el num de cel (con lada)
#define dir_hay_cel 100     //Direccion de mem EEPROM donde se indica si hay un celular guardado  

#define m -0.13  			//pendiente negativa de ecuacion para obtener
				  			//el % de humedad, a menor voltaje de salida, mayor humedad



volatile __bit respuesta_lista=0,at_flag=0,sms_flag=0,re_sms_flag=0;
__bit fin_del_proceso=0, salto=0,num_correcto=0;
char caracter=0, cadena[buffer],cmd[buffer];
char num_cel[cel];
char num_cel_lada[cel_lada]=LADA;
char num_cel_eeprom[cel_lada];
uint8_t casilla=0,i=0,paso=0;
volatile uint16_t seg=0;
float temp =0.0;


void blink(void);
void led_ok(void);
__bit init_sim800l(void);
__bit pedir_num_cel(void);
void limpiar_cadena(void);
void limpiar_cmd(void);
void check_info(void);

void main(void) {
    
    INTCONbits.GIE=1;       //Habilitar Global interrupts
    INTCONbits.PEIE=1;      //Habilitar Peripheral interrupts
    RCONbits.IPEN=1;        //Habilitar prioridad en interrupciones
    INTCONbits.GIEH=1;      //Habilitar INT GLOB ALTAS
    INTCONbits.GIEL=1;      //Habilitar INT GLOB BAJAS

    
    TRISDbits.RD6=0;        //configurar PIN_D6 como salida digital
    TRISDbits.RD7=0;        //configurar PIN_D7 como salida digital
    LED=0;bomba=0;          //LED indicador y bomba apagados
    
    limpiar_cadena();       //limpiar cadena
    for(i=0;i<cel;i++){num_cel[i]=0;}   //limpiar num_cel
    
    uart_init(9600);        //Inicializar la comunicacion serial a 9600 bits/s
       
    limpiar_cadena();       //se limpia la cadena por seguridad
    
    
    if(EEPROM_Read(dir_hay_cel)){
        led_ok();
        for(i=0;i<cel_lada;i++){
            num_cel_eeprom[i]=EEPROM_Read(i);
        }
        
        uart_send_string("cel_EE=\r\n");
        uart_send_string(num_cel_eeprom);
        uart_send_string("\r\n");
        __delay_ms(500);
    
    }
    else{
        blink();
        while(!pedir_num_cel()); //Esperar mientras a que se reciba un num de cel valido
    }
    
    adc_init();             //Inicializar el ADC
    init_TC();
    
    uint16_t adc0;
   
    while(1){
        
 
        if(respuesta_lista){
            check_info();       //analizar mensaje enviado al modulo GSM
            limpiar_cadena();   //se limpia la cadena por seguridad
            respuesta_lista=0;  //limpiar bandera
        }
        
  
        sprintf(cmd,"Temp= %.0f%cC\r\n Hum= %.0f%c\r\nLuz= %.0f%c\r\n",read_TC(),176,get_humidity(),37,get_light(),37);
        uart_send_string(cmd);
        __delay_ms(4000);
    }
    
    return;
}

void __interrupt(high_priority)RX_INT(void){
    
    if(PIR1bits.RCIF){
        
        caracter=RCREG;
        
        if(casilla == buffer){
            casilla=0;
            for(i=0;i<buffer;i++){cadena[i]=0;}
        }
        
        cadena[casilla]=caracter;
       
        casilla++;
        if(caracter == '\r'){
            
            casilla=0;
            respuesta_lista = 1;
            salto=0;
        }
    }
}

void __interrupt(low_priority) ISR_TIMER_0(void){
    
    if(INTCONbits.TMR0IF){
        seg++;
        TMR0L = tmr0_load;
        TMR0H = (tmr0_load>> 8);
        INTCONbits.TMR0IF=0;
    }
}

__bit init_sim800l(void){
  
    while(fin_del_proceso != 1){
        
        switch(paso){

            case 0:
                while(at_flag != 1){
                    uart_send_string("AT\r\n"); //comprobar que el modulo tenga signal GSM
                    __delay_ms(1000);

                    if(respuesta_lista){
                        if(cadena[0] == 'O' && cadena[1] == 'K'){
                            limpiar_cadena();
                            blink();
                            at_flag=1;
                        }
                        respuesta_lista=0;
                    }
                }
                if(at_flag){paso++;}
                break;

            case 1:
                while(sms_flag != 1){
                    uart_send_string("AT+CMGF=1\r\n"); //modo: SMS
                    __delay_ms(1000);

                    if(respuesta_lista){
                        if(cadena[0] == 'O' && cadena[1] == 'K'){
                            limpiar_cadena();
                            blink();
                            sms_flag=1;
                        }    
                        respuesta_lista=0;
                    }
                }
                if(sms_flag){paso++;}
                break;
                
            case 2:
                while(re_sms_flag != 1){
                    uart_send_string("AT+CNMI=2,2\r\n"); //modo: recibir SMS
                    __delay_ms(1000);

                    if(respuesta_lista){
                        if(cadena[0] == 'O' && cadena[1] == 'K'){
                            limpiar_cadena();
                            blink();
                            re_sms_flag=1;
                        }    
                        respuesta_lista=0;
                    }
                }
                if(re_sms_flag){fin_del_proceso=1;}
                break;
        }
    }
    
    if(fin_del_proceso){return 1;}
    else{return 0;}

}
void blink(void){
    LED=1;
    __delay_ms(200);
    LED=0;  
    __delay_ms(200);
    LED=1;
    __delay_ms(200);
    LED=0;
    __delay_ms(500);
}

void led_ok(void){
    LED=1;
    __delay_ms(2000);
    LED = 0;
}

void limpiar_cadena(void){
    casilla=0;
    for(i=0;i<buffer;i++){cadena[i]=0;} 
}

void limpiar_cmd(void){
    for(i=0;i<buffer;i++){cmd[i]=0;} 
}

void check_info(void){
    
    /* Si se recibe un numero de celular... C753xxxxxxx = 10 digitos*/
    
    if(cadena[inicio]=='C' || cadena[inicio]=='c'){
        if(strlen(cadena) == 12){
            for(i=0;i<cel;i++){
                num_cel[i]=cadena[(inicio+1)+i]; //guardar num celular
            }
        
            strncat(num_cel_lada, num_cel, cel); //concatenar "+52" y el num cel. 
                                                 //Guardar el numero completo en num_cel_lada
            
            for(i=0;i<cel;i++){num_cel[i]=0;}   //limpiar num_cel
            
        
            uart_send_string("cel=\r\n");
            uart_send_string(num_cel_lada);
            uart_send_string("\r\n");
        
        
            for(i=0;i<cel_lada;i++){
                EEPROM_Write(i,num_cel_lada[i]);
            }
            EEPROM_Write(dir_hay_cel,1);
            for(i=3;i<cel_lada;i++){num_cel_lada[i]=0;}   //limpiar num_cel_lada
            for(i=0;i<cel_lada;i++){
                num_cel_eeprom[i]=EEPROM_Read(i);
            }
        
            uart_send_string("cel_EE=\r\n");
            uart_send_string(num_cel_eeprom);
            uart_send_string("\r\n");
            num_correcto=1;
            led_ok();
            for(i=3;i<cel_lada;i++){num_cel_lada[i]=0;}   //limpiar num_cel_lada
            for(i=0;i<cel_lada;i++){num_cel_eeprom[i]=0;}   //limpiar num_cel_lada
            
        }
        else{blink();num_correcto=0;}
        
    }
    
    /*Si se recibe la instruccion para On/Off la bomba*/
    else{
        if(cadena[inicio]=='O' && cadena[inicio+1] == 'n'){
            LED=1;
        }
        else if(cadena[inicio]=='O' && cadena[inicio+1] == 'f' && cadena[inicio+2] == 'f'){
            LED=0;
        }
    }
    
}

__bit pedir_num_cel(void){
    
    if(respuesta_lista){
            check_info();       //analizar mensaje enviado al modulo GSM
            limpiar_cadena();   //se limpia la cadena por seguridad
            respuesta_lista=0;  //limpiar bandera
        }
    
    if(num_correcto){return 1;}
    else{return 0;}
    
}