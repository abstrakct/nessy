// The PPU!
//


#include "machine.h"
#include "ppu.h"

PPU::PPU()
{
}

PPU::~PPU()
{
}

//bool PPU::frame_complete()
//{
//    return (scanline == 262);
//}

void PPU::render_scanline()
{
}

void PPU::clock()
{
    cycles++;

    if (cycles >= 341) {
        cycles = 0;
        scanline++;
        if (scanline < 240) {
            //render_scanline();
        } else if (scanline == 241) {
            reg[PPUStatus] |= PPU_VBLANK;
            this->nes->cpu.nmi();
        } else if (scanline >= 261) {
            reg[PPUStatus] &= ~PPU_VBLANK;
            reg[PPUStatus] &= ~PPU_SPRITE_HIT;
            reg[PPUStatus] &= ~PPU_SPRITE_OVERFLOW;
            scanline = -1;
            frame_complete = true;
        }
    }
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

    if (cart->ppuRead(addr, data)) {
    }

    return data;
}

void PPU::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
    if (cart->ppuWrite(addr, data)) {
    }
}

void PPU::connectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge ;
}
