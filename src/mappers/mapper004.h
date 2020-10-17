#pragma once
#include "../mapper.h"
#include <vector>

class Mapper004 : public Mapper
{
private:
    uint16_t lastBank;
    uint8_t reg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<uint8_t> vram;

    bool irqActive = false;
    bool irqEnable = false;
    bool irqUpdate = false;
    uint16_t irqCounter = 0;
    uint16_t irqReload = 0;
    uint8_t command = 0;
    bool chrAddrSel, prgAddrSel;

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
    void romOverwrite(uint16_t addr, uint8_t data) override{};

    bool irqState() override;
    void irqClear() override;
    void scanline() override;

    void reset() override;

    int implementationStatus() override { return MI_DEVELOPMENT; };
    std::vector<std::string> getInfoStrings() override;

    void apply();
};
