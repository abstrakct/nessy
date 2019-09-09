#pragma once

#include <memory>
#include <cstdint>

#include "cartridge.h"

class Machine;

#define PPUCtrl    0
#define PPUMask    1
#define PPUStatus  2
#define OAMAddr    3
#define OAMData    4
#define PPUScroll  5
#define PPUAddr    6
#define PPUData    7
#define OAMDMA     0x4014

#define PPU_SPRITE_OVERFLOW (1 << 5)
#define PPU_SPRITE_HIT      (1 << 6)
#define PPU_VBLANK          (1 << 7)

class PPU {
    private:
        int scanline = 0, cycles = 0;
        std::shared_ptr<Cartridge> cart;

        uint8_t nametable[2][1024];
        uint8_t palette[32];
        // this is something OLC is planning to use in the future...
        //uint8_t patterntable[2][4096];

    public:
        PPU();
        ~PPU();

        // TODO: shared_ptr
        Machine *nes;
        void ConnectMachine(Machine *n) { nes = n; }

        uint8_t ppuctrl   = 0;
        uint8_t ppumask   = 0;
        uint8_t ppustatus = 0;
        uint8_t oamaddr   = 0;
        uint8_t oamdata   = 0;
        uint8_t ppuscroll = 0;
        uint8_t ppuaddr   = 0;
        uint8_t ppudata   = 0;
        uint8_t oamdma    = 0;
        uint8_t reg[8];

        bool frame_complete = false;

        // registers
        //enum PPURegs {
        //    PPUCtrl   = 0x2000,
        //    PPUMask   = 0x2001,
        //    PPUStatus = 0x2002,
        //    OAMAddr   = 0x2003,
        //    OAMData   = 0x2004,
        //    PPUScroll = 0x2005,
        //    PPUAddr   = 0x2006,
        //    PPUData   = 0x2007,
        //    OAMDMA    = 0x4014,
        //};
        
        //bool frame_complete();
        void render_scanline();
        void clock();

        uint8_t cpuRead(uint16_t addr, bool readOnly = false);
        void cpuWrite(uint16_t addr, uint8_t data);

        uint8_t ppuRead(uint16_t addr, bool readOnly = false);
        void ppuWrite(uint16_t addr, uint8_t data);

        void connectCartridge(const std::shared_ptr<Cartridge>& cartridge);
};
