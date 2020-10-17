
#pragma once

#include <cstdint>
#include <memory>

#include "memory.h"

#define MI_NOT_WORKING 0
#define MI_DEVELOPMENT 1
#define MI_WORKING 2

class Mapper
{
public:
    Mapper(uint16_t p, uint16_t c);
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
    virtual void romOverwrite(uint16_t addr, uint8_t data) = 0;

    virtual void reset() = 0; // Reset anything in the mapper

    virtual bool getMirrorType(int &data) = 0;

    virtual bool irqState();
    virtual void irqClear();
    virtual void scanline();

    virtual std::vector<std::string> getInfoStrings() = 0;

    // What is the status of the emulator's implementation of this mapper?
    virtual int implementationStatus() = 0;

    void setPrgROM(std::shared_ptr<BankedMemory> p)
    {
        prgROM = p;
    };

    void setChrROM(std::shared_ptr<BankedMemory> p)
    {
        chrROM = p;
    };

    const char *implementationStatusDescription(int i);

    // Link back to the Cartridge
    //std::shared_ptr<Cartridge> cart;
protected:
    uint16_t prgBanks = 0;
    uint16_t chrBanks = 0;
    std::shared_ptr<BankedMemory> prgROM;
    std::shared_ptr<BankedMemory> chrROM;
};
