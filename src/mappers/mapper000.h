#pragma once
#include "../mapper.h"

class Mapper000 : public Mapper {
    public:
        Mapper000(uint8_t p, uint8_t c);
        ~Mapper000();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;

        int implementationStatus() override { return MI_WORKING; };
};
