/* 
 * File:   mainl2.c
 * Author: saras
 *
 * Created on July 28, 2022, 7:48 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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
#include <stdio.h>


#define _XTAL_FREQ  4000000
#define RS RE1
#define EN RE2
#define D0 RD0
#define D1 RD1
#define D2 RD2
#define D3 RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include "lcd.h"
#include "adc.h"
#include "oscilador.h"
#include "tmr0.h"

unsigned short voltaje_1;
unsigned short voltaje_2;
uint8_t entero_1;
uint8_t entero_2;
uint8_t dec_1;
uint8_t dec_2;
uint8_t pot1;
uint8_t pot2;
char s[];
uint8_t cont = 0;
uint8_t flag = 0;

void __interrupt() isr (void){
    if(PIR1bits.ADIF){      // Interrupción ADC
        if(ADCON0bits.CHS == 0){
           pot1 = adc_read();
       }
        if(ADCON0bits.CHS == 1){
           pot2 = adc_read();
       }
    }
    if(INTCONbits.T0IF){ //Interrupción TMR0
        cont++;
        if(cont == 100){ // 5s
            if(flag == 1){ //primer estado
                flag = 0;
            }
            else if(flag == 0){ //segundo estado
                flag = 1;
            }
            cont = 0;
        }
        tmr0_reload();
    }
    return;
}

void main(void){
    unsigned int a;
    TRISD = 0;
    TRISC = 0;
    PORTD = 0;
    PORTC = 0;
    
    TRISE  = 0;
    PORTD = 0;
    
    TRISB = 0;
    PORTB = 0;
    
    TRISA = 0b11;
    PORTA = 0;
    
    ANSEL = 0b11;
    ANSELH = 0;
    
    unsigned short map(uint8_t val, uint8_t in_min, uint8_t in_max, 
            unsigned short out_min, unsigned short out_max);
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1;
    INTCONbits.T0IE = 1; //Habiliatamos int. TMR0
    
    
    
    int_osc_MHz(4); // Oscilador 1, 2, 4 u 8MHz
    
    adc_init(1,     // 0 = FOSC/2       1 = FOSC/8     2 = FOSC/32      3 = FRC
            0,      // 0 = Voltaje positivo externo     1 = Voltaje positivo interno
            0);      // 0 = Voltaje negativo externo     1 = Voltaje negativo interno
    
    adc_start(0);
    adc_start(1);
    
    tmr0_reload();   //función de TMR0
    tmr0_init(256);  //configuración prescaler 256 TMR0 
    
    
    // 4 bits
    
    
//    Lcd_Init();
//    while (1)
//    {
//        Lcd_Clear();
//        Lcd_Set_Cursor(1,1);
//        Lcd_Write_String("Sara Schumann");
//        Lcd_Set_Cursor(2,1);
//        Lcd_Write_String("20163 - 4b");
//        __delay_ms(2000);
//        Lcd_Clear();
//    }
    
    
    // 8 bits
    
    Lcd_Clear_8();  
    
    /*
        Lcd_Set_Cursor_8(1,1);           
        Lcd_Write_String_8("Sara Schumann"); 
        Lcd_Set_Cursor_8(2,1);       
        Lcd_Write_String_8("20163 - 8b");            
        __delay_ms(2000);
        Lcd_Clear_8(); */
    
    
    Lcd_Init_8();                            
    while (1)
    {
          
        if(ADCON0bits.GO == 0){             // No hay proceso de conversion
                if(ADCON0bits.CHS == 0){
                    ADCON0bits.CHS = 1;      // Canal que se desea leer e inicia conversión de ADC
                    voltaje_1 = map(pot1, 0,255,0,500);
                    entero_1 = voltaje_1/100;
                    dec_1 = voltaje_1-entero_1*100;
                    
                }
                else if(ADCON0bits.CHS == 1){
                    ADCON0bits.CHS = 0;       // Canal que se desea leer e inicia conversión de ADC
                    voltaje_2 = map(pot2, 0,255,0,500);
                    entero_2 = voltaje_2/100;
                    dec_2 = voltaje_2-entero_2*100;
                }
                __delay_us(40);
                ADCON0bits.GO = 1;              // Iniciamos proceso de conversión
            }
        if(flag == 1){
            Lcd_Set_Cursor_8(1,1);
            Lcd_Write_String_8("      POT1        ");
            Lcd_Set_Cursor_8(2,1);
            sprintf(s, "      %d.%dV     ", entero_1,dec_1); //guardamos el string que vamos a mostrar
            Lcd_Set_Cursor_8(2,1);
            Lcd_Write_String_8(s);
        }
        else if(flag == 0){
            Lcd_Set_Cursor_8(1,1);
            Lcd_Write_String_8("      POT2      ");
            Lcd_Set_Cursor_8(2,1);
            sprintf(s, "      %d.%dV      ", entero_2, dec_2);
            Lcd_Set_Cursor_8(2,1);
            Lcd_Write_String_8(s);
        }
    }
   return; 
}

unsigned short map(uint8_t x, uint8_t x0, uint8_t x1, 
            unsigned short y0, unsigned short y1){
    return (unsigned short)(y0+((float)(y1-y0)/(x1-x0))*(x-x0));
}