#include <Arduino.h>
#include "io.h"
#include "element.h"

Element::Element(String name, IO *button, IO *led, IO *relais)
    : _name(name),
      _button(button),
      _led(led),
      _relais(relais)
{
    if (_button)
        _button->setMode(INPUT_PULLUP);
    if (_led)
        _led->setMode(OUTPUT);
    if (_relais)
        _relais->setMode(OUTPUT);
    updateOutputs();
}

bool Element::update()
{
    if (!_button)
        return false;

    bool currentElement = !_button->read();
    bool stateChanged = false;

    if (currentElement && !_lastButtonState)
    {
        _status = (_status == ElementStatus::ACTIVE) ? ElementStatus::INACTIVE : ElementStatus::ACTIVE;
        updateOutputs();
        stateChanged = true;
    }
    _lastButtonState = currentElement;
    return stateChanged;
}

bool Element::setStatus(ElementStatus newStatus)
{
    if (_status != newStatus)
    {
        _status = newStatus;
        updateOutputs();
        return true;
    }
    return false;
}

ElementStatus Element::getStatus() const
{
    return _status;
}

String Element::getName() const
{
    return _name;
}

void Element::updateOutputs()
{
    bool outputActive = (_status == ElementStatus::ACTIVE);
    if (_led)
        _led->write(outputActive);
    if (_relais)
        _relais->write(!outputActive);
}