#pragma once
#include "../mapper.h"

class Mapper001 : public Mapper {
    public:
        Mapper001(uint8_t p, uint8_t c);
        ~Mapper001();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWriteData(uint16_t addr, uint8_t data) override { return false; };
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuReadData(uint16_t addr, uint8_t &data) override { return false; };
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWriteData(uint16_t addr, uint8_t data) override { return false; };

        int implementationStatus() override { return MI_NOT_USABLE; };
};

