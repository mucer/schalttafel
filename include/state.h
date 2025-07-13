#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include "io.h"

enum class Status
{
    ACTIVE,
    INACTIVE,
    ERROR
};

class State
{
private:
    String _name;
    IO *_button;
    IO *_led;
    IO *_relais;
    bool _lastButtonState;
    Status _status;
    void updateOutputs();

public:
    State(String name, IO *button, IO *led, IO *relais);
    ~State();

    bool update();
    bool setStatus(Status newStatus);
    Status getStatus() const;
    String getName() const;
};

#endif
