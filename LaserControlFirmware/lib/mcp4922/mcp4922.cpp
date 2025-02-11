#include "mcp4922.h"

MCP4922::MCP4922(spi_inst_t *spi, uint clock_frequency, uint sclk_pin, uint dout_pin, uint cs_pin)
{
    _spi = spi;
    _sclk_pin = sclk_pin;
    _dout_pin = dout_pin;
    _cs_pin = cs_pin;

    // Set the corresponding SPI pins
    gpio_set_function(_sclk_pin, GPIO_FUNC_SPI);
    gpio_set_function(_dout_pin, GPIO_FUNC_SPI);
    gpio_init(_cs_pin);
    gpio_set_dir(_cs_pin, GPIO_OUT);
    gpio_pull_up(_cs_pin);

    spi_init(_spi, clock_frequency);
    // Install the corresponding spi settings
    spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_put(_cs_pin, 1);
}

void MCP4922::analogWrite(Channel channel, uint16_t value)
{
    invalid_params_if(SPI, value > 0xFFF);

    uint8_t channel_bit = (channel == Channel::channel_A) ? 0x00 : 0x80;

    channel_bit |= 0x10;
    // channel_bit |= 0x40;

    _buffer[0] = channel_bit | (value >> 8);
    _buffer[1] = 0xFF & value;
    printf(".......\n");
    printf("%d\n", value);
    printf("%d\n", _buffer[0]);
    printf("%d\n", _buffer[1]);
    printf(".......\n");

    // Write buffer
    gpio_put(_cs_pin, 0);
    spi_write_blocking(_spi, _buffer, sizeof(_buffer));
    gpio_put(_cs_pin, 1);
}