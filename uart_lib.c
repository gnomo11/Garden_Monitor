#include "uart_lib.h"

void uart_init(unsigned long baud_rate){
    
    TRISCbits.RC6=0; //Tx
    TRISCbits.RC7=1; //Rx
    
    SPBRG = (unsigned char)(_XTAL_FREQ/(64*(baud_rate-1)));
            
    
    /*configure SPBRGH:SPBRG registers */
    BAUDCONbits.BRG16=0; //as TABLE 20-1: BAUD RATE FORMULAS says
    TXSTAbits.BRGH=0; //as TABLE 20-1: BAUD RATE FORMULAS says (x2)
    TXSTAbits.SYNC=0; //asychronous mode
    RCSTAbits.SPEN=1;  //Enable Serial Port Enable bit
    
    /*Registers for interrupts*/
    
    //INTCONbits.GIE=1; //Enable Global interrupts
    //INTCONbits.PEIE=1; //Enable Peripheral interrupts
    //PIE1bits.TXIE=1; //Enable Transmission Interrupt
    //RCONbits.IPEN=1;//disable priority levels on interrupts
    
    //PIR1bits.TXIF=1; //FLAG it cannot be cleared by sw
    
    /*TRANSMISSION*/
    TXSTAbits.TX9=0; //8-BIT transmission
    TXSTAbits.TXEN=1;
    
    /*RECEPTION*/
    PIE1bits.RCIE=1;//Enable Receive Interrupt
    IPR1bits.RCIP=1;//Prioridad alta
    RCSTAbits.RC9=0; //8-bit reception
    RCSTAbits.CREN=1; //Enable receiver
    
    
}

void uart_tx(char data){
    while(TXSTAbits.TRMT != 1);
    TXREG = data;
}

void uart_send_string(char *s){
    
    while(*s !='\0'){
        uart_tx(*s);
        s++;
    }

}
/*
char uart_rx(void){
    return RCREG;
}
*/

