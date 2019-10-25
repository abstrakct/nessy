#pragma once
#include <vector>
#include "../mapper.h"

class Mapper011 : public Mapper {
    private:
        uint8_t  selectedPrgBank   = 0;
        uint8_t  selectedChrBank   = 0;
        std::vector<std::string> infoString;
        bool updateInfo = true;
        //std::vector<uint8_t> vram;
        
    public:
        Mapper011(uint8_t p, uint8_t c);
        ~Mapper011();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWriteData(uint16_t addr, uint8_t data) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuReadData(uint16_t addr, uint8_t &data) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWriteData(uint16_t addr, uint8_t data) override;
        bool getMirrorType(int &data) override { return false; };
        void reset() override;

        void apply();

        int implementationStatus() override { return MI_WORKING; };
        std::vector<std::string> getInfoStrings() override;
};
