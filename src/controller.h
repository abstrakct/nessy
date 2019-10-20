#pragma once

#include <cstdint>

class Controller {
    private:
        uint8_t buttons;
        uint8_t state;

    public:
        Controller();
        ~Controller();

        enum Button {
            Right  = 0x01,
            Left   = 0x02,
            Down   = 0x04,
            Up     = 0x08,
            Start  = 0x10,
            Select = 0x20,
            B      = 0x40,
            A      = 0x80,
        };

        void pressButton(Button b);
        void releaseButton(Button b);

        uint8_t read();
        void write(uint8_t data);
};
