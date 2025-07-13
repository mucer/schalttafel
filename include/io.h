#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

class IO
{
public:
    virtual ~IO() = default;
    virtual bool read() = 0;
    virtual void write(bool state) = 0;
    virtual void setMode(int mode) = 0;
};

class GPIO_IO : public IO
{
private:
    int _pin;

public:
    GPIO_IO(int pin);
    bool read() override;
    void write(bool state) override;
    void setMode(int mode) override;
};

class MCP_IO : public IO
{
private:
    Adafruit_MCP23X17 *_mcp;
    int _pin;

public:
    MCP_IO(Adafruit_MCP23X17 *mcp, int pin);
    bool read() override;
    void write(bool state) override;
    void setMode(int mode) override;
};

#endif
