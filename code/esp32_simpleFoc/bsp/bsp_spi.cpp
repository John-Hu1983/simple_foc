#include <Arduino.h>
#include <SPI.h>

#include "bsp_beacon.h"
#include "bsp_spi.h"

// SPI object
SPIClass *spi = NULL;

/*
 * @brief Initialize SPI interface
 * @param None
 * @return None
 */
void init_spi_device(void)
{
  // Use the default SPI instance instead of creating a new one
  spi = &SPI;

  // Configure SPI pins
  spi->begin(SPI_IO_CLK, SPI_IO_MISO, SPI_IO_MOSI, SPI_IO_CS0);

  // Configure CS pin as output
  pinMode(SPI_IO_CS0, OUTPUT);
  digitalWrite(SPI_IO_CS0, HIGH); // Disable device by default

  Serial.println("SPI initialization completed");
}

/*
 * @brief Write a single byte via SPI
 * @param data Byte to be written
 * @return None
 */
void write_spi_byte(uint8_t data)
{
  digitalWrite(SPI_IO_CS0, LOW); // Enable slave device

  spi->beginTransaction(SPISettings(SPI_VELOCITY, MSBFIRST, SPI_MODE0));
  spi->transfer(data);
  spi->endTransaction();

  digitalWrite(SPI_IO_CS0, HIGH); // Disable slave device
}

/*
 * @brief Read a single byte via SPI
 * @param None
 * @return The read byte value
 */
uint8_t read_spi_byte(void)
{
  uint8_t data;

  digitalWrite(SPI_IO_CS0, LOW); // Enable slave device

  spi->beginTransaction(SPISettings(SPI_VELOCITY, MSBFIRST, SPI_MODE0));
  data = spi->transfer(0x00);
  spi->endTransaction();

  digitalWrite(SPI_IO_CS0, HIGH); // Disable slave device

  return data;
}

/*
 * @brief Transfer a single byte via SPI (write and read simultaneously)
 * @param tx_data Byte to be transmitted
 * @return The received byte
 */
uint8_t transfer_spi_byte(uint8_t tx_data)
{
  uint8_t rx_data;

  digitalWrite(SPI_IO_CS0, LOW); // Enable slave device

  spi->beginTransaction(SPISettings(SPI_VELOCITY, MSBFIRST, SPI_MODE0));
  rx_data = spi->transfer(tx_data);
  spi->endTransaction();

  digitalWrite(SPI_IO_CS0, HIGH); // Disable slave device

  return rx_data;
}

/*
 * @brief Write multiple bytes via SPI
 * @param data Pointer to data buffer to be written
 * @param length Number of bytes to write
 * @return None
 */
void write_spi_bytes(uint8_t *data, size_t length)
{
  digitalWrite(SPI_IO_CS0, LOW); // Enable slave device

  spi->beginTransaction(SPISettings(SPI_VELOCITY, MSBFIRST, SPI_MODE0));
  for (size_t i = 0; i < length; i++)
  {
    spi->transfer(data[i]);
  }
  spi->endTransaction();

  digitalWrite(SPI_IO_CS0, HIGH); // Disable slave device
}

/*
 * @brief Read multiple bytes via SPI
 * @param buffer Pointer to buffer to store read data
 * @param length Number of bytes to read
 * @return None
 */
void read_spi_bytes(uint8_t *buffer, size_t length)
{
  digitalWrite(SPI_IO_CS0, LOW); // Enable slave device

  spi->beginTransaction(SPISettings(SPI_VELOCITY, MSBFIRST, SPI_MODE0));
  for (size_t i = 0; i < length; i++)
  {
    buffer[i] = spi->transfer(0x00);
  }
  spi->endTransaction();

  digitalWrite(SPI_IO_CS0, HIGH); // Disable slave device
}

/*
 * @brief Perform a simple SPI self-test (loopback test, requires MISO and MOSI to be shorted)
 * @param None
 * @return true if self-test passes
 * @return false if self-test fails
 */
static u32_t beacon = 0;
bool spi_self_test(void)
{
  uint8_t test_data = 0x55;
  uint8_t received_data;

  if (!is_beacon_active(&beacon, 20))
  {
    return false; // Fixed: return proper boolean value
  }

  Serial.println("Starting SPI self-test (please ensure MISO and MOSI are shorted)...");

  received_data = transfer_spi_byte(test_data);

  Serial.print("Sent data: 0x");
  Serial.print(test_data, HEX);
  Serial.print(", Received data: 0x");
  Serial.println(received_data, HEX);

  if (received_data == test_data)
  {
    Serial.println("SPI self-test passed!");
    return true;
  }
  else
  {
    Serial.println("SPI self-test failed!");
    return false;
  }
}