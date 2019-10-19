
#include "controller.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::pressButton(Button b)
{
    state |= (1 << b);
}

void Controller::releaseButton(Button b)
{
    state &= ~(1 << b);
}

uint8_t Controller::read()
{
    // Borrowed code from LaiNES
    //
    // When strobe is high, it keeps reading A
    if (strobe)
        //return 0x40 | (state & 1);
        return (state & 1);

    // Get the status of a button and shift the register:
    uint8_t ret = (state & 1);
    state = (state >> 1);
    return ret;
}

void Controller::write(bool v)
{
    // Borrowed code from LaiNES
    // Read the joypad data on strobe's transition 1 -> 0.
    //if (strobe and !v)
    //        joypad_bits[i] = GUI::get_joypad_state(i);

    strobe = v;
}
