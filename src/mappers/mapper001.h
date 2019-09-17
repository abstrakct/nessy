#pragma once
#include "../mapper.h"

class Mapper001 : public Mapper {
    public:
        Mapper001(uint8_t p, uint8_t c);
        ~Mapper001();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;

        int implementationStatus() override { return MI_NOT_USABLE; };
};

