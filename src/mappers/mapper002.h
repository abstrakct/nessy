#pragma once
#include "../mapper.h"
#include <vector>

class Mapper002 : public Mapper
{
private:
    uint8_t selectedBank = 0;
    uint8_t lastBank = 0;
    std::vector<uint8_t> vram;
    std::vector<std::string> infoString = {};
    bool updateInfo = true;

public:
    Mapper002(uint8_t p, uint8_t c);
    ~Mapper002();

    bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) override;
    bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool cpuWriteData(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuReadData(uint16_t addr, uint8_t &data) override;
    bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuWriteData(uint16_t addr, uint8_t data) override;
    bool getMirrorType(int &data) override { return false; };
    void romOverwrite(uint16_t addr, uint8_t data) override{};
    void reset() override;

    int implementationStatus() override { return MI_WORKING; };
    std::vector<std::string> getInfoStrings() override;
};
