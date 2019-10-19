#include <cstdint>
#include <array>
#include <memory>

#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"
#include "controller.h"

class Machine {
    public:
        Machine();
        ~Machine();

        // Devices connected to the bus
        std::array<uint8_t, 2048> cpuRam;
        CPU cpu;
        PPU ppu;
        std::shared_ptr<Cartridge> cart;
        Controller controller1;

        // Bus read and write
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr, bool readonly = false);

        void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
        void reset();
        void clock();

    private:
        uint32_t systemClockCounter = 0;

        uint8_t dma_page = 0x00;
        uint8_t dma_addr = 0x00;
        uint8_t dma_data = 0x00;
        bool dma_transfer = false;
        bool dma_dummy = true;
};
