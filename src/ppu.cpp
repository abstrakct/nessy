// The PPU!
//
// PPU: 16 kB address space ($0000 - $3FFF)
// Memory map:
// $0000 - $0FFF : Pattern table 0
// $1000 - $1FFF : Pattern table 1
// $2000 - $23FF : Nametable 0
// $2400 - $27FF : Nametable 1
// $2800 - $2BFF : Nametable 2
// $2C00 - $2FFF : Nametable 3
// $3000 - $3EFF : Mirrors of $2000 - $2EFF (PPU doesn't render from this range, can probably be ignored)
// $3F00 - $3F1F : Palette RAM indexes
// $3F20 - $3FFF : Mirrors of $3F00 - $3F1F
//
//
//
//
// $2000 - PPUCtrl
// VPHB SINN
// V = NMI Enable in vblank
// P = master/slave thing - ignore
// H = 0: 8x8 pixels; 1: 8x16 pixels
// B = Background pattern table - 0: $0000 - 1: $1000
// S = Sprite pattern table for 8x8 sprites (ignored in 8x16 mode)
//     0: $0000 - 1: $10000
// I = VRAM address increment per CPU read/write of PPUDATA
//     0: add 1 (going across)
//     1: add 32 (going down)
// NN= Base nametable address
//     0: 0x2000  1: 0x2400  2: 0x2800  3: 0x2C00


#include "machine.h"
#include "ppu.h"
#include "logger.h"

extern Logger l;

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

	palScreen[0x00] = olc::Pixel(84, 84, 84);
	palScreen[0x01] = olc::Pixel(0, 30, 116);
	palScreen[0x02] = olc::Pixel(8, 16, 144);
	palScreen[0x03] = olc::Pixel(48, 0, 136);
	palScreen[0x04] = olc::Pixel(68, 0, 100);
	palScreen[0x05] = olc::Pixel(92, 0, 48);
	palScreen[0x06] = olc::Pixel(84, 4, 0);
	palScreen[0x07] = olc::Pixel(60, 24, 0);
	palScreen[0x08] = olc::Pixel(32, 42, 0);
	palScreen[0x09] = olc::Pixel(8, 58, 0);
	palScreen[0x0A] = olc::Pixel(0, 64, 0);
	palScreen[0x0B] = olc::Pixel(0, 60, 0);
	palScreen[0x0C] = olc::Pixel(0, 50, 60);
	palScreen[0x0D] = olc::Pixel(0, 0, 0);
	palScreen[0x0E] = olc::Pixel(0, 0, 0);
	palScreen[0x0F] = olc::Pixel(0, 0, 0);

	palScreen[0x10] = olc::Pixel(152, 150, 152);
	palScreen[0x11] = olc::Pixel(8, 76, 196);
	palScreen[0x12] = olc::Pixel(48, 50, 236);
	palScreen[0x13] = olc::Pixel(92, 30, 228);
	palScreen[0x14] = olc::Pixel(136, 20, 176);
	palScreen[0x15] = olc::Pixel(160, 20, 100);
	palScreen[0x16] = olc::Pixel(152, 34, 32);
	palScreen[0x17] = olc::Pixel(120, 60, 0);
	palScreen[0x18] = olc::Pixel(84, 90, 0);
	palScreen[0x19] = olc::Pixel(40, 114, 0);
	palScreen[0x1A] = olc::Pixel(8, 124, 0);
	palScreen[0x1B] = olc::Pixel(0, 118, 40);
	palScreen[0x1C] = olc::Pixel(0, 102, 120);
	palScreen[0x1D] = olc::Pixel(0, 0, 0);
	palScreen[0x1E] = olc::Pixel(0, 0, 0);
	palScreen[0x1F] = olc::Pixel(0, 0, 0);

	palScreen[0x20] = olc::Pixel(236, 238, 236);
	palScreen[0x21] = olc::Pixel(76, 154, 236);
	palScreen[0x22] = olc::Pixel(120, 124, 236);
	palScreen[0x23] = olc::Pixel(176, 98, 236);
	palScreen[0x24] = olc::Pixel(228, 84, 236);
	palScreen[0x25] = olc::Pixel(236, 88, 180);
	palScreen[0x26] = olc::Pixel(236, 106, 100);
	palScreen[0x27] = olc::Pixel(212, 136, 32);
	palScreen[0x28] = olc::Pixel(160, 170, 0);
	palScreen[0x29] = olc::Pixel(116, 196, 0);
	palScreen[0x2A] = olc::Pixel(76, 208, 32);
	palScreen[0x2B] = olc::Pixel(56, 204, 108);
	palScreen[0x2C] = olc::Pixel(56, 180, 204);
	palScreen[0x2D] = olc::Pixel(60, 60, 60);
	palScreen[0x2E] = olc::Pixel(0, 0, 0);
	palScreen[0x2F] = olc::Pixel(0, 0, 0);

	palScreen[0x30] = olc::Pixel(236, 238, 236);
	palScreen[0x31] = olc::Pixel(168, 204, 236);
	palScreen[0x32] = olc::Pixel(188, 188, 236);
	palScreen[0x33] = olc::Pixel(212, 178, 236);
	palScreen[0x34] = olc::Pixel(236, 174, 236);
	palScreen[0x35] = olc::Pixel(236, 174, 212);
	palScreen[0x36] = olc::Pixel(236, 180, 176);
	palScreen[0x37] = olc::Pixel(228, 196, 144);
	palScreen[0x38] = olc::Pixel(204, 210, 120);
	palScreen[0x39] = olc::Pixel(180, 222, 120);
	palScreen[0x3A] = olc::Pixel(168, 226, 144);
	palScreen[0x3B] = olc::Pixel(152, 226, 180);
	palScreen[0x3C] = olc::Pixel(160, 214, 228);
	palScreen[0x3D] = olc::Pixel(160, 162, 160);
	palScreen[0x3E] = olc::Pixel(0, 0, 0);
	palScreen[0x3F] = olc::Pixel(0, 0, 0);
}

PPU::~PPU()
{
}

//bool PPU::frame_complete()
//{
//    return (scanline == 262);
//}


olc::Pixel& PPU::GetColorFromPaletteRam(uint8_t palette, uint8_t pixel)
{
    return palScreen[ppuRead(0x3F00 + (palette << 2) + pixel)];
}

olc::Sprite& PPU::GetPatterntable(uint8_t i, uint8_t palette)
{
    for (uint16_t tileY = 0; tileY < 16; tileY++) {
        for (uint16_t tileX = 0; tileX < 16; tileX++) {
            uint16_t offset = (tileY * 256) + (tileX * 16);
            for (uint8_t row = 0; row < 8; row++) {
                uint8_t tile_lo = ppuRead(i * 0x1000 + offset + row);
                uint8_t tile_hi = ppuRead(i * 0x1000 + offset + row + 8);
                for (uint8_t col = 0; col < 8; col++) {
                    uint8_t pixel = (tile_lo & 0x01) + (tile_hi & 0x01);
                    tile_lo >>= 1;
                    tile_hi >>= 1;
                    sprPatterntable[i].SetPixel(
                            (tileX * 8) + (7 - col),
                            (tileY * 8) + row,
                            GetColorFromPaletteRam(palette, pixel)
                            );
                }
            }
        }
    }

    return sprPatterntable[i];
}


void PPU::render_scanline()
{
}

void PPU::clock()
{
	//sprScreen.SetPixel(cycles - 1, scanline, palScreen[(rand() % 2) ? 0x3F : 0x30]);

    cycle++;

    if (cycle < 341) {
        // We are in a scanline
        // Each scanline takes 341 ppu clock cycles
        // PPU renders 262 scanlines per frame

        if (scanline == -1 && cycle == 1) {
            //nmiOccurred = false;
            reg[PPUStatus] &= ~PPUSTAT_VBLANK;
            reg[PPUStatus] &= ~PPUSTAT_SPRITE_HIT;
            reg[PPUStatus] &= ~PPUSTAT_SPRITE_OVERFLOW;
        } else if (scanline < 240) {
            // render scanlines
        } else if (scanline == 240) {
            // Post-render scanline - PPU idles here
        } else if (scanline == 241) {
            if (cycle == 1) {
                reg[PPUStatus] |= PPUSTAT_VBLANK;
                // This NMI can be disabled with reg 2000
                if (reg[PPUCtrl] & 0x80 && !nmiOccurred) {
                    //this->nes->cpu.nmi();
                    nmiOccurred = true;
                }
            }
        } else if (scanline > 241 && scanline < 261) {
            // During Vblank, the CPU can load data into the PPU
            // We probably don't need to do anything here
        }
    }

    // One scanline done
    if (cycle >= 341) {
        cycle = 0;
        scanline++;
        if (scanline >= 261) {
            // is this the pre-render line?
            scanline = -1;
            frame_complete = true;
        }
    }
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    if (readOnly)
        return reg[addr & 0x0007];

    switch (addr & 0x0007) {
        case PPUCtrl: break; // write only
        case PPUMask: break; // write only
        case PPUStatus:
            data = reg[PPUStatus] & 0xE0;
            data |= (vramBuffer & 0x1F);
            reg[PPUStatus] &= ~PPUSTAT_VBLANK;
            vramAddress = 0;
            flip = false;
            break;
        case OAMAddr: break; // write only
        case OAMData:
            if (reg[PPUStatus] & PPUSTAT_VBLANK)
                data = oam[oamAddr];
            break;
        case PPUScroll: break; // write only
        case PPUAddr: break; // write only
        case PPUData:
            // Read data
            data = vramBuffer;
            vramBuffer = this->ppuRead(vramAddress);
            if (vramAddress > 0x3F00)
                data = vramBuffer;

            // Increment address
            vramAddress += vramInc;
            break;
    }

    return data;
}

void PPU::cpuWrite(uint16_t addr, uint8_t data)
{
    //vramBuffer = data;
    if (addr >= 0x2000 && addr < 0x4000) {
        // not sure about the next line... but it seems correct?
        //reg[addr & 0x0007] = data;
        switch (addr & 0x0007) {
            case PPUCtrl: {
                auto old = reg[PPUCtrl];
                reg[PPUCtrl] = data;
                bool vOn = (!(old & PPUCTRL_NMI_ENABLE)) && (data & PPUCTRL_NMI_ENABLE);
                vramInc = (data & PPUCTRL_VRAM_INC) ? 32 : 1;

                if (vOn) {
                    // if NMI flag was changed from off to on, do an NMI
                    //nes->cpu.nmi();
                    nmiOccurred = true;
                }

                // also use nametable bits
                break;
            }
            case PPUMask: reg[PPUMask] = data; break;
            case PPUStatus: break; // read only
            case OAMAddr:
                            oamAddr = data; break;
            case OAMData:
                            oam[oamAddr++] = data; break;
            case PPUScroll: reg[PPUScroll] = data; break;
            case PPUAddr:
                            // write twice to set address
                            if (!flip) {
                                vramAddress = (vramAddress & 0x00FF) | (data << 8);
                                //hi = data;
                            } else {
                                vramAddress = (vramAddress & 0xFF00) | data;
                                //lo = data;
                            }

                            if (flip) {
                                //vramAddress = ((uint16_t)hi << 8) | (uint16_t)lo;
                                //printf("vramAddress set to %04x\n", vramAddress);
                            }

                            flip = !flip;

                            break;
            case PPUData: 
                            // Write data
                            this->ppuWrite(vramAddress, data);
                            // Increment address
                            vramAddress += vramInc;
                            break;
            default: break;
        }
    } else if (addr == 0x4014) {
        // Transfer 256 bytes to OAM
        // data = high byte of address
        // so if data = FF then address is FF00 - FFFF etc
        //printf("YO! OAM DMA IN DA HOUSE! data = %02X\n", data);
        //
        // TODO: takes 513-514 cycles
        // TODO: should start at oamAddr, not 0 (?)
        for (int i = 0; i < 256; i++) {
            //printf("oam dma address: %04x\n", ((uint16_t)data << 8) | ((uint8_t) (oamAddr + i)));
            oam[i] = nes->cpuRead(((uint16_t)data << 8) | ((uint8_t) (oamAddr + i)));
        }
    }
}

// THIS IS A MESS
uint8_t PPU::ppuRead(uint16_t addr, bool readOnly)
{
    char out[100];
    uint8_t data = 0x00;

    addr &= 0x3FFF;

    if (addr <= 0x3EFF)
        data = vramBuffer;

    // TODO: mapper 2 isn't working now........
    if (cart->ppuRead(addr, vramBuffer)) {
        data = vramBuffer;
        sprintf(out, "[ppuRead] addr: %04X  data: %02X  vramBuffer: %02X", addr, data, vramBuffer);
        l.w(out);
    } else if (addr >= 0x2000 && addr < 0x3000) {
        // Read from nametables
        if (cart->mirror == Cartridge::HORIZONTAL) {
            if (addr < 0x2800)
                // probably data -> vramBuffer
                data = nametable[0][addr & 0x03FF];
            else
                data = nametable[1][addr & 0x03FF];
        } else if (cart->mirror == Cartridge::VERTICAL) {
            if ((addr & 0x07FF) >= 0x400)
                data = nametable[1][addr & 0x03FF];
            else
                data = nametable[0][addr & 0x03FF];
        }
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        data = palette[addr];
    } else if (addr >= 0x3000) {
        printf("suspicious ppu read from addr %04x\n", addr);
    } else {
        data = vramBuffer;
    }


    return data;
}

void PPU::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
    
    //printf("ppuWrite: addr %04X  data %02X\n", addr, data);

    if (addr <= 0x3EFF)
        vramBuffer = data;

    if (cart->ppuWrite(addr, data)) {
    //} else if (addr < 0x2000) {
        //printf("write to patterntable %d %04x   data = %02x\n", (addr & 0x1000) >> 12, addr & 0x0FFF, data);
        //patterntable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    } else if (addr >= 0x2000 && addr < 0x3000) {
        // Write to nametables
        if (cart->mirror == Cartridge::HORIZONTAL) {
            if (addr < 0x2800)
                nametable[0][addr & 0x03FF] = data;
            else
                nametable[1][addr & 0x03FF] = data;
        } else if (cart->mirror == Cartridge::VERTICAL) {
            if ((addr & 0x07FF) >= 0x400)
                nametable[1][addr & 0x03FF] = data;
            else
                nametable[0][addr & 0x03FF] = data;
        }
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        //printf("writing palette data! addr = %04X (%d)  data  = %02X\n", addr, addr & 0x001F, data);
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        palette[addr] = data;
    } else if (addr >= 0x3000) {
        printf("suspicious ppu write to addr %04x\n", addr);
    }
}

void PPU::connectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
}

void PPU::reset()
{
    // probably incomplete
    this->cycle = 0;
    this->reg[PPUCtrl] = 0x00;
    this->reg[PPUMask] = 0x00;
    this->reg[PPUData] = 0x00;
}
