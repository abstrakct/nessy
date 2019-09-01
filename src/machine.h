#include <cstdint>
#include <array>
#include <memory>

#include "cpu.h"

class Bus {
    public:
        Bus();
        ~Bus();

        // Devices connected to the bus
        std::array<uint8_t, 64*1024> ram;
        CPU cpu;

        // Bus read and write
        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr, bool readonly = false);
};

// delete/change
class Machine {
    public:
        Machine();
        ~Machine();

        Bus bus;
        CPU *cpu;

        void init();
        void load_rom(std::vector<uint8_t> data, uint16_t offset, uint16_t length);
};
