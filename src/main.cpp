#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <Adafruit_MCP23X17.h>
#include "io.h"
#include "state.h"

#define I2C_SDA 25
#define I2C_SCL 26
#define MCP_1_ADDR 0x20

Adafruit_MCP23X17 mcp1;
std::vector<State *> _states;

void setup()
{
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);
  if (!mcp1.begin_I2C(MCP_1_ADDR))
  {
    Serial.println("Error connecting to MCP 1: SDA=" + String(I2C_SDA) +
                   ", SCL=" + String(I2C_SCL) +
                   ", ADDR=" + String(MCP_1_ADDR));
    while (1)
      ;
  }
  Serial.println("MCP Ready");

  _states.push_back(new State(
      "Toplicht",
      new MCP_IO(&mcp1, 7),
      new MCP_IO(&mcp1, 7 + 8),
      nullptr));

  _states.push_back(new State(
      "Navigationslicht",
      new MCP_IO(&mcp1, 6),
      new MCP_IO(&mcp1, 6 + 8),
      nullptr));

  _states.push_back(new State(
      "Kabinenlicht",
      new MCP_IO(&mcp1, 5),
      new MCP_IO(&mcp1, 5 + 8),
      nullptr));

  _states.push_back(new State(
      "Steckdosen innen",
      new MCP_IO(&mcp1, 4),
      new MCP_IO(&mcp1, 4 + 8),
      nullptr));

  _states.push_back(new State(
      "Steckdosen außen",
      new MCP_IO(&mcp1, 3),
      new MCP_IO(&mcp1, 3 + 8),
      nullptr));

  _states.push_back(new State(
      "Frischwasserpumpe",
      new MCP_IO(&mcp1, 2),
      new MCP_IO(&mcp1, 2 + 8),
      nullptr));

  _states.push_back(new State(
      "Navigationssystem",
      new MCP_IO(&mcp1, 1),
      new MCP_IO(&mcp1, 1 + 8),
      nullptr));

  _states.push_back(new State(
      "Bordcomputer",
      new MCP_IO(&mcp1, 0),
      new MCP_IO(&mcp1, 0 + 8),
      nullptr));
}

void loop()
{
  for (State *state : _states)
  {
    if (state->update())
    {
      String message = state->getName() + ":" + (state->getStatus() == Status::ACTIVE ? "ON" : "OFF");
      Serial.println("State changed: " + message);
    }
  }

  delay(50);
}