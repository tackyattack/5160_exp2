#ifndef _SPI_H
#define _SPI_H

#include "main.h"

//------- Public Constant definitions --------------------------------
#define CPOL (0)
#define CPHA (0)

#define SPI_INIT_ILLEGAL_CLOCK_RATE (0)
#define SPI_INIT_OK                 (1)
#define SPI_TRANSFER_TIMEOUT_ERROR  (2)
#define SPI_TRANSFER_SPI_ERROR      (3)
#define SPI_TRANSFER_OK             (4)


// ------ Public function prototypes -------------------------------
uint8_t SPI_transfer(uint8_t send_value, uint8_t *received_value);
uint8_t SPI_master_init(uint32_t clock_rate);

#endif
