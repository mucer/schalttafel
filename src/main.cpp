#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <Adafruit_MCP23X17.h>
#include "io.h"
#include "element.h"
#include "bluetooth.cpp"

#define I2C_SDA 25
#define I2C_SCL 26
#define MCP_1_ADDR 0x20

std::vector<Element *> _elements;
Adafruit_MCP23X17 mcp1;
BluetoothManager bleManager(&_elements);

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

  _elements.push_back(new Element(
      "Toplicht",
      new MCP_IO(&mcp1, 7),
      new MCP_IO(&mcp1, 7 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Navigationslicht",
      new MCP_IO(&mcp1, 6),
      new MCP_IO(&mcp1, 6 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Kabinenlicht",
      new MCP_IO(&mcp1, 5),
      new MCP_IO(&mcp1, 5 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Steckdosen innen",
      new MCP_IO(&mcp1, 4),
      new MCP_IO(&mcp1, 4 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Steckdosen außen",
      new MCP_IO(&mcp1, 3),
      new MCP_IO(&mcp1, 3 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Frischwasserpumpe",
      new MCP_IO(&mcp1, 2),
      new MCP_IO(&mcp1, 2 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Navigationssystem",
      new MCP_IO(&mcp1, 1),
      new MCP_IO(&mcp1, 1 + 8),
      nullptr));

  _elements.push_back(new Element(
      "Bordcomputer",
      new MCP_IO(&mcp1, 0),
      new MCP_IO(&mcp1, 0 + 8),
      nullptr));

  bleManager.init();
}

void loop()
{
  for (Element *state : _elements)
  {
    if (state->update())
    {
      String message = state->getName() + ":" + (state->getStatus() == ElementStatus::ACTIVE ? "ON" : "OFF");
      Serial.println("Element changed: " + message);
      bleManager.updateElements();
    }
  }

  delay(50);
}