#include <Arduino.h>
#include "io.h"
#include "state.h"

State::State(String name, IO *button, IO *led, IO *relais)
    : _name(name),
      _button(button),
      _led(led),
      _relais(relais),
      _lastButtonState(false),
      _status(Status::INACTIVE)
{
    if (_button)
        _button->setMode(INPUT_PULLUP);
    if (_led)
        _led->setMode(OUTPUT);
    if (_relais)
        _relais->setMode(OUTPUT);
    updateOutputs();
}

bool State::update()
{
    if (!_button)
        return false;

    bool currentState = !_button->read();
    bool stateChanged = false;

    if (currentState && !_lastButtonState)
    {
        _status = (_status == Status::ACTIVE) ? Status::INACTIVE : Status::ACTIVE;
        updateOutputs();
        stateChanged = true;
    }
    _lastButtonState = currentState;
    return stateChanged;
}

bool State::setStatus(Status newStatus)
{
    if (_status != newStatus)
    {
        _status = newStatus;
        updateOutputs();
        return true;
    }
    return false;
}

Status State::getStatus() const
{
    return _status;
}

String State::getName() const
{
    return _name;
}

void State::updateOutputs()
{
    bool outputActive = (_status == Status::ACTIVE);
    if (_led)
        _led->write(outputActive);
    if (_relais)
        _relais->write(outputActive);
}