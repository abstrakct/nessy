#pragma once

#include <cstdint>
#include <memory>

#include <SFML/Graphics.hpp>

#include "cartridge.h"

class Machine;

#define PPUCtrl 0
#define PPUMask 1
#define PPUStatus 2
#define OAMAddr 3
#define OAMData 4
#define PPUScroll 5
#define PPUAddr 6
#define PPUData 7
#define OAMDMA 0x4014

// Bits 0-1: Base nametable address
// 0: add 1, going across; 1: add 32, going down
#define PPUCTRL_VRAM_INC (1 << 2)
// 0: 0x0000  1: 0x1000  - ignored in 8x16 mode
#define PPUCTRL_SPRITE_PTABLE (1 << 3)
// 0: 0x0000  1: 0x1000
#define PPUCTRL_BG_PTABLE (1 << 4)
// 0: 8x8  1: 8x16
#define PPUCTRL_SPRITE_SIZE (1 << 5)
#define PPUCTRL_MASTER_SLAVE (1 << 6)
#define PPUCTRL_NMI_ENABLE (1 << 7)

// TODO: change registers to bitfield union

#define PPUMASK_RENDER_BG (1 << 3)

#define PPUSTAT_SPRITE_OVERFLOW (1 << 5)
#define PPUSTAT_SPRITE_HIT (1 << 6)
#define PPUSTAT_VBLANK (1 << 7)

/* OAM
 * Byte 0 = Y position of top of sprite ($EF - $FF = invisible)
 * Byte 1 = Tile index number.
 *          For 8x8 sprites: tile number within pattern table selected in PPUCTRL
 *          For 8x16 sprites: PPU ignores PPUCTRL, selects PT  from bit 0 of this
 *                            number.
 * Byte 2 = Attributes
 * 765---10
   ||||||||
   ||||||++- Palette (4 to 7) of sprite
   |||+++--- Unimplemented
   ||+------ Priority (0: in front of background; 1: behind background)
   |+------- Flip sprite horizontally
   +-------- Flip sprite vertically

   Byte 3 = X position of left side of sprite ($F9 - $FF = invisible)

 */

class PPU
{
private:
    /* Sprite buffer (taken from LaiNES) */
    struct SpriteBuffer {
        uint8_t id;    // Index in OAM.
        uint8_t x;     // X position.
        uint8_t y;     // Y position.
        uint8_t tile;  // Tile index.
        uint8_t attr;  // Attributes.
        uint8_t dataL; // Tile data (low).
        uint8_t dataH; // Tile data (high).
    };

    int scanline = 0, cycle = 0;
    std::shared_ptr<Cartridge> cart;

    uint8_t patterntable[2][0x1000]; // 2x4096 bytes - maps to Cartridge
    uint8_t nametable[2][0x400];     // 2x1024 bytes, aka NES internal VRAM
    uint8_t palette[32];
    //uint8_t oam[256];  // 64 sprites, 4 bytes each, 256 bytes total
    SpriteBuffer oamBuf[8], oamBuf2[8];
    //uint8_t oam2[32];  //  8 sprites, 4 bytes each, 32 bytes total

    //uint16_t vramAddress = 0;
    uint8_t vramBuffer = 0, vramInc = 1;
    // uint8_t lo = 0, hi = 0;
    bool flip = false;

    union {
        struct
        {
            uint8_t nametableX : 1;
            uint8_t nametableY : 1;
            uint8_t incrementMode : 1;
            uint8_t spritePatternTable : 1;
            uint8_t bgPatternTable : 1;
            uint8_t spriteSize : 1;
            uint8_t slaveMode : 1; // unused
            uint8_t enableNmi : 1;
        };

        uint8_t reg;
    } ctrl; // $2000

    union {
        struct
        {
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
        struct
        {
            uint8_t unused : 5;
            uint8_t spriteOverflow : 1;
            uint8_t spriteZeroHit : 1;
            uint8_t verticalBlank : 1;
        };

        uint8_t reg;
    } status; // $2002

    union loopyRegister {
        struct
        {
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

    // Background data
    uint8_t bgNextTileId = 0;
    uint8_t bgNextTileAttrib = 0;
    uint8_t bgNextTileHi = 0;
    uint8_t bgNextTileLo = 0;
    uint16_t bgShifterPatternLo = 0;
    uint16_t bgShifterPatternHi = 0;
    uint16_t bgShifterAttribLo = 0;
    uint16_t bgShifterAttribHi = 0;

    // Sprite data
    struct ObjectAttributeEntry {
        uint8_t y;
        uint8_t id;
        uint8_t attr;
        uint8_t x;
    } oam[64];
    // uint8_t spriteNum = 0, oam2index = 0;
    uint8_t spriteCount;
    uint8_t spriteShifterLo[8];
    uint8_t spriteShifterHi[8];
    bool spriteZeroHitPossible = false;
    bool spriteZeroBeingRendered = false;
    uint8_t *oamPointer = (uint8_t *)oam;
    ObjectAttributeEntry spriteScanline[8];

    // Sprite functions
    void evaluateSprites();
    void loadSprites();

    // SFML Output
    sf::Image nesScreen;
    sf::Image sfmlPatterntable[2];

    // Inline functions
    inline int spriteHeight()
    {
        return (ctrl.spriteSize == 1) ? 16 : 8;
    }

public:
    PPU();
    ~PPU();

    // TODO: shared_ptr
    // TODO: ppu reset
    Machine *nes;
    void ConnectMachine(Machine *n) { nes = n; }

    bool nmiOccurred = false;

    bool frame_complete = false;

    // SFML stuff
    sf::Color sfmlPalette[0x40];
    sf::Color &GetColorFromPaletteRam(uint8_t palette, uint8_t pixel);
    const sf::Image &GetNesScreen() { return nesScreen; };
    const sf::Image &GetPatterntable(uint8_t i, uint8_t palette);

    // OAM stuff
    uint8_t oamAddr = 0;
    inline void oamWrite(uint8_t addr, uint8_t data)
    {
        oamPointer[addr] = data;
    };

    // Clock
    void clock();

    // CPU Bus IO
    uint8_t cpuRead(uint16_t addr, bool readOnly = false);
    void cpuWrite(uint16_t addr, uint8_t data);

    // PPU Bus IO
    uint8_t ppuRead(uint16_t addr, bool readOnly = false);
    void ppuWrite(uint16_t addr, uint8_t data);

    // Connect / Reset
    void connectCartridge(const std::shared_ptr<Cartridge> &cartridge);
    void reset();
};
