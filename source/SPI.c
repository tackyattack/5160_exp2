#include "main.h"
#include "SPI.h"

#define MAX_CLK_SELECT (7)
#define CPHA_BIT  (2)
#define CPOL_BIT  (3)
#define MSTR_BIT  (4)
#define SSDIS_BIT (5)
#define SPEN_BIT  (6)
#define SPR0_BIT  (0)
#define SPR1_BIT  (1)
#define SPR2_BIT  (7)

uint8_t SPI_master_init(uint32_t clock_rate)
{
  uint8_t divider,power2_val,power2_cnt;
  divider = (uint8_t)(((OSC_FREQ/OSC_PER_INST)*6)/clock_rate);
  power2_val = 2; // start off checking 2

  // run through each possible divider and check if it's still
  // less than the requested divider
  for(power2_cnt = 0; (power2_cnt < MAX_CLK_SELECT) && (divider > power2_val); power2_cnt++)
  {
    power2_val = power2_val * 2;
  }

  if(divider <= power2_val)
  {
    SPCON = ( power2_cnt&(1<<0) | power2_cnt&(1<<1) | ((power2_cnt&(1<<2))>>2)<<SPR2_BIT )
            |(CPOL<<CPOL_BIT)|(CPHA<<CPHA_BIT)|(1<<SPEN_BIT)|(1<<MSTR_BIT)|(1<<SSDIS_BIT);
  }
  else
  {
    return SPI_INIT_ILLEGAL_CLOCK_RATE;
  }

  return SPI_INIT_OK;
}

uint8_t SPI_transfer(uint8_t send_value, uint8_t *received_value)
{
  uint8_t timeout, status, error_flag;

  //send and wait
  SPDAT = send_value;
  timeout = 0;
  do
  {
    status = SPSTA;
    timeout++;
  } while(((status&0xF0)==0)&&(timeout!=0));

  // check for errors
  if(timeout==0)
  {
    error_flag = SPI_TRANSFER_TIMEOUT_ERROR;
    *received_value = 0xFF;
  }
  else if((status&0x70)!=0)
  {
    error_flag = SPI_TRANSFER_SPI_ERROR;
    *received_value = 0xFF;
  }
  else
  {
    error_flag = SPI_TRANSFER_OK;
    *received_value = SPDAT;
  }

  return error_flag;

}
