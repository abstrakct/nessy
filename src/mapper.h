
#pragma once
#include <cstdint>

#define MI_WORKING     0
#define MI_NOT_WORKING 1
#define MI_NOT_USABLE  2

class Mapper {
    public:
        Mapper(uint8_t p, uint8_t c);
        virtual ~Mapper();

        virtual bool cpuRead(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool cpuWriteData(uint16_t addr, uint8_t data) = 0;
        virtual bool ppuRead(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) = 0;

        // What is the status of the emulator's implementation of this mapper?
        virtual int implementationStatus() = 0;
    protected:
        uint8_t prgBanks = 0;
        uint8_t chrBanks = 0;
};
