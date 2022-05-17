/* 
 * File:   main_EEPROM.c
 * Author: Christopher Chiroy
 * 
 * 2 botones para modificar la direccion a leer/escribir en la EEPROM
 * 2 botones para modificar dato a escribir en la EEPROM
 * 1 boton para leer la EEPROM
 * 1 boton para escribir en la EEPROM
 * 
 * Direccion de la EEPROM -> PORTA
 * Dato a escribir en la EERPOM -> PORTC
 * Dato obtenido de la EEPROM -> PORTD
 *
 * Created on 10 mei 2022, 19:00
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 1000000

/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
uint8_t address = 0, cont = 0;

/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void setup(void);
uint8_t read_EEPROM(uint8_t address);
void write_EEPROM(uint8_t address, uint8_t data);


/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    if (INTCONbits.RBIF){
        if (PORTBbits.RB0 == 0)
            address++;
        else if (PORTBbits.RB1 == 0)
            address--;
        else if (PORTBbits.RB2 == 0)
            cont++;
        else if (PORTBbits.RB3 == 0)
            cont--;
        else if (PORTBbits.RB4 == 0)
            write_EEPROM(address, cont);
        else if (PORTBbits.RB5 == 0)
            PORTD = read_EEPROM(address);
        
        PORTA = address;
        PORTC = cont;
        INTCONbits.RBIF = 0;
    }
    return;
}

/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    while(1){        

    }
    return;
}

/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0b00111111;
    PORTB = 0;
    
    TRISA = 0;
    TRISC = 0;
    TRISD = 0;
    
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
    
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b00111111;
    IOCB = 0b00111111;
    
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    INTCONbits.GIE = 1;
}

uint8_t read_EEPROM(uint8_t address){
    EEADR = address;
    EECON1bits.EEPGD = 0;       // Lectura a la EEPROM
    EECON1bits.RD = 1;          // Obtenemos dato de la EEPROM
    return EEDAT;               // Regresamos dato 
}

void write_EEPROM(uint8_t address, uint8_t data){
    EEADR = address;
    EEDAT = data;
    EECON1bits.EEPGD = 0;       // Escritura a la EEPROM
    EECON1bits.WREN = 1;        // Habilitamos escritura en la EEPROM
    
    INTCONbits.GIE = 0;         // Deshabilitamos interrupciones
    EECON2 = 0x55;      
    EECON2 = 0xAA;
    
    EECON1bits.WR = 1;          // Iniciamos escritura
    
    EECON1bits.WREN = 0;        // Deshabilitamos escritura en la EEPROM
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
}