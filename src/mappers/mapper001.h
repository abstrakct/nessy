#pragma once

#include "../mapper.h"
#include <vector>

class Mapper001 : public Mapper
{
private:
    uint8_t writes;
    uint8_t tmpreg;
    uint8_t reg[4];
    uint16_t lastBank;
    std::vector<uint8_t> vram;
    std::vector<std::string> infoString;
    bool updateInfo = true;

public:
    Mapper001(uint8_t p, uint8_t c);
    ~Mapper001();

    bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) override;
    bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool cpuWriteData(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuReadData(uint16_t addr, uint8_t &data) override;
    bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuWriteData(uint16_t addr, uint8_t data) override;
    bool getMirrorType(int &data) override;
    void romOverwrite(uint16_t addr, uint8_t data) override{};
    void reset() override;

    int implementationStatus() override { return MI_WORKING; };
    std::vector<std::string> getInfoStrings() override;

    // Apply settings written to the mapper's registers
    void apply();
};
