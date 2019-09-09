
#pragma once
#include <cstdint>

class Mapper {
    public:
        Mapper(uint8_t p, uint8_t c);
        virtual ~Mapper();

        virtual bool cpuRead(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool ppuRead(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) = 0;

    protected:
        uint8_t prgBanks = 0;
        uint8_t chrBanks = 0;
};
