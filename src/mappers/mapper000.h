#pragma once
#include "../mapper.h"

#include <string>
#include <vector>

class Mapper000 : public Mapper
{
public:
    Mapper000(uint8_t p, uint8_t c);
    ~Mapper000();

    bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) override;
    bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool cpuWriteData(uint16_t addr, uint8_t data) override { return false; };
    bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuReadData(uint16_t addr, uint8_t &data) override { return false; };
    bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuWriteData(uint16_t addr, uint8_t data) override { return false; };
    void romOverwrite(uint16_t addr, uint8_t data) override;
    bool getMirrorType(int &data) override { return false; };
    void reset() override;

    int implementationStatus() override { return MI_WORKING; };
    std::vector<std::string> getInfoStrings() override { return infoString; };

    const std::vector<std::string> infoString = {"MAPPER 000"};
};
