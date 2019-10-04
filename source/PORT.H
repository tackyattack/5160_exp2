/*------------------------------------------------------------------*-

   Port.H (v1.00)

  ------------------------------------------------------------------

   Port Header - Exp#1 UART & LCD Module

   Programmer(s): Henry Bergin

   Date: 10/3/19

   Description: Defines the port pins for the basic 8051 circuit
-*------------------------------------------------------------------*/

#ifndef _PORT_H
#define _PORT_H

#define PORT0 (0)
#define PORT1 (1)
#define PORT2 (2)
#define PORT3 (3)

sfr LCD_Data_Port = 0x80;
// Port 0
//sbit P0_0 = P0^0;
//sbit P0_1 = P0^1;
//sbit P0_2 = P0^2;
//sbit P0_3 = P0^3;
//sbit P0_4 = P0^4;
//sbit P0_5 = P0^5;
//sbit P0_6 = P0^6;
//sbit P0_7 = P0^7;

// Port 1
sbit P1_0 = P1^0;
sbit P1_1 = P1^1;
sbit P1_2 = P1^2;
sbit P1_3 = P1^3;
sbit P1_4 = P1^4;
sbit P1_5 = P1^5;
sbit P1_6 = P1^6;
sbit P1_7 = P1^7;


// Port 2
sbit SW1 = P2^0;
sbit SW2 = P2^1;
sbit SW3 = P2^2;
sbit SW4 = P2^3;
sbit REDLED = P2^4;
sbit YELLOWLED = P2^5;
sbit AMBERLED = P2^6;
sbit GREENLED = P2^7;

#define Red_LED 1<<4
#define Yellow_LED 1<<5
#define Amber_LED 1<<6
#define Green_LED 1<<7

// Port 3
sbit per_TxD = P3^0;
sbit per_RxD = P3^1;
sbit P3_2 = P3^2;
sbit P3_3 = P3^3;
sbit P3_4 = P3^4;
sbit P3_5 = P3^5;
sbit RS = P3^6;
sbit E = P3^7;

#define RS_bit 1<<6
#define E_bit 1<<7

#define GREEN_LED_PIN  (7)
#define AMBER_LED_PIN  (6)
#define YELLOW_LED_PIN (5)
#define RED_LED_PIN    (4)

#define SD_PORT PORT1
#define SD_NCS_PIN (4)


#endif

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
