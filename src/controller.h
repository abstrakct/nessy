#pragma once

#include <cstdint>

class Controller {
    private:
        uint8_t state;
        bool strobe;

    public:
        Controller();
        ~Controller();

        enum Button {
            A = 0,        // 1
            B = 1,        // 2
            Select = 2,   // 4
            Start = 3,    // 8
            Up = 4,       // 10
            Down = 5,     // 20
            Left = 6,     // 40
            Right = 7     // 80
        };

        void pressButton(Button b);
        void releaseButton(Button b);

        uint8_t read();
        void write(bool v);
};
