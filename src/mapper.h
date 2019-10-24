
#pragma once
#include <cstdint>

#define MI_WORKING     0
#define MI_DEVELOPMENT 1
#define MI_NOT_WORKING 2

class Mapper {
    public:
        Mapper(uint8_t p, uint8_t c);
        virtual ~Mapper();

        virtual bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) = 0;
        virtual bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool cpuWriteData(uint16_t addr, uint8_t data) = 0;
        // ppu...Data could instead just use the ppu... methods and map the
        // address to chrMem in Cartridge, but I like this approach.
        virtual bool ppuRead(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool ppuReadData(uint16_t addr, uint8_t &data) = 0;
        virtual bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) = 0;
        virtual bool ppuWriteData(uint16_t addr, uint8_t data) = 0;

        virtual bool getMirrorType(int &data) = 0;

        // What is the status of the emulator's implementation of this mapper?
        virtual int implementationStatus() = 0;

        // Link back to the Cartridge
        //std::shared_ptr<Cartridge> cart;
    protected:
        uint8_t prgBanks = 0;
        uint8_t chrBanks = 0;
};
