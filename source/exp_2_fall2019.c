#include <stdio.h>
#include "main.h"
#include "PORT.H"
#include "UART.h"
#include "print_bytes.h"
#include "Timer0_hardware_delay_1ms.h"
#include "LCD_routines.h"
#include "LED_Outputs.h"
#include "Long_Serial_In.h"
#include "SPI.h"
#include "SD_card.h"
#include "outputs.h"

main()
{
   uint8_t error_flag;
   LED_ON(RED_LED_PIN);
   LED_OFF(GREEN_LED_PIN);
   LED_OFF(AMBER_LED_PIN);
   LED_OFF(YELLOW_LED_PIN);
   //*** This line is needed to make all 1,024 byes of XRAM avaiable ***//
   AUXR=0x0c;   // make all of XRAM available, ALE always on
   if(OSC_PER_INST==6)
   {
       CKCON0=0x01;  // set X2 clock mode
   }
   else if(OSC_PER_INST==12)
   {
       CKCON0=0x00;  // set standard clock mode
   }
   Timer0_DELAY_1ms(300); // Delay to see RED LED (visual indication program is executing)
   //*** Initialization Functions  ***//
   uart_init(9600);
   LCD_Init();
   error_flag = SPI_master_init(400000);
   if(error_flag != SPI_INIT_OK) printf("SPI init error\n");

   error_flag = SD_card_init();
   if(error_flag != SD_CARD_INIT_OK)
   {
    printf("SD card init error\n");
    LCD_Print(0x00, sizeof("SD card error")-1, "SD card error");
   }
   else
   {
     printf("SD card init success\n");
     LCD_Print(0x00, sizeof("SD card success")-1, "SD card success");
     LED_ON(GREEN_LED_PIN);
   }
   LED_OFF(RED_LED_PIN);
   //**** Super Loop ****//
   while(1)
   {
        //Example of how to use lond serial input function to read a 32-bit input value
        uint32_t input_value;
        printf("Input a value: ");
        input_value = long_serial_input();
		//Notice the 'l' modifier on %u which indicates a long (32-bit value)
		//If the value to print is a char (8-bit value), then use a 'b' modifier (%bu).
        printf("Value Entered = %lu\n\r",input_value);

   }
}
