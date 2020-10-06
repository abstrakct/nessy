#pragma once
#include <vector>
#include "../mapper.h"

class Mapper004 : public Mapper
{
private:
    uint16_t lastBank;
    uint8_t reg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<uint8_t> vram;

    // Debug info
    std::vector<std::string> infoString = {};
    bool updateInfo = true;

public:
    Mapper004(uint8_t p, uint8_t c);
    ~Mapper004();

    bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) override;
    bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool cpuWriteData(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuReadData(uint16_t addr, uint8_t &data) override;
    bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuWriteData(uint16_t addr, uint8_t data) override;
    bool getMirrorType(int &data) override { return false; };
    void reset() override;

    int implementationStatus() override { return MI_DEVELOPMENT; };
    std::vector<std::string> getInfoStrings() override;

    void apply();
};
