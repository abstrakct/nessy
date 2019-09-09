#pragma once
#include "../mapper.h"

class Mapper_000 : public Mapper {
    public:
        Mapper_000(uint8_t p, uint8_t c);
        ~Mapper_000();

        bool cpuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool cpuWrite(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuRead(uint16_t addr, uint32_t &mapped_addr) override;
        bool ppuWrite(uint16_t addr, uint32_t &mapped_addr) override;
};
