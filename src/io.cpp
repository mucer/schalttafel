#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include "io.h"

GPIO_IO::GPIO_IO(int pin) : _pin(pin) {}

bool GPIO_IO::read()
{
    return digitalRead(_pin);
}

void GPIO_IO::write(bool state)
{
    digitalWrite(_pin, state);
}

void GPIO_IO::setMode(int mode)
{
    pinMode(_pin, mode);
}

MCP_IO::MCP_IO(Adafruit_MCP23X17 *mcp, int pin)
    : _mcp(mcp),
      _pin(pin) {}

bool MCP_IO::read()
{
    return _mcp->digitalRead(_pin);
}

void MCP_IO::write(bool state)
{
    _mcp->digitalWrite(_pin, state);
}

void MCP_IO::setMode(int mode)
{
    _mcp->pinMode(_pin, mode);
}