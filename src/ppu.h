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

// Bits 0-1: Base nametable address
// 0: 0x2000  1: 0x2400  2: 0x2800  3: 0x2C00
// 0: add 1, going across; 1: add 32, going down
#define PPUCTRL_VRAM_INC      (1 << 2)
// 0: 0x0000  1: 0x1000  - ignored in 8x16 mode
#define PPUCTRL_SPRITE_PTABLE (1 << 3)  
// 0: 0x0000  1: 0x1000
#define PPUCTRL_BG_PTABLE     (1 << 4)
// 0: 8x8  1: 8x16
#define PPUCTRL_SPRITE_SIZE   (1 << 5)
#define PPUCTRL_MASTER_SLAVE  (1 << 6)
#define PPUCTRL_NMI_ENABLE    (1 << 7)

#define PPUSTAT_SPRITE_OVERFLOW (1 << 5)
#define PPUSTAT_SPRITE_HIT      (1 << 6)
#define PPUSTAT_VBLANK          (1 << 7)

class PPU {
    private:
        int scanline = 0, cycles = 0;
        std::shared_ptr<Cartridge> cart;

        uint8_t nametable[2][1024];
        uint8_t palette[32];
        // this is something OLC is planning to use in the future...
        //uint8_t patterntable[2][4096];
        
        uint8_t latch;

    public:
        PPU();
        ~PPU();

        // TODO: shared_ptr
        // TODO: ppu reset
        Machine *nes;
        void ConnectMachine(Machine *n) { nes = n; }

        //uint8_t ppuctrl   = 0;
        //uint8_t ppumask   = 0;
        //uint8_t ppustatus = 0;
        //uint8_t oamaddr   = 0;
        //uint8_t oamdata   = 0;
        //uint8_t ppuscroll = 0;
        //uint8_t ppuaddr   = 0;
        //uint8_t ppudata   = 0;
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
