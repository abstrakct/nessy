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
    scanline++;
}

void PPU::clock()
{
    // just a simple hack for now
    render_scanline();
    if (scanline == 241) {
        reg[PPUStatus] |= 0x80;
    } else if (scanline > 262) {
        scanline = 0;
        reg[PPUStatus] &= ~0x80;
    }
}
