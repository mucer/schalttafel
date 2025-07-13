#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include "io.h"

enum class ElementStatus
{
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2
};

class Element
{
private:
    String _name;
    IO *_button;
    IO *_led;
    IO *_relais;
    bool _lastButtonState = false;
    ElementStatus _status = ElementStatus::INACTIVE;
    void updateOutputs();

public:
    Element(String name, IO *button, IO *led, IO *relais);
    ~Element();

    bool update();
    bool setStatus(ElementStatus newStatus);
    ElementStatus getStatus() const;
    String getName() const;
};

#endif
