#pragma once
#include <vector>
#include "../mapper.h"
#include "../memory.h"

class Mapper033 : public Mapper {
    private:
        // Mapper data
        uint8_t mirror = 0;
        uint8_t prgBank[2];
        uint8_t chrBank[6];

        // Debug info
        std::vector<std::string> infoString = { };
        bool updateInfo = true;

    public:
        Mapper033(uint16_t p, uint16_t c);
        ~Mapper033();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWriteData(uint16_t addr, uint8_t data) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuReadData(uint16_t addr, uint8_t &data) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWriteData(uint16_t addr, uint8_t data) override;
        bool getMirrorType(int &data) override;
        void reset() override;
        
        void apply();

        int implementationStatus() override { return MI_DEVELOPMENT; };
        std::vector<std::string> getInfoStrings() override;
};

