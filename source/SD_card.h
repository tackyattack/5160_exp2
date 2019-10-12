#ifndef _SD_CARD_H
#define _SD_CARD_H

#include "main.h"

//------- Public Constant definitions --------------------------------
#define SD_SEND_COMMAND_INVALID              (0)
#define SD_CARD_SPI_ERROR                    (1)
#define SD_CARD_SEND_COMMAND_BYTE_ERROR      (2)
#define SD_SEND_COMMAND_OK                   (3)
#define SD_TIMEOUT_ERROR                     (4)
#define SD_COMMUNICATION_ERROR               (5)
#define SD_CARD_INIT_ERROR                   (6)
#define SD_CARD_INIT_ERROR_UNUSABLE_CARD     (7)
#define SD_CARD_INIT_POWERUP_ERROR           (8)
#define SD_CARD_INIT_OK                      (9)
#define SD_CARD_RECEIVE_COMMAND_OK           (10)
#define SD_CARD_READ_BLOCK_OK                (11)
#define SD_CARD_READ_BLOCK_ERROR             (12)
// ------ Public function prototypes -------------------------------
uint8_t SD_card_init(void);
uint8_t read_block(uint32_t block_address, uint16_t number_of_bytes, uint8_t *array);

#endif
