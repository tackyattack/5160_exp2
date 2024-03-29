#include <stdio.h>
#include "main.h"
#include "SD_card.h"
#include "SPI.h"
#include "PORT.H"
#include "outputs.h"

#define SD_SPI_COMMAND_NO_ERRORS (0)
#define CMD0    (0)
#define CMD8    (8)
#define CMD58   (58)
#define ACMD41  (41)
#define CMD55   (55)
#define CMD16   (16)
#define CMD17   (17)
#define R1_ACTIVE (0x00)
#define R1_IDLE   (0x01)

#define CARD_VERSION_1 (1)
#define CARD_VERSION_2 (2)

#define BASIC_COMMAND_OK (0)
#define BASIC_COMMAND_FAIL (1)
#define BASIC_COMMAND_NO_MORE_CMDS (0)
#define BASIC_COMMAND_MORE_CMDS (1)

uint8_t send_command(uint8_t command, uint32_t argument)
{
  uint8_t error_flag, received_value, byte_cnt;
  uint8_t send_bytes[6];
  if(command > 63) return SD_SEND_COMMAND_INVALID;

  // append start and transmission bits to first byte
  send_bytes[0] = 0x40 | command;
  send_bytes[1] = (argument>>24);
  send_bytes[2] = (argument&0x00ffffff)>>16;
  send_bytes[3] = (argument&0x0000ffff)>>8;
  send_bytes[4] = (argument&0x000000ff);
  // CRC7 and end bit
  if(command == 0)
  {
    send_bytes[5] = 0x95;
  }
  else if(command == 8)
  {
    send_bytes[5] = 0x87;
  }
  else
  {
    send_bytes[5] = 0x01; // just end bit for other commands
  }

  for(byte_cnt = 0; byte_cnt < 6; byte_cnt++)
  {
    error_flag = SPI_transfer(send_bytes[byte_cnt], &received_value);
    if(error_flag != SPI_TRANSFER_OK)
    {
      return SD_CARD_SPI_ERROR;
    }
  }

  return SD_SEND_COMMAND_OK;
}

uint8_t receive_response(uint8_t number_of_bytes, uint8_t *array_name)
{
  uint8_t ret_val, SPI_value, error_flag, index;
  uint16_t timeout;
  ret_val = SD_CARD_RECEIVE_COMMAND_OK;
  // Get R1 response
  timeout=0;
  do
  {
    error_flag = SPI_transfer(0xFF, &SPI_value);
    timeout++;
  }while((SPI_value == 0xFF)&&(timeout!=0)&&(error_flag==SPI_TRANSFER_OK));

  if(error_flag != SPI_TRANSFER_OK)
  {
    ret_val = SD_CARD_SPI_ERROR;
  }
  else if(timeout == 0)
  {
    ret_val = SD_TIMEOUT_ERROR;
  }
  else if((SPI_value&0xFE)!=0x00)
  {
    array_name[0] = SPI_value;
    ret_val = SD_COMMUNICATION_ERROR;
  }
  else
  {
    // get the rest of the bytes
    array_name[0] = SPI_value;
    if(number_of_bytes > 1)
    {
      for(index=1; index<number_of_bytes; index++)
      {
        error_flag = SPI_transfer(0xFF, &SPI_value);
        array_name[index] = SPI_value;
      }
    }
  }

  error_flag = SPI_transfer(0xFF, &SPI_value); // send one more byte to give SD card 8 clocks
  return ret_val;
}



uint8_t send_basic_init_cmd(uint8_t cmd, uint32_t arg, uint8_t response_size, uint8_t *values, uint8_t more_cmds)
{
  uint8_t error_flag, i;
  printf("Sending CMD%bu  arg:< ", cmd);
  for(i=0;i<4;i++)printf("%2.2bX ",(arg&(0xff<<i*8))>>i*8);
  printf(">\n");

  write_port_bit(SD_PORT, SD_NCS_PIN, CLEAR_BIT);

  error_flag = send_command(cmd, arg);
  if(error_flag != SD_SEND_COMMAND_OK)
  {
    write_port_bit(SD_PORT, SD_NCS_PIN, SET_BIT);
    printf("Error: command: code: %2.2bX\n", error_flag);
    return BASIC_COMMAND_FAIL;
  }
  error_flag = receive_response(response_size, values);
  // don't set /CS if there's going to be more commands
  if(more_cmds == BASIC_COMMAND_NO_MORE_CMDS) write_port_bit(SD_PORT, SD_NCS_PIN, SET_BIT);
  if(error_flag != SD_CARD_RECEIVE_COMMAND_OK)
  {
    printf("Error: response: code: %2.2bX\n", error_flag);
    return BASIC_COMMAND_FAIL;
  }

  printf("   response:< ");
  for(i=0;i<response_size;i++)printf("%2.2bX ", values[i]);
  printf(">\n");

  return BASIC_COMMAND_OK;
}

uint8_t SD_card_init(void)
{
  uint8_t i, error_flag, SPI_values[8], card_version;
  uint16_t timeout;
  printf("SD Card init...\n\r");

  write_port_bit(SD_PORT, SD_NCS_PIN, SET_BIT);
  // send 80 clocks
  for(i = 0; i < 10; i++)
  {
    error_flag = SPI_transfer(0xFF, &SPI_values[0]);
  }

  // ---------------- Send CMD0 ----------------
  error_flag = send_basic_init_cmd(CMD0, 0x00, 1, SPI_values, BASIC_COMMAND_NO_MORE_CMDS);
  if (error_flag == BASIC_COMMAND_FAIL || SPI_values[0] != R1_IDLE) return SD_CARD_INIT_ERROR;

  // ---------------- Send CMD8 ----------------
  error_flag = send_basic_init_cmd(CMD8, 0x000001AA, 5, SPI_values, BASIC_COMMAND_NO_MORE_CMDS);
  if (error_flag == BASIC_COMMAND_FAIL) return SD_CARD_INIT_ERROR;

  if(SPI_values[0] == 0x01)
  {
    card_version = CARD_VERSION_2;
    // voltage check
    // 0x01: 2.7V-3.6V
    if(SPI_values[3] != 0x01)
    {
      printf("Error: SD card voltage is not in range\n");
      return SD_CARD_INIT_ERROR_UNUSABLE_CARD;
    }
  }
  else if(SPI_values[0] == 0x05)
  {
    card_version = CARD_VERSION_1;
  }
  else
  {
    return SD_CARD_INIT_ERROR;
  }
  printf("SD card version:%bu \n", card_version);

  // ---------------- Send CMD58 ----------------
  error_flag = send_basic_init_cmd(CMD58, 0x000001AA, 5, SPI_values, BASIC_COMMAND_NO_MORE_CMDS);
  if (error_flag == BASIC_COMMAND_FAIL || SPI_values[0] != R1_IDLE) return SD_CARD_INIT_ERROR;

  // voltage check
  if((SPI_values[2]&0xFC) != 0xFC)
  {
    printf("Error: SD card voltage is not in range\n");
    return SD_CARD_INIT_ERROR_UNUSABLE_CARD;
  }

  // ---------------- Send ACMD41 ----------------
  timeout=0;
  do
  {
    error_flag = send_basic_init_cmd(CMD55, 0x00, 1, SPI_values, BASIC_COMMAND_MORE_CMDS);
    if (error_flag == BASIC_COMMAND_FAIL || SPI_values[0] != R1_IDLE) return SD_CARD_INIT_ERROR;


    if(card_version == CARD_VERSION_1)
    {
      error_flag = send_basic_init_cmd(ACMD41, 0x00, 1, SPI_values, BASIC_COMMAND_MORE_CMDS);
    }
    else
    {
      error_flag = send_basic_init_cmd(ACMD41, 0x40000000, 1, SPI_values, BASIC_COMMAND_MORE_CMDS);
    }
    if(error_flag == BASIC_COMMAND_FAIL) return SD_CARD_INIT_ERROR;

    timeout++;
  }while((timeout!=0) && (SPI_values[0] != R1_ACTIVE));
  write_port_bit(SD_PORT, SD_NCS_PIN, SET_BIT);
  if(timeout == 0)
  {
    printf("Error: SD card timeout\n");
    return SD_CARD_INIT_ERROR;
  }

  // ---------------- Send CMD58 ----------------
  if(card_version == CARD_VERSION_2)
  {
    error_flag = send_basic_init_cmd(CMD58, 0x00, 5, SPI_values, BASIC_COMMAND_NO_MORE_CMDS);
    if (error_flag == BASIC_COMMAND_FAIL || SPI_values[0] != R1_ACTIVE) return SD_CARD_INIT_ERROR;

    // check for power up
    if((SPI_values[1]&0x80) != 0x80)
    {
      printf("Error: SD card did not power up\n");
      return SD_CARD_INIT_POWERUP_ERROR;
    }
    // check for standard capacity card
    if((SPI_values[1]&0x40) != 0x40)
    {
      printf("SD card capacity: standard\n");
      error_flag = send_basic_init_cmd(CMD16, 512, 1, SPI_values, BASIC_COMMAND_NO_MORE_CMDS);
      if (error_flag == BASIC_COMMAND_FAIL || SPI_values[0] != R1_ACTIVE) return SD_CARD_INIT_ERROR;
    }
    else
    {
      printf("SD card capacity: high\n");
    }
  }

  return SD_CARD_INIT_OK;

}

uint8_t read_block(uint32_t block_address, uint16_t number_of_bytes, uint8_t *array)
{
  uint8_t ret_val, SPI_value, error_flag;
  uint16_t timeout, index;
  ret_val = SD_CARD_RECEIVE_COMMAND_OK;

  send_basic_init_cmd(CMD17, block_address, 1, &SPI_value, BASIC_COMMAND_MORE_CMDS);
  if (error_flag == BASIC_COMMAND_FAIL || SPI_value != R1_ACTIVE) return SD_CARD_READ_BLOCK_ERROR;

  printf("Waiting for data start token\n");
  // Get to data start token
  timeout = 0;
  do
  {
    error_flag = SPI_transfer(0xFF, &SPI_value);
    timeout++;
  }while((SPI_value == 0xFF)&&(timeout!=0)&&(error_flag==SPI_TRANSFER_OK));
  if(timeout == 0) return SD_TIMEOUT_ERROR;
  if(error_flag != SPI_TRANSFER_OK) return SD_CARD_SPI_ERROR;

  // data token
  if(SPI_value == 0xFE)
  {
    printf("Data start token found\n");
    for(index = 0; index < number_of_bytes; index++)
    {
      error_flag = SPI_transfer(0xFF, &SPI_value);
      if(error_flag != SPI_TRANSFER_OK) return SD_CARD_READ_BLOCK_ERROR;
      array[index] = SPI_value;
    }
  }
  else
  {
    return SD_CARD_READ_BLOCK_ERROR;
  }

  // CRC16 and give SD card 8 more clocks
  SPI_transfer(0xFF, &SPI_value);
  SPI_transfer(0xFF, &SPI_value);
  SPI_transfer(0xFF, &SPI_value);

  write_port_bit(SD_PORT, SD_NCS_PIN, SET_BIT);

  return SD_CARD_READ_BLOCK_OK;
}
