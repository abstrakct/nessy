// The PPU!
//


#include "ppu.h"

PPU::PPU()
{
}

PPU::~PPU()
{
}

bool PPU::frame_complete()
{
    return (scanline == 262);
}

void PPU::render_scanline()
{
}

void PPU::clock()
{
    // just a simple hack for now
    if (scanline < 240) {
        reg[PPUStatus] &= ~PPU_SPRITE_OVERFLOW;
        render_scanline();
    } else if (scanline == 241) {
        reg[PPUStatus] |= PPU_VBLANK;
    } else if (scanline == 261) {
        reg[PPUStatus] &= ~PPU_VBLANK;
        scanline = 0;
    }

    scanline++;
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    // this will change
    data = reg[addr % 8];

    return data;
}

void PPU::cpuWrite(uint16_t addr, uint8_t data)
{
    // this will change
    reg[addr % 8] = data;
}

uint8_t PPU::ppuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    addr &= 0x3FFF;

    return data;
}

void PPU::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
}

