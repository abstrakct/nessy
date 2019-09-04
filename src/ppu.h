#pragma once

#include <cstdint>

class Bus;

#define PPUCtrl    0
#define PPUMask    1
#define PPUStatus  2
#define OAMAddr    3
#define OAMData    4
#define PPUScroll  5
#define PPUAddr    6
#define PPUData    7
#define OAMDMA     0x4014

class PPU {
    public:
        PPU();
        ~PPU();

        Bus *bus;
        void ConnectBus(Bus *n) { bus = n; }

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
        
        bool frame_complete();
        void render_scanline();
        void clock();
    private:
        int scanline = 0, cycles = 0;
};
