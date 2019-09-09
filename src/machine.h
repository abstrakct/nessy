#include <cstdint>
#include <array>
#include <memory>

#include "cpu.h"
#include "ppu.h"

class Bus {
    public:
        Bus();
        ~Bus();

        // Devices connected to the bus
        std::array<uint8_t, 2*1024> cpuRam;
        CPU cpu;
        PPU ppu;

        // Bus read and write
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr, bool readonly = false);
};

// delete/change
class Machine {
    public:
        Machine();
        ~Machine();

        Bus bus;
        CPU *cpu;
        PPU *ppu;

        void init();
        //void load_rom(std::vector<uint8_t> data, uint16_t offset, uint16_t length);
};
