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


#include <cstring>
#include "machine.h"
#include "ppu.h"
#include "logger.h"

extern Logger l;

PPU::PPU()
{
    // set PPU to power-up state
    ctrl.reg = 0;
    mask.reg = 0;
    status.reg = 0;
    //reg[PPUCtrl] = 0;
    //reg[PPUMask] = 0;
    //reg[PPUStatus] = 0;
    //reg[OAMAddr] = 0;
    //reg[PPUScroll] = 0;  // latch, might need changing?
    //reg[PPUAddr] = 0;    // latch, might need changing?
    //reg[PPUData] = 0;

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
                    uint8_t pixel = (tile_lo & 0x01) + ((tile_hi & 0x01) << 1);
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

olc::Sprite& PPU::GetOAM(uint8_t palette)
{
    int tilex = 0, tiley = 0;

    //uint8_t sprite = 1;
    //uint8_t offset = sprite * 4;
    //uint8_t y = oam[offset];
    //uint8_t tile = oam[offset + 1];
    //uint8_t attr = oam[offset + 2];
    //uint8_t x = oam[offset + 3];

    //printf("Sprite %d\n", sprite);
    //printf("Y = %02X\n", y);
    //printf("T = %02X\n", tile);
    //printf("A = %02X\n", attr);
    //printf("X = %02X\n\n", x);

    for (uint16_t s = 0; s < 64; s++) {
        bool sixteen = ctrl.spriteSize ? true : false;
        uint16_t offset = s * 4;
        uint8_t y = oam[offset];
        uint8_t tileNum = oam[offset + 1];
        uint8_t attr = oam[offset + 2];
        uint8_t x = oam[offset + 3];

        uint8_t i = 0;
        if (!sixteen) {  // 8x8
            //printf("8x8");
            i = ctrl.spritePatternTable;
        } else {                 // 8x16
            //printf("8x16");
            i = tileNum & 0x01;
            tileNum >>= 1;
        }

        uint8_t tile;

        for (uint8_t row = 0; row < 8; row++) {
            tile = ppuRead(i * 0x1000 + tileNum + row);
            //uint8_t tile1 = ppuRead(i * 0x1000 + tileNum + row + 1);
            //uint8_t tile2 = ppuRead(i * 0x1000 + tileNum + row + 2);
            //uint8_t tile3 = ppuRead(i * 0x1000 + tileNum + row + 3);
            //uint8_t tile_hi = ppuRead(i * 0x1000 + tile + row + 8);
            for (uint8_t col = 0; col < 8; col++) {
                uint8_t pixel = tile & 0x01; //(tile_lo & 0x01) + ((tile_hi & 0x01) << 1);
                tile >>= 1;
                //tile_hi >>= 1;
                sprOAM.SetPixel(
                        (tilex * 8) + (7 - col),
                        (tiley * 8) + row,
                        GetColorFromPaletteRam(attr & 0x03, pixel)
                        );
            }
        }

        tilex++;
        if (tilex == 8) {
            tiley++;
            tilex = 0;
        }
    }

    return sprOAM;
}

void PPU::evaluateSprites()
{
    int n = 0;
    for (int i = 0; i < 64; i++) {
        int line = (scanline == 261 ? -1 : scanline) - oam[i*4 + 0];
        // If the sprite is in the scanline, copy its properties into secondary OAM:
        if (line >= 0 && line < spriteHeight()) {
            oamBuf2[n].id   = i;
            oamBuf2[n].y    = oam[i*4 + 0];
            oamBuf2[n].tile = oam[i*4 + 1];
            oamBuf2[n].attr = oam[i*4 + 2];
            oamBuf2[n].x    = oam[i*4 + 3];

            if (++n > 8) {
                status.spriteOverflow = true;
                break;
            }
        }
    }
}

void PPU::loadSprites()
{
    uint16_t addr;
    for (int i = 0; i < 8; i++) {
        oamBuf[i] = oamBuf2[i];  // Copy secondary OAM into primary.

        // Different address modes depending on the sprite height:
        if (spriteHeight() == 16)
            addr = ((oamBuf[i].tile & 0x01) * 0x1000) + ((oamBuf[i].tile & ~1) * 16);
        else
            addr = (ctrl.spritePatternTable * 0x1000) + (oamBuf[i].tile * 16);

        unsigned sprY = (scanline - oamBuf[i].y) % spriteHeight();  // Line inside the sprite.
        if (oamBuf[i].attr & 0x80)
            sprY ^= spriteHeight() - 1;      // Vertical flip.
        addr += sprY + (sprY & 0x08);         // Select the second tile if on 8x16.

        oamBuf[i].dataL = ppuRead(addr + 0);
        oamBuf[i].dataH = ppuRead(addr + 8);
    }
}

void PPU::clock()
{
    auto IncrementScrollX = [&]()
    {
        if (mask.renderBackground || mask.renderSprites) {
            if(vramAddress.coarseX == 31) {
                vramAddress.coarseX = 0;
                vramAddress.nametableX = ~vramAddress.nametableX;
            } else {
                vramAddress.coarseX++;
            }
        }
    };

    auto IncrementScrollY = [&]()
    {
        if (mask.renderBackground || mask.renderSprites) {
            if (vramAddress.fineY < 7) {
                vramAddress.fineY++;
            } else {
                vramAddress.fineY = 0;
                if (vramAddress.coarseY == 29) {
                    vramAddress.coarseY = 0;
                    vramAddress.nametableY = ~vramAddress.nametableY;
                } else if (vramAddress.coarseY == 31) {
                    vramAddress.coarseY = 0;
                } else {
                    vramAddress.coarseY++;
                }
            }
        }
    };

    auto TransferAddressX = [&]()
    {
        if (mask.renderBackground || mask.renderSprites) {
            vramAddress.nametableX = tramAddress.nametableX;
            vramAddress.coarseX = tramAddress.coarseX;
        }
    };

    auto TransferAddressY = [&]()
    {
        if (mask.renderBackground || mask.renderSprites) {
            vramAddress.nametableY = tramAddress.nametableY;
            vramAddress.coarseY = tramAddress.coarseY;
            vramAddress.fineY = tramAddress.fineY;
        }
    };

    auto LoadBackgroundShifters = [&]()
    {
        bgShifterPatternLo = (bgShifterPatternLo & 0xFF00) | bgNextTileLo;
        bgShifterPatternHi = (bgShifterPatternHi & 0xFF00) | bgNextTileHi;
        bgShifterAttribLo  = (bgShifterAttribLo  & 0xFF00) | ((bgNextTileAttrib & 0b01) ? 0xFF : 0x00);
        bgShifterAttribHi  = (bgShifterAttribHi  & 0xFF00) | ((bgNextTileAttrib & 0b10) ? 0xFF : 0x00);
    };

    auto UpdateShifters = [&]()
    {
        if (mask.renderBackground) {
            bgShifterPatternLo <<= 1;
            bgShifterPatternHi <<= 1;
            bgShifterAttribLo  <<= 1;
            bgShifterAttribHi  <<= 1;
        }
    };

    if (scanline >= -1 && scanline < 240) {
        if (scanline == 0 && cycle == 0) {
            cycle = 1; // "odd frame" cycle skip
        }

        // Clear Vblank flag
        if (scanline == -1 && cycle == 1) {
            status.verticalBlank = 0;
            status.spriteOverflow = 0;
            status.spriteZeroHit = 0;
        }

        //// Clear OAM 2
        //if (cycle >= 1 && cycle <= 64) {
        //    if (cycle == 1)
        //        std::memset(oam2, 0xFF, sizeof(uint8_t) * 32);
        //}

        //// Sprite evaluation
        //if (cycle >= 65 && cycle <= 256) {
        //    if (mask.renderSprites || mask.renderBackground) {
        //        if (cycle % 2) {               // odd cycle
        //            uint8_t spriteY = 0;
        //            if (spriteNum < 8) {
        //                spriteY = oam[spriteNum * 4];
        //                oam2[oam2index * 4] = spriteY;
        //            }
        //            if (spriteY < 0xEF) {      // EF = 239 = invisible
        //                // scanline 1:
        //                // y will be 238
        //                // 239 - 1
        //                if (spriteY == (239 - scanline)) {
        //                    for (int i = 1; i < 4; i++) 
        //                        oam2[oam2index + i] = oam[spriteNum * 4 + i];
        //                    oam2index++;
        //                    if (spriteNum >= 64) {
        //                    }
        //                }
        //            }
        //        } else {                       // even cycle
        //            if (oam2index < 8) {

        //            }
        //        }
        //    }
        //}
        //
        
        // Clear secondary OAM
        if (cycle == 1) {
            for (int i = 0; i < 8; i++) {
                oamBuf2[i].id    = 64;
                oamBuf2[i].y     = 0xFF;
                oamBuf2[i].tile  = 0xFF;
                oamBuf2[i].attr  = 0xFF;
                oamBuf2[i].x     = 0xFF;
                oamBuf2[i].dataL = 0;
                oamBuf2[i].dataH = 0;
            }
        }

        if (scanline && cycle == 257) {
            evaluateSprites();
        }

        if (scanline && cycle == 321) {
            loadSprites();
        }

        // Load Background data
        if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
            UpdateShifters();

            switch ((cycle - 1) % 8) {
                case 0: 
                    LoadBackgroundShifters();
                    bgNextTileId = ppuRead(0x2000 | (vramAddress.reg & 0x0FFF));
                    break;
                case 2:
                    // I HAVE NO IDEA WHAT'S GOING ON LOL
                    bgNextTileAttrib = ppuRead(0x23C0 |  (vramAddress.nametableY << 11)
                            |  (vramAddress.nametableX << 10)
                            | ((vramAddress.coarseY >> 2) << 3)
                            |  (vramAddress.coarseX >> 2));
                    if (vramAddress.coarseY & 0x02) bgNextTileAttrib >>= 4;
                    if (vramAddress.coarseX & 0x02) bgNextTileAttrib >>= 2;
                    bgNextTileAttrib &= 0x03;
                    break;
                case 4:
                    bgNextTileLo = ppuRead((ctrl.bgPatternTable << 12)
                            + ((uint16_t)bgNextTileId << 4)
                            + (vramAddress.fineY));
                    break;
                case 6:
                    bgNextTileHi = ppuRead((ctrl.bgPatternTable << 12)
                            + ((uint16_t)bgNextTileId << 4)
                            + (vramAddress.fineY) + 8);
                    break;
                case 7:
                    IncrementScrollX();
                    break;
            }
        }

        if (cycle == 256) {
            IncrementScrollY();
        }

        if (cycle == 257) {
            LoadBackgroundShifters();
            TransferAddressX();
        }

        if (cycle == 338 || cycle == 340)
            bgNextTileId = ppuRead(0x2000 | (vramAddress.reg & 0x0FFF));

        if (scanline == -1 && cycle >= 280 && cycle < 305) {
            // End of vblank period, reset Y address
            TransferAddressY();
        }
    }

    if (scanline == 240) {
        // Post-render scanline - PPU idles here
    }

    if (scanline >= 241 && scanline < 261) {
        if (scanline == 241 && cycle == 1) {
            status.verticalBlank = 1;
            // This NMI can be disabled with reg 2000
            if (ctrl.enableNmi) {
                nmiOccurred = true;
            }
        }
    }

    // We now have background pixel information for this cycle
    // Let's render!

    uint8_t bgPixel = 0;
    uint8_t bgPalette = 0;
    if (mask.renderBackground) {
        uint16_t mux = 0x8000 >> fineX;
        uint8_t p0 = (bgShifterPatternLo & mux) > 0;
        uint8_t p1 = (bgShifterPatternHi & mux) > 0;

        bgPixel = (p1 << 1) | p0;

        uint8_t pal0 = (bgShifterAttribLo & mux) > 0;
        uint8_t pal1 = (bgShifterAttribHi & mux) > 0;
        bgPalette = (pal1 << 1) | pal0;
    }

    sprScreen.SetPixel(cycle - 1, scanline, GetColorFromPaletteRam(bgPalette, bgPixel));

    // let's not care about priority yet, let's just draw sprites
    int x = cycle - 1;
    uint8_t sprPixel = 0;
    uint8_t sprPalette = 0;
    bool front = false;
    if (mask.renderSprites/* && !(!mask.renderSpritesLeft && x < 8)*/) {
        for (int i = 7; i >= 0; i--) {
            if (oamBuf[i].id == 64) continue;
            unsigned sprX = x - oamBuf[i].x;
            if (sprX >= 8) continue;
            if (oamBuf[i].attr & 0x40)
                sprX ^= 7; // horizontal flip

            if (oamBuf[i].attr & 0x20)
                front = false;
            else
                front = true;

            sprPixel = ((((oamBuf[i].dataH >> (7 - sprX)) & 0x01) << 1) |
                         ((oamBuf[i].dataL >> (7 - sprX)) & 0x01));

            if (sprPixel == 0) continue;
            
            if (oamBuf[i].id == 0 && bgPalette && x != 255)
                status.spriteZeroHit = 1;

            sprPalette = (oamBuf[i].attr & 0x03);
        }
    }

    if (((sprPixel & 0x03) && front) || (bgPixel == 0))
        sprScreen.SetPixel(x, scanline, GetColorFromPaletteRam(sprPalette, sprPixel));

    cycle++;

    // One scanline done
    if (cycle >= 341) {
        cycle = 0;
        scanline++;
        if (scanline >= 261) {
            scanline = -1;
            frame_complete = true;
        }
    }
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    //if (readOnly)
    //    return reg[addr & 0x0007];

    switch (addr & 0x0007) {
        case PPUCtrl: break; // write only
        case PPUMask: break; // write only
        case PPUStatus:
            data = status.reg & 0xE0;
            data |= (vramBuffer & 0x1F);
            // "simulate" sprite zero hits
            //if (cycle & 0x08)
            //    data |= 0x40;
            //else
            //    data &= ~0x40;

            status.verticalBlank = 0;
            //vramAddress.reg = 0;
            flip = false;
            break;
        case OAMAddr: break; // write only
        case OAMData:
            if (status.verticalBlank)
                data = oam[oamAddr];
            break;
        case PPUScroll: break; // write only
        case PPUAddr: break; // write only
        case PPUData:
            // Read data
            data = vramBuffer;
            vramBuffer = this->ppuRead(vramAddress.reg);
            if (vramAddress.reg >= 0x3F00)
                data = vramBuffer;

            // Increment address
            vramAddress.reg += vramInc;
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
                //auto old = ctrl;
                ctrl.reg = data;
                //bool vOn = (!(old.enableNmi)) && (ctrl.enableNmi);
                vramInc = (ctrl.incrementMode ? 32 : 1);

                tramAddress.nametableX = ctrl.nametableX;
                tramAddress.nametableY = ctrl.nametableY;

                //if (vOn) {
                    // if NMI flag was changed from off to on, do an NMI
                    //nes->cpu.nmi();
                    //nmiOccurred = true;
                //}

                break;
            }
            case PPUMask: mask.reg = data; break;
            case PPUStatus: break; // read only
            case OAMAddr:
                oamAddr = data; break;
            case OAMData:
                oam[oamAddr++] = data; break;
            case PPUScroll:
                if (!flip) {
                    fineX = data & 0x07;
                    tramAddress.coarseX = data >> 3;
                    flip = true;
                } else {
                    tramAddress.fineY = data & 0x07;
                    tramAddress.coarseY = data >> 3;
                    flip = false;
                }
                break;
            case PPUAddr:
                // write twice to set address
                if (!flip) {
                    tramAddress.reg = (uint16_t)((data & 0x3F) << 8) | (tramAddress.reg & 0x00FF);
                    flip = true;
                } else {
                    tramAddress.reg = (tramAddress.reg & 0xFF00) | data;
                    vramAddress = tramAddress;
                    flip = false;
                }
                break;
            case PPUData: 
                // Write data
                this->ppuWrite(vramAddress.reg, data);
                // Increment address
                vramAddress.reg += vramInc;
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
        //printf("\nOAMDMA written - transferred bytes from %04X\n", ((uint16_t)data << 8) | ((uint16_t) (oamAddr)));
        //for (int i = 0; i < 256; i++) {
        //    if (i && !(i % 8))
        //        printf("\n");
        //    printf("%02x ", oam[i]);
        //}
        //printf("\n");
    }
}

// THIS IS A BIT MESSY
uint8_t PPU::ppuRead(uint16_t addr, bool readOnly)
{
    char out[100];
    uint8_t data = 0x00;

    addr &= 0x3FFF;

    //if (addr <= 0x3EFF)
    //    data = vramBuffer;

    // TODO: mapper 2 isn't working now........
    if (cart->ppuRead(addr, data)) {
        //data = vramBuffer;
        sprintf(out, "[ppuRead] addr: %04X  data: %02X  vramBuffer: %02X", addr, data, vramBuffer);
        l.w(out);
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        if (addr >= 0x3000)
            addr -= 0x1000;
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
    /*} else if (addr >= 0x3000) {
        data = ppuRead(addr - 0x1000);
        printf("suspicious ppu read from addr %04x\n", addr);
        */
    } else {
        data = vramBuffer;
    }

    return data;
}

void PPU::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
    
    //printf("ppuWrite: addr %04X  data %02X\n", addr, data);

    //if (addr <= 0x3EFF)
    //    vramBuffer = data;

    if (cart->ppuWrite(addr, data)) {
    //} else if (addr < 0x2000) {
        //printf("write to patterntable %d %04x   data = %02x\n", (addr & 0x1000) >> 12, addr & 0x0FFF, data);
        //patterntable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    } else if (addr >= 0x2000 && addr < 0x3EFF) {
        if (addr >= 0x3000)
            addr -= 0x1000;
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
    }/* else if (addr >= 0x3000) {
        ppuWrite(addr - 0x1000, data);
        printf("suspicious ppu write to addr %04x\n", addr);
    }*/
}

void PPU::connectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
}

void PPU::reset()
{
    // very probably incomplete
    cycle = 0;
    ctrl.reg = 0x00;
    mask.reg = 0x00;
}


// vim: fdm=syntax
