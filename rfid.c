#include "rfid.h"
#include <string.h>

// Private Function Prototypes
static bool pn532_spi_wakeup(PN532_SPI *pn532);
static bool pn532_spi_write_command(PN532_SPI *pn532, const uint8_t *cmd, uint8_t cmd_len);
static bool pn532_spi_read_response(PN532_SPI *pn532, uint8_t expected_length);

bool pn532_spi_init(PN532_SPI *pn532, spi_inst_t *spi,
					uint miso_pin, uint mosi_pin, uint sck_pin, uint cs_pin, uint reset_pin, uint irq_pin)
{
	pn532->spi_instance = spi;
	pn532->miso_pin = miso_pin;
	pn532->mosi_pin = mosi_pin;
	pn532->sck_pin = sck_pin;
	pn532->cs_pin = cs_pin;
	pn532->reset_pin = reset_pin;
	pn532->irq_pin = irq_pin;

	pn532->irq_mode = PN532_IRQ_MODE_NONE;

	// Initialize SPI
	spi_init(pn532->spi_instance, SPI_BAUDRATE);

	// Set SPI pins
	gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
	gpio_set_function(miso_pin, GPIO_FUNC_SPI);
	gpio_set_function(sck_pin, GPIO_FUNC_SPI);

	// Configure Chip Select pin
	gpio_init(cs_pin);
	gpio_set_dir(cs_pin, GPIO_OUT);
	gpio_put(cs_pin, 1); // Deselect chip initially

	// Configure Reset pin
	gpio_init(reset_pin);
	gpio_set_dir(reset_pin, GPIO_OUT);
	gpio_put(reset_pin, 1); // Keep reset pin high initially

	// Configure IRQ pin as input
	gpio_init(irq_pin);
	gpio_set_dir(irq_pin, GPIO_IN);
	gpio_pull_up(irq_pin);

	// Perform initial reset
	pn532_spi_reset(pn532);

	// Wakeup and initialize PN532
	if (!pn532_spi_wakeup(pn532))
		return false;
	if (!pn532_get_firmware_version(pn532))
		return false;
	if (!pn532_sam_configuration(pn532))
		return false;

	return true;
}

void pn532_spi_reset(PN532_SPI *pn532)
{
	// Pull reset pin low
	gpio_put(pn532->reset_pin, 0);
	sleep_ms(10); // Hold low for 10ms

	// Bring reset pin high
	gpio_put(pn532->reset_pin, 1);
	sleep_ms(10); // Wait for module to stabilize
}

void pn532_spi_set_irq_mode(PN532_SPI* pn532, uint8_t mode) {
    pn532->irq_mode = mode;
    
    // Configure SAM with appropriate IRQ settings
    uint8_t cmd[] = {0x00, 0x00, 0xFF, 0x03, 0xFD, 
                     PN532_COMMAND_SAMCONFIGURATION, 
                     0x01,  // Normal mode
                     0x00,  // Timeout
                     mode}; // IRQ mode
    
    pn532_spi_write_command(pn532, cmd, sizeof(cmd));
}

static bool pn532_spi_wakeup(PN532_SPI *pn532)
{
	// Wakeup sequence for PN532 over SPI
	gpio_put(pn532->cs_pin, 0); // Select chip

	// Send multiple 0x00 bytes to wake up
	uint8_t wakeup[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	spi_write_blocking(pn532->spi_instance, wakeup, sizeof(wakeup));

	gpio_put(pn532->cs_pin, 1); // Deselect chip

	// Poll the IRQ pin for readiness (if IRQ is used)
    for (int i = 0; i < 10; i++) {
        if (gpio_get(pn532->irq_pin) == 0) { // Active low indicates ready
            sleep_ms(10);
            return true;
        }
        sleep_ms(10);
    }

    return false;
}

bool pn532_get_firmware_version(PN532_SPI *pn532)
{
	uint8_t cmd[] = {0x00, 0x00, 0xFF, 0x02, 0xFE, PN532_COMMAND_GETFIRMWAREVERSION};

	if (!pn532_spi_write_command(pn532, cmd, sizeof(cmd)))
		return false;
	if (!pn532_spi_read_response(pn532, 4))
		return false;

	return true;
}

bool pn532_sam_configuration(PN532_SPI *pn532)
{
	uint8_t cmd[] = {0x00, 0x00, 0xFF, 0x03, 0xFD,
					 PN532_COMMAND_SAMCONFIGURATION,
					 0x01,	// Normal mode
					 0x00,	// Timeout
					 0x00}; // No IRQ

	if (!pn532_spi_write_command(pn532, cmd, sizeof(cmd)))
		return false;
	if (!pn532_spi_read_response(pn532, 0))
		return false;

	return true;
}

bool pn532_read_passive_target(PN532_SPI *pn532, uint8_t *uid, uint8_t *uid_length)
{
	uint8_t cmd[] = {0x00, 0x00, 0xFF, 0x04, 0xFC,
					 PN532_COMMAND_INLISTPASSIVETARGET,
					 0x01,					  // Maximum cards to detect
					 PN532_MIFARE_ISO14443A}; // Card type

	if (!pn532_spi_write_command(pn532, cmd, sizeof(cmd)))
		return false;
	if (!pn532_spi_read_response(pn532, 10))
		return false;

	// Check if a card was detected
	if (pn532->response_length < 10)
		return false;

	// Copy UID
	*uid_length = pn532->response[7];
	memcpy(uid, &pn532->response[8], *uid_length);

	return true;
}

static bool pn532_spi_write_command(PN532_SPI *pn532, const uint8_t *cmd, uint8_t cmd_len)
{
	// Select chip
	gpio_put(pn532->cs_pin, 0);

	// Write command
	int result = spi_write_blocking(pn532->spi_instance, cmd, cmd_len);

	// Deselect chip
	gpio_put(pn532->cs_pin, 1);

	return (result == cmd_len);
}

static bool pn532_spi_read_response(PN532_SPI *pn532, uint8_t expected_length)
{
	uint8_t status[2];

	// Select chip
	gpio_put(pn532->cs_pin, 0);

	// Send status request
	uint8_t status_cmd[] = {0x03}; // Read status
	spi_write_blocking(pn532->spi_instance, status_cmd, 1);

	// Read status
	spi_read_blocking(pn532->spi_instance, 0, status, 2);

	// Check if data is ready
	if (status[0] != 0x01)
	{
		gpio_put(pn532->cs_pin, 1);
		return false;
	}

	// Read response length
	spi_read_blocking(pn532->spi_instance, 0, &pn532->response_length, 1);

	// Validate expected length if specified
	if (expected_length > 0 && pn532->response_length != expected_length)
	{
		gpio_put(pn532->cs_pin, 1);
		return false;
	}

	// Read full response
	spi_read_blocking(pn532->spi_instance, 0, pn532->response, pn532->response_length);

	// Deselect chip
	gpio_put(pn532->cs_pin, 1);

	return true;
}