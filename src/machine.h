#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "cartridge.h"
#include "controller.h"
#include "cpu.h"
#include "disassembler.h"
#include "ppu.h"

class Machine
{
public:
    Machine();
    ~Machine();

    // Devices connected to the bus
    std::array<uint8_t, 2048> cpuRam;
    CPU cpu;
    PPU ppu;
    std::shared_ptr<Cartridge> cart;
    Controller controller[2];

    // Bus read and write
    void cpuWrite(uint16_t addr, uint8_t data);
    uint8_t cpuRead(uint16_t addr, bool readonly = false);

    void insertCartridge(const std::shared_ptr<Cartridge> &cartridge);
    void reset();
    void clock();

private:
    uint32_t systemClockCounter = 0;

    uint8_t dma_page = 0x00;
    uint8_t dma_addr = 0x00;
    uint8_t dma_data = 0x00;
    uint8_t dma_count = 0x00;
    bool dma_transfer = false;
    bool dma_dummy = true;
};
