#pragma once

#include <vector>
#include "../mapper.h"

class Mapper001 : public Mapper {
    private:
        uint8_t writes;
        uint8_t tmpreg;
        uint8_t reg[4];
        uint32_t lastBankOffset;
        std::vector<uint8_t> vram;

    public:
        Mapper001(uint8_t p, uint8_t c);
        ~Mapper001();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWriteData(uint16_t addr, uint8_t data) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuReadData(uint16_t addr, uint8_t &data) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWriteData(uint16_t addr, uint8_t data) override;
        bool getMirrorType(int &data) override;

        int implementationStatus() override { return MI_DEVELOPMENT; };
};

