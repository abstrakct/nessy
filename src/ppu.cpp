// The PPU!
//


#include "machine.h"
#include "ppu.h"

PPU::PPU()
{
    // set PPU to power-up state
    reg[PPUCtrl] = 0;
    reg[PPUMask] = 0;
    reg[PPUStatus] = 0;
    reg[OAMAddr] = 0;
    reg[PPUScroll] = 0;  // latch, might need changing?
    reg[PPUAddr] = 0;    // latch, might need changing?
    reg[PPUData] = 0;
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
        if (scanline == -1) {
            // ??
        } else if (scanline < 240) {
            //render_scanline();
            //
            // Here we actually render the screen (scanlines)
            //
        } else if (scanline == 241) {
            reg[PPUStatus] |= PPUSTAT_VBLANK;
            // This NMI can be disabled with reg 2000
            if (reg[PPUCtrl] & 0x80)
                this->nes->cpu.nmi();
        } else if (scanline > 241 && scanline < 261) {
            // During Vblank, the CPU can load data into the PPU
            // We probably don't need to do anything here
        } else if (scanline >= 261) {
            // is this the pre-render line?
            reg[PPUStatus] &= ~PPUSTAT_VBLANK;
            reg[PPUStatus] &= ~PPUSTAT_SPRITE_HIT;
            reg[PPUStatus] &= ~PPUSTAT_SPRITE_OVERFLOW;
            scanline = -1;
            frame_complete = true;
        }
    }
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    switch (addr % 8) {
        case PPUCtrl: break;
        case PPUMask: break;
        case PPUStatus:
            data = reg[PPUStatus] & 0xE0; // 3 high bits
            data |= (latch & 0x1F);
            reg[PPUStatus] &= ~PPUSTAT_VBLANK;
            // also clear vrm address flipflop thing
            break;
        case OAMAddr: break;
        case OAMData: break;
        case PPUScroll: break;
        case PPUAddr: break;
        case PPUData: break;
        default:
            data = latch;
            break;
    }

    // this will change
    //data = reg[addr % 8];
    //if ((addr % 8) == PPUStatus) {
    //    // Reading $2002 apparently clears this bit
    //    reg[PPUStatus] &= ~PPUSTAT_VBLANK;
    //}


    return data;
}

void PPU::cpuWrite(uint16_t addr, uint8_t data)
{
    latch = data;
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
