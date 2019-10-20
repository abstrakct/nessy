
#include "controller.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::pressButton(Button b)
{
    buttons |= b;
}

void Controller::releaseButton(Button b)
{
    buttons &= ~b;
}

uint8_t Controller::read()
{
    // Get the status of a button and shift the register:
    uint8_t ret = ((state & 0x80) > 0);
    state <<= 1;
    return ret;
}

void Controller::write(uint8_t data)
{
    state = buttons;
}
