#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include <stdint.h>
#include <stddef.h>

// SPI pin definitions
#define SPI_IO_MOSI 11
#define SPI_IO_MISO 12
#define SPI_IO_CLK 13
#define SPI_IO_CS0 14

#define SPI_VELOCITY 20000000

// Function declarations
void init_spi_device(void);
void write_spi_byte(uint8_t data);
uint8_t read_spi_byte(void);
uint8_t transfer_spi_byte(uint8_t tx_data);
void write_spi_bytes(uint8_t *data, size_t length);
void read_spi_bytes(uint8_t *buffer, size_t length);
bool spi_self_test(void);

#endif