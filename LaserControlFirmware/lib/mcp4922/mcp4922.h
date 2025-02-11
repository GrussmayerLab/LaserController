/*******************************************************************************
 * @file mcp4922.h
 * @author Jelle Komen
 * @date January 1, 2025
 * @brief Header file for the mcp4922 class.
 *
 * @copyright Copyright (C) TU Delft
 ******************************************************************************/

#ifndef MCP4922_H
#define MCP4922_H

#include <stdio.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>

class MCP4922
{
public:
    enum Channel
    {
        channel_A,
        channel_B
    };

    MCP4922(spi_inst_t *spi, uint clock_frequency, uint sclk_pin, uint dout_pin, uint cs_pin);
    void analogWrite(Channel channel, uint16_t value);

private:
    spi_inst_t *_spi; // SPI instance specifier, either spi0 or spi1
    uint _sclk_pin;
    uint _dout_pin;
    uint _cs_pin;
    uint8_t _buffer[2];
};

#endif
