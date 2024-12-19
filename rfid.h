#ifndef PN532_SPI_H
#define PN532_SPI_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdbool.h>
#include <stdint.h>

// PN532 SPI Commands
#define PN532_COMMAND_GETFIRMWAREVERSION 0x02
#define PN532_COMMAND_SAMCONFIGURATION 0x14
#define PN532_COMMAND_INLISTPASSIVETARGET 0x4A

// Card Types
#define PN532_MIFARE_ISO14443A 0x00

// SPI Configuration
#define SPI_BAUDRATE 1000000 // 1 MHz

// IRQ Modes
#define PN532_IRQ_MODE_NONE 0
#define PN532_IRQ_MODE_CARD_DETECTION 1
#define PN532_IRQ_MODE_COMMUNICATION 2

typedef struct
{
	spi_inst_t *spi_instance;
	uint miso_pin;
	uint mosi_pin;
	uint sck_pin;
	uint cs_pin;
	uint reset_pin;
	uint irq_pin;
	uint8_t irq_mode;
	uint8_t response[64];
	uint8_t response_length;
} PN532_SPI;

// Public Function Prototypes
bool pn532_spi_init(PN532_SPI *pn532, spi_inst_t *spi,
					uint miso_pin, uint mosi_pin, uint sck_pin, uint cs_pin, uint reset_pin, uint irq_pin);
void pn532_spi_reset(PN532_SPI *pn532); // Added reset function
bool pn532_get_firmware_version(PN532_SPI *pn532);
bool pn532_sam_configuration(PN532_SPI *pn532);
bool pn532_read_passive_target(PN532_SPI *pn532, uint8_t *uid, uint8_t *uid_length);

// IRQ-related functions
void pn532_spi_set_irq_mode(PN532_SPI* pn532, uint8_t mode);

#endif // PN532_SPI_H