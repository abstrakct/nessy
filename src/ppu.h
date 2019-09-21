#pragma once

#include <memory>
#include <cstdint>

#include "olcPixelGameEngine.h"
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

// TODO: change registers to bitfield union

#define PPUMASK_RENDER_BG (1 << 3)

#define PPUSTAT_SPRITE_OVERFLOW (1 << 5)
#define PPUSTAT_SPRITE_HIT      (1 << 6)
#define PPUSTAT_VBLANK          (1 << 7)

class PPU {
    private:
        int scanline = 0, cycle = 0;
        std::shared_ptr<Cartridge> cart;

        //uint8_t patterntable[2][0x1000];  // 2x4096 bytes - maps to Cartridge
        uint8_t nametable[2][0x400];      // 2x1024 bytes, aka NES internal VRAM
        uint8_t palette[32];
        uint8_t oam[256];
        
        //uint16_t vramAddress = 0;
        uint8_t vramBuffer = 0, vramInc = 1;
        uint8_t lo = 0, hi = 0;
        uint8_t oamAddr = 0;
        bool flip = false;

        union {
            struct {
                uint8_t nametableX : 1;
                uint8_t nametableY : 1;
                uint8_t incrementMode : 1;
                uint8_t spritePatternTable : 1;
                uint8_t bgPatternTable : 1;
                uint8_t spriteSize : 1;
                uint8_t slaveMode : 1;   // unused
                uint8_t enableNmi : 1;
            };

            uint8_t reg;
        } ctrl; // $2000

        union {
            struct {
                uint8_t grayscale : 1;
                uint8_t renderBackgroundLeft : 1;
                uint8_t renderSpritesLeft : 1;
                uint8_t renderBackground : 1;
                uint8_t renderSprites : 1;
                uint8_t enhanceRed : 1;
                uint8_t enhanceGreen : 1;
                uint8_t enhanceBlue : 1;
            };

            uint8_t reg;
        } mask; // $2001
        
        union {
            struct {
                uint8_t unused : 5;
                uint8_t spriteOverflow : 1;
                uint8_t spriteZeroHit : 1;
                uint8_t verticalBlank : 1;
            };

            uint8_t reg;
        } status; // $2002

        union loopyRegister {
            struct {
                uint16_t coarseX : 5;
                uint16_t coarseY : 5;
                uint16_t nametableX : 1;
                uint16_t nametableY : 1;
                uint16_t fineY : 3;
                uint16_t unused : 1;
            };

            uint16_t reg = 0x0000;
        };

        loopyRegister vramAddress;
        loopyRegister tramAddress;

        uint8_t fineX = 0;

        uint8_t bgNextTileId = 0;
        uint8_t bgNextTileAttrib = 0;
        uint8_t bgNextTileHi = 0;
        uint8_t bgNextTileLo = 0;
        uint16_t bgShifterPatternLo = 0;
        uint16_t bgShifterPatternHi = 0;
        uint16_t bgShifterAttribLo = 0;
        uint16_t bgShifterAttribHi = 0;



        // Output
        olc::Pixel  palScreen[0x40];
        olc::Sprite sprScreen = olc::Sprite(256, 240);
        olc::Sprite sprNametable[2]    = { olc::Sprite(256, 240), olc::Sprite(256, 240) };
        olc::Sprite sprPatterntable[2] = { olc::Sprite(128, 128), olc::Sprite(128, 128) };

    public:
        PPU();
        ~PPU();

        // TODO: shared_ptr
        // TODO: ppu reset
        Machine *nes;
        void ConnectMachine(Machine *n) { nes = n; }

        //uint8_t oamdma    = 0;
        //uint8_t reg[8];

        bool nmiOccurred = false;

        bool frame_complete = false;
        olc::Sprite& GetScreen() { return sprScreen; };
        olc::Sprite& GetNametable(uint8_t i) { return sprNametable[i]; };
        olc::Sprite& GetPatterntable(uint8_t i, uint8_t palette);
        olc::Pixel& GetColorFromPaletteRam(uint8_t palette, uint8_t pixel);

        //bool frame_complete();
        void render_scanline();
        void clock();

        uint8_t cpuRead(uint16_t addr, bool readOnly = false);
        void cpuWrite(uint16_t addr, uint8_t data);

        uint8_t ppuRead(uint16_t addr, bool readOnly = false);
        void ppuWrite(uint16_t addr, uint8_t data);

        void connectCartridge(const std::shared_ptr<Cartridge>& cartridge);
        void reset();
};
