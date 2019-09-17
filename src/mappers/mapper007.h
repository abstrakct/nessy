#pragma once
#include <vector>
#include "../mapper.h"

class Mapper007 : public Mapper {
    public:
        Mapper007(uint8_t p, uint8_t c);
        ~Mapper007();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWriteData(uint16_t addr, uint8_t data) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuReadData(uint16_t addr, uint8_t &data) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWriteData(uint16_t addr, uint8_t data) override;

        int implementationStatus() override { return MI_WORKING; };
    private:
        uint8_t  selectedBank   = 0;
        std::vector<uint8_t> vram;
};


