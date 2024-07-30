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

#include "ppu.h"
#include "logger.h"
#include "machine.h"
#include <cstring>

extern Logger l;

PPU::PPU()
{
    nesScreen.create(340, 260, sf::Color::Black);
    sfmlPatterntable[0].create(128, 128, sf::Color::Black);
    sfmlPatterntable[1].create(128, 128, sf::Color::Black);
    sfmlOAM.create(512, 128, sf::Color::Black);

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

    // palette
    sfmlPalette[0x00] = sf::Color(84, 84, 84);
    sfmlPalette[0x01] = sf::Color(0, 30, 116);
    sfmlPalette[0x02] = sf::Color(8, 16, 144);
    sfmlPalette[0x03] = sf::Color(48, 0, 136);
    sfmlPalette[0x04] = sf::Color(68, 0, 100);
    sfmlPalette[0x05] = sf::Color(92, 0, 48);
    sfmlPalette[0x06] = sf::Color(84, 4, 0);
    sfmlPalette[0x07] = sf::Color(60, 24, 0);
    sfmlPalette[0x08] = sf::Color(32, 42, 0);
    sfmlPalette[0x09] = sf::Color(8, 58, 0);
    sfmlPalette[0x0A] = sf::Color(0, 64, 0);
    sfmlPalette[0x0B] = sf::Color(0, 60, 0);
    sfmlPalette[0x0C] = sf::Color(0, 50, 60);
    sfmlPalette[0x0D] = sf::Color(0, 0, 0);
    sfmlPalette[0x0E] = sf::Color(0, 0, 0);
    sfmlPalette[0x0F] = sf::Color(0, 0, 0);

    sfmlPalette[0x10] = sf::Color(152, 150, 152);
    sfmlPalette[0x11] = sf::Color(8, 76, 196);
    sfmlPalette[0x12] = sf::Color(48, 50, 236);
    sfmlPalette[0x13] = sf::Color(92, 30, 228);
    sfmlPalette[0x14] = sf::Color(136, 20, 176);
    sfmlPalette[0x15] = sf::Color(160, 20, 100);
    sfmlPalette[0x16] = sf::Color(152, 34, 32);
    sfmlPalette[0x17] = sf::Color(120, 60, 0);
    sfmlPalette[0x18] = sf::Color(84, 90, 0);
    sfmlPalette[0x19] = sf::Color(40, 114, 0);
    sfmlPalette[0x1A] = sf::Color(8, 124, 0);
    sfmlPalette[0x1B] = sf::Color(0, 118, 40);
    sfmlPalette[0x1C] = sf::Color(0, 102, 120);
    sfmlPalette[0x1D] = sf::Color(0, 0, 0);
    sfmlPalette[0x1E] = sf::Color(0, 0, 0);
    sfmlPalette[0x1F] = sf::Color(0, 0, 0);

    sfmlPalette[0x20] = sf::Color(236, 238, 236);
    sfmlPalette[0x21] = sf::Color(76, 154, 236);
    sfmlPalette[0x22] = sf::Color(120, 124, 236);
    sfmlPalette[0x23] = sf::Color(176, 98, 236);
    sfmlPalette[0x24] = sf::Color(228, 84, 236);
    sfmlPalette[0x25] = sf::Color(236, 88, 180);
    sfmlPalette[0x26] = sf::Color(236, 106, 100);
    sfmlPalette[0x27] = sf::Color(212, 136, 32);
    sfmlPalette[0x28] = sf::Color(160, 170, 0);
    sfmlPalette[0x29] = sf::Color(116, 196, 0);
    sfmlPalette[0x2A] = sf::Color(76, 208, 32);
    sfmlPalette[0x2B] = sf::Color(56, 204, 108);
    sfmlPalette[0x2C] = sf::Color(56, 180, 204);
    sfmlPalette[0x2D] = sf::Color(60, 60, 60);
    sfmlPalette[0x2E] = sf::Color(0, 0, 0);
    sfmlPalette[0x2F] = sf::Color(0, 0, 0);

    sfmlPalette[0x30] = sf::Color(236, 238, 236);
    sfmlPalette[0x31] = sf::Color(168, 204, 236);
    sfmlPalette[0x32] = sf::Color(188, 188, 236);
    sfmlPalette[0x33] = sf::Color(212, 178, 236);
    sfmlPalette[0x34] = sf::Color(236, 174, 236);
    sfmlPalette[0x35] = sf::Color(236, 174, 212);
    sfmlPalette[0x36] = sf::Color(236, 180, 176);
    sfmlPalette[0x37] = sf::Color(228, 196, 144);
    sfmlPalette[0x38] = sf::Color(204, 210, 120);
    sfmlPalette[0x39] = sf::Color(180, 222, 120);
    sfmlPalette[0x3A] = sf::Color(168, 226, 144);
    sfmlPalette[0x3B] = sf::Color(152, 226, 180);
    sfmlPalette[0x3C] = sf::Color(160, 214, 228);
    sfmlPalette[0x3D] = sf::Color(160, 162, 160);
    sfmlPalette[0x3E] = sf::Color(0, 0, 0);
    sfmlPalette[0x3F] = sf::Color(0, 0, 0);
}

PPU::~PPU()
{
}

void PPU::UpdatePPUInfo()
{
    info.nametableBaseAddress = 0x2000 + ((ctrl.reg & 0x3) * 0x400);
    info.vramInc = ctrl.incrementMode ? 32 : 1;
    info.spritePatternTableAddress8x8 = ctrl.reg & 0x08 ? 0x1000 : 0x0000;
    info.bgPatternTableAddress = ctrl.reg & 0x10 ? 0x1000 : 0x0000;
    info.spriteSize = ctrl.spriteSize;
    info.nmi = ctrl.enableNmi;

    info.oamAddress = oamAddr;
}

inline sf::Color &PPU::GetColorFromPaletteRam(uint8_t palette, uint8_t pixel)
{
    return sfmlPalette[ppuRead(0x3F00 + (palette << 2) + pixel)];
}

const sf::Image &PPU::GetPatterntable(uint8_t i, uint8_t palette)
{
    for (uint16_t tileY = 0; tileY < 16; tileY++) {
        for (uint16_t tileX = 0; tileX < 16; tileX++) {
            uint16_t offset = (tileY * 256) + (tileX * 16);
            for (uint8_t row = 0; row < 8; row++) {
                uint8_t tile_lo = ppuRead(i * 0x1000 + offset + row);
                uint8_t tile_hi = ppuRead(i * 0x1000 + offset + row + 8);
                for (uint8_t col = 0; col < 8; col++) {
                    uint8_t pixel = (tile_lo & 0x01) | ((tile_hi & 0x01) << 1);
                    tile_lo >>= 1;
                    tile_hi >>= 1;
                    sfmlPatterntable[i].setPixel(
                        (tileX * 8) + (7 - col),
                        (tileY * 8) + row,
                        // sfmlPalette[ppuRead(0x3F00 + (palette << 2) + pixel)]);
                        GetColorFromPaletteRam(palette, pixel));
                }
            }
        }
    }

    return sfmlPatterntable[i];
}

const sf::Image &PPU::GetOAM(uint8_t palette)
{
    return sfmlOAM;
}

void PPU::evaluateSprites()
{
    //    int n = 0;
    //    for (int i = 0; i < 64; i++) {
    //        int line = (scanline == 261 ? -1 : scanline) - oam[i*4 + 0];
    //        // If the sprite is in the scanline, copy its properties into secondary OAM:
    //        if (line >= 0 && line < spriteHeight()) {
    //            oamBuf2[n].id   = i;
    //            oamBuf2[n].y    = oam[i*4 + 0];
    //            oamBuf2[n].tile = oam[i*4 + 1];
    //            oamBuf2[n].attr = oam[i*4 + 2];
    //            oamBuf2[n].x    = oam[i*4 + 3];
    //
    //            if (++n > 8) {
    //                status.spriteOverflow = true;
    //                break;
    //            }
    //        }
    //    }
}

void PPU::loadSprites()
{
    uint16_t addr;
    for (int i = 0; i < 8; i++) {
        oamBuf[i] = oamBuf2[i]; // Copy secondary OAM into primary.

        // Different address modes depending on the sprite height:
        if (spriteHeight() == 16)
            addr = ((oamBuf[i].tile & 0x01) * 0x1000) + ((oamBuf[i].tile & ~1) * 16);
        else
            addr = (ctrl.spritePatternTable * 0x1000) + (oamBuf[i].tile * 16);

        unsigned sprY = (scanline - oamBuf[i].y) % spriteHeight(); // Line inside the sprite.
        if (oamBuf[i].attr & 0x80)
            sprY ^= spriteHeight() - 1; // Vertical flip.
        addr += sprY + (sprY & 0x08);   // Select the second tile if on 8x16.

        oamBuf[i].dataL = ppuRead(addr + 0);
        oamBuf[i].dataH = ppuRead(addr + 8);
    }
}

void PPU::clock()
{
    // auto IncrementScrollX = [&]() {
    //     if (mask.renderBackground || mask.renderSprites) {
    //         if (vramAddress.coarseX == 31) {
    //             vramAddress.coarseX = 0;
    //             vramAddress.nametableX = ~vramAddress.nametableX;
    //         } else {
    //             vramAddress.coarseX++;
    //         }
    //     }
    // };

    // auto IncrementScrollY = [&]() {
    //     if (mask.renderBackground || mask.renderSprites) {
    //         if (vramAddress.fineY < 7) {
    //             vramAddress.fineY++;
    //         } else {
    //             vramAddress.fineY = 0;
    //             if (vramAddress.coarseY == 29) {
    //                 vramAddress.coarseY = 0;
    //                 vramAddress.nametableY = ~vramAddress.nametableY;
    //             } else if (vramAddress.coarseY == 31) {
    //                 vramAddress.coarseY = 0;
    //             } else {
    //                 vramAddress.coarseY++;
    //             }
    //         }
    //     }
    // };

    // auto TransferAddressX = [&]() {
    //     if (mask.renderBackground || mask.renderSprites) {
    //         vramAddress.nametableX = tramAddress.nametableX;
    //         vramAddress.coarseX = tramAddress.coarseX;
    //     }
    // };

    // auto TransferAddressY = [&]() {
    //     if (mask.renderBackground || mask.renderSprites) {
    //         vramAddress.nametableY = tramAddress.nametableY;
    //         vramAddress.coarseY = tramAddress.coarseY;
    //         vramAddress.fineY = tramAddress.fineY;
    //     }
    // };

    // auto LoadBackgroundShifters = [&]() {
    //     bgShifterPatternLo = (bgShifterPatternLo & 0xFF00) | bgNextTileLo;
    //     bgShifterPatternHi = (bgShifterPatternHi & 0xFF00) | bgNextTileHi;
    //     bgShifterAttribLo = (bgShifterAttribLo & 0xFF00) | ((bgNextTileAttrib & 0b01) ? 0xFF : 0x00);
    //     bgShifterAttribHi = (bgShifterAttribHi & 0xFF00) | ((bgNextTileAttrib & 0b10) ? 0xFF : 0x00);
    // };

    // auto UpdateShifters = [&]() {
    //     if (mask.renderBackground) {
    //         bgShifterPatternLo <<= 1;
    //         bgShifterPatternHi <<= 1;
    //         bgShifterAttribLo <<= 1;
    //         bgShifterAttribHi <<= 1;
    //     }

    //     if (mask.renderSprites && cycle >= 1 && cycle < 258) {
    //         for (int i = 0; i < spriteCount; i++) {
    //             if (spriteScanline[i].x > 0) {
    //                 spriteScanline[i].x--;
    //             } else {
    //                 spriteShifterLo[i] <<= 1;
    //                 spriteShifterHi[i] <<= 1;
    //             }
    //         }
    //     }
    // };

    if (scanline >= -1 && scanline < 240) {
        if (scanline == 0 && cycle == 0 && oddFrame && (mask.renderBackground || mask.renderSprites)) {
            cycle = 1; // "odd frame" cycle skip
        }

        // Clear Vblank flag
        if (scanline == -1 && cycle == 1) {
            status.verticalBlank = 0;
            status.spriteOverflow = 0;
            status.spriteZeroHit = 0;
            for (int i = 0; i < 8; i++) {
                spriteShifterHi[i] = 0;
                spriteShifterLo[i] = 0;
            }
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

        // Load Background data
        if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
            // UpdateShifters();
            // Update shifters
            if (mask.renderBackground) {
                bgShifterPatternLo <<= 1;
                bgShifterPatternHi <<= 1;
                bgShifterAttribLo <<= 1;
                bgShifterAttribHi <<= 1;
            }

            if (mask.renderSprites && cycle >= 1 && cycle < 258) {
                for (int i = 0; i < spriteCount; i++) {
                    if (spriteScanline[i].x > 0) {
                        spriteScanline[i].x--;
                    } else {
                        spriteShifterLo[i] <<= 1;
                        spriteShifterHi[i] <<= 1;
                    }
                }
            }

            switch ((cycle - 1) % 8) {
            case 0:
                // LoadBackgroundShifters();
                bgShifterPatternLo = (bgShifterPatternLo & 0xFF00) | bgNextTileLo;
                bgShifterPatternHi = (bgShifterPatternHi & 0xFF00) | bgNextTileHi;
                bgShifterAttribLo = (bgShifterAttribLo & 0xFF00) | ((bgNextTileAttrib & 0b01) ? 0xFF : 0x00);
                bgShifterAttribHi = (bgShifterAttribHi & 0xFF00) | ((bgNextTileAttrib & 0b10) ? 0xFF : 0x00);
                bgNextTileId = ppuRead(0x2000 | (vramAddress.reg & 0x0FFF));
                break;
            case 2:
                // I HAVE NO IDEA WHAT'S GOING ON LOL
                bgNextTileAttrib = ppuRead(0x23C0 | (vramAddress.nametableY << 11) | (vramAddress.nametableX << 10) | ((vramAddress.coarseY >> 2) << 3) | (vramAddress.coarseX >> 2));
                if (vramAddress.coarseY & 0x02)
                    bgNextTileAttrib >>= 4;
                if (vramAddress.coarseX & 0x02)
                    bgNextTileAttrib >>= 2;
                bgNextTileAttrib &= 0x03;
                break;
            case 4:
                bgNextTileLo = ppuRead((ctrl.bgPatternTable << 12) + ((uint16_t)bgNextTileId << 4) + (vramAddress.fineY));
                break;
            case 6:
                bgNextTileHi = ppuRead((ctrl.bgPatternTable << 12) + ((uint16_t)bgNextTileId << 4) + (vramAddress.fineY) + 8);
                break;
            case 7:
                // IncrementScrollX();
                if (mask.renderBackground || mask.renderSprites) {
                    if (vramAddress.coarseX == 31) {
                        vramAddress.coarseX = 0;
                        vramAddress.nametableX = ~vramAddress.nametableX;
                    } else {
                        vramAddress.coarseX++;
                    }
                }
                break;
            }
        }

        if (cycle == 256) {
            // IncrementScrollY();
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
        }

        if (cycle == 257) {
            // LoadBackgroundShifters();
            bgShifterPatternLo = (bgShifterPatternLo & 0xFF00) | bgNextTileLo;
            bgShifterPatternHi = (bgShifterPatternHi & 0xFF00) | bgNextTileHi;
            bgShifterAttribLo = (bgShifterAttribLo & 0xFF00) | ((bgNextTileAttrib & 0b01) ? 0xFF : 0x00);
            bgShifterAttribHi = (bgShifterAttribHi & 0xFF00) | ((bgNextTileAttrib & 0b10) ? 0xFF : 0x00);
            // TransferAddressX();
            if (mask.renderBackground || mask.renderSprites) {
                vramAddress.nametableX = tramAddress.nametableX;
                vramAddress.coarseX = tramAddress.coarseX;
            }
        }

        if (cycle == 338 || cycle == 340)
            bgNextTileId = ppuRead(0x2000 | (vramAddress.reg & 0x0FFF));

        // Sprites / Foreground
        //
        // Clear secondary OAM
        //if (cycle == 1) {
        //    for (int i = 0; i < 8; i++) {
        //        oamBuf2[i].id    = 64;
        //        oamBuf2[i].y     = 0xFF;
        //        oamBuf2[i].tile  = 0xFF;
        //        oamBuf2[i].attr  = 0xFF;
        //        oamBuf2[i].x     = 0xFF;
        //        oamBuf2[i].dataL = 0;
        //        oamBuf2[i].dataH = 0;
        //    }
        //}

        //if (scanline && cycle == 257) {
        //    evaluateSprites();
        //}

        //if (scanline && cycle == 321) {
        //    loadSprites();
        //}

        if (scanline == -1 && cycle >= 280 && cycle < 305) {
            // End of vblank period, reset Y address
            // TransferAddressY();
            if (mask.renderBackground || mask.renderSprites) {
                vramAddress.nametableY = tramAddress.nametableY;
                vramAddress.coarseY = tramAddress.coarseY;
                vramAddress.fineY = tramAddress.fineY;
            }
        }

        // SPRITE STUFF
        // performs all sprite evaluation in one hit - this is not how the nes actually does it!

        if (cycle == 257 && scanline >= 0) {
            std::memset(spriteScanline, 0xFF, 8 * sizeof(ObjectAttributeEntry));
            spriteCount = 0;
            for (int i = 0; i < 8; i++) {
                spriteShifterHi[i] = 0;
                spriteShifterLo[i] = 0;
            }

            uint8_t oamEntry = 0;
            spriteZeroHitPossible = false;

            while (oamEntry < 64 && spriteCount < 9) {
                int16_t diff = ((int16_t)scanline - (int16_t)oam[oamEntry].y);
                if (diff >= 0 && diff < spriteHeight()) {
                    if (spriteCount < 8) {
                        if (oamEntry == 0)
                            spriteZeroHitPossible = true;
                        memcpy(&spriteScanline[spriteCount], &oam[oamEntry], sizeof(ObjectAttributeEntry));
                        spriteCount++;
                    }
                }
                oamEntry++;
            }

            status.spriteOverflow = (spriteCount > 8);
        }

        if (cycle == 340) {
            for (int i = 0; i < spriteCount; i++) {
                uint8_t spritePatternBitsLo, spritePatternBitsHi;
                uint16_t spritePatternAddrLo, spritePatternAddrHi;
                if (!ctrl.spriteSize) { // 8x8
                    if (!(spriteScanline[i].attr & 0x80)) {
                        // Sprite is NOT flipped vertically
                        spritePatternAddrLo = (ctrl.spritePatternTable << 12) | (spriteScanline[i].id << 4) | (scanline - spriteScanline[i].y);
                    } else {
                        // Sprite IS flipped vertically
                        spritePatternAddrLo = (ctrl.spritePatternTable << 12) | (spriteScanline[i].id << 4) | (7 - (scanline - spriteScanline[i].y));
                    }
                } else { // 8x16
                    if (!(spriteScanline[i].attr & 0x80)) {
                        // NOT flipped vertically
                        if (scanline - spriteScanline[i].y < 8) {
                            spritePatternAddrLo = ((spriteScanline[i].id & 0x01) << 12) | ((spriteScanline[i].id & 0xFE) << 4) | ((scanline - spriteScanline[i].y) & 0x07);
                        } else {
                            spritePatternAddrLo = ((spriteScanline[i].id & 0x01) << 12) | (((spriteScanline[i].id & 0xFE) + 1) << 4) | ((scanline - spriteScanline[i].y) & 0x07);
                        }
                    } else {
                        // FLIPPED vertically
                        if (scanline - spriteScanline[i].y < 8) {
                            spritePatternAddrLo = ((spriteScanline[i].id & 0x01) << 12) | (((spriteScanline[i].id & 0xFE) + 1) << 4) | ((7 - (scanline - spriteScanline[i].y)) & 0x07);
                        } else {
                            spritePatternAddrLo = ((spriteScanline[i].id & 0x01) << 12) | (((spriteScanline[i].id & 0xFE)) << 4) | ((7 - (scanline - spriteScanline[i].y)) & 0x07);
                        }
                    }
                }

                spritePatternAddrHi = spritePatternAddrLo + 8;
                spritePatternBitsLo = ppuRead(spritePatternAddrLo);
                spritePatternBitsHi = ppuRead(spritePatternAddrHi);

                if (spriteScanline[i].attr & 0x40) {
                    // flipped horizontally?
                    // This little lambda function "flips" a byte
                    // so 0b11100000 becomes 0b00000111. It's very
                    // clever, and stolen completely from here:
                    // https://stackoverflow.com/a/2602885
                    auto flipbyte = [](uint8_t b) {
                        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
                        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
                        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
                        return b;
                    };

                    // Flip Patterns Horizontally
                    spritePatternBitsLo = flipbyte(spritePatternBitsLo);
                    spritePatternBitsHi = flipbyte(spritePatternBitsHi);
                }

                spriteShifterLo[i] = spritePatternBitsLo;
                spriteShifterHi[i] = spritePatternBitsHi;
            }
        }
    }

    // END OF SPRITE STUFF

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

    //sprScreen.SetPixel(cycle - 1, scanline, GetColorFromPaletteRam(bgPalette, bgPixel));

    // let's not care about priority yet, let's just draw sprites
    //int x = cycle - 1;
    //uint8_t sprPixel = 0;
    //uint8_t sprPalette = 0;
    //bool front = false;
    //if (mask.renderSprites/* && !(!mask.renderSpritesLeft && x < 8)*/) {
    //    for (int i = 0; i < 8; i++) {
    //        if (oamBuf[i].id == 64) continue;
    //        unsigned sprX = x - oamBuf[i].x;
    //        if (sprX >= 8) continue;
    //        if (oamBuf[i].attr & 0x40)
    //            sprX ^= 7; // horizontal flip

    //        if (oamBuf[i].attr & 0x20)
    //            front = false;
    //        else
    //            front = true;

    //        sprPixel = ((((oamBuf[i].dataH >> (7 - sprX)) & 0x01) << 1) |
    //                     ((oamBuf[i].dataL >> (7 - sprX)) & 0x01));

    //        if (sprPixel == 0) continue;
    //
    //        if (oamBuf[i].id == 0 && sprPixel && x != 255)
    //            status.spriteZeroHit = 1;

    //        sprPalette = (oamBuf[i].attr & 0x03) + 4;
    //    }
    //}
    //

    uint8_t sprPixel = 0, sprPalette = 0;
    bool front = false;
    if (mask.renderSprites) {
        spriteZeroBeingRendered = false;
        for (uint8_t i = 0; i < spriteCount; i++) {
            if (spriteScanline[i].x == 0) {
                uint8_t lo = (spriteShifterLo[i] & 0x80) > 0;
                uint8_t hi = (spriteShifterHi[i] & 0x80) > 0;
                sprPixel = (hi << 1) | lo;
                sprPalette = (spriteScanline[i].attr & 0x03) + 4;
                front = (!(spriteScanline[i].attr & 0x20));
                if (sprPixel) {
                    if (i == 0)
                        spriteZeroBeingRendered = true;
                    break;
                }
            }
        }
    }

    uint8_t pixel = 0;
    uint8_t palette = 0;
    if (sprPixel == 0 && bgPixel == 0) {
        pixel = 0x00;
        palette = 0x00;
    } else if (sprPixel > 0 && bgPixel == 0) {
        pixel = sprPixel;
        palette = sprPalette;
    } else if (sprPixel == 0 && bgPixel > 0) {
        pixel = bgPixel;
        palette = bgPalette;
    } else if (sprPixel > 0 && bgPixel > 0) {
        if (front) {
            pixel = sprPixel;
            palette = sprPalette;
        } else {
            pixel = bgPixel;
            palette = bgPalette;
        }

        if (spriteZeroHitPossible && spriteZeroBeingRendered) {
            if (mask.renderBackground && mask.renderSprites) {
                if (~(mask.renderBackgroundLeft | mask.renderSpritesLeft)) {
                    if (cycle >= 9 && cycle < 258) {
                        status.spriteZeroHit = 1;
                    }
                } else {
                    if (cycle >= 1 && cycle < 258) {
                        status.spriteZeroHit = 1;
                    }
                }
            }
        }
    }

    // SFML
    if ((cycle - 1) >= 0 && scanline >= 0 && scanline < 260) {
        // printf("cycle %d scanline %d\n", cycle - 1, scanline);
        nesScreen.setPixel(cycle - 1, scanline, GetColorFromPaletteRam(palette, pixel));
    }
    // nesScreen.setPixel(cycle - 1, scanline, sfmlPalette[ppuRead(0x3F00 + (palette << 2) + pixel)]);

    //if (((sprPixel & 0x03) && front) || (bgPixel))
    //    sprScreen.SetPixel(x, scanline, GetColorFromPaletteRam(palette, pixel));

    cycle++;

    if (mask.renderBackground || mask.renderSprites) {
        if (cycle == 260 && scanline < 240) {
            // TODO: maybe add flag to see if the cart has anything useful to contribute here.
            cart->mapper->scanline();
        }
    }

    // One scanline done
    if (cycle >= 341) {
        cycle = 0;
        scanline++;
        if (scanline >= 261) {
            scanline = -1;
            frame_complete = true;
            oddFrame = !oddFrame;
        }
    }
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    if (readOnly) {
        switch (addr & 0x0007) {
        case PPUCtrl:
            data = ctrl.reg;
            break;
        case PPUMask:
            data = mask.reg;
            break;
        case PPUStatus:
            data = status.reg;
            break;
        case OAMAddr:
            data = oamAddr;
            break;
        case OAMData:
            data = oamPointer[oamAddr];
            break;
        case PPUScroll:
            data = 0;
            break;
        case PPUAddr:
            data = 0;
            break;
        case PPUData:
            data = vramBuffer;
            break;
        }
    } else {
        switch (addr & 0x0007) {
        case PPUCtrl:
            break; // write only
        case PPUMask:
            break; // write only
        case PPUStatus:
            data = (status.reg & 0xE0) | (vramBuffer & 0x1F);
            status.verticalBlank = 0;
            //vramAddress.reg = 0;
            flip = false;
            break;
        case OAMAddr:
            break; // write only
        case OAMData:
            //if (status.verticalBlank)
            data = oamPointer[oamAddr];
            break;
        case PPUScroll:
            break; // write only
        case PPUAddr:
            break; // write only
        case PPUData:
            // Read data
            data = vramBuffer;
            vramBuffer = this->ppuRead(vramAddress.reg);
            if (vramAddress.reg >= 0x3F00 && vramAddress.reg < 0x4000) {
                data = vramBuffer;
                vramBuffer = ppuRead(vramAddress.reg - 0x1000);
            }

            // Increment address
            vramAddress.reg += vramInc;
            break;
        }
    }

    return data;
}

void PPU::cpuWrite(uint16_t addr, uint8_t data)
{
    vramBuffer = data;
    if (addr >= 0x2000 && addr < 0x4000) {
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
        case PPUMask:
            mask.reg = data;
            break;
        case PPUStatus:
            break; // read only
        case OAMAddr:
            oamAddr = data;
            break;
        case OAMData:
            oamPointer[oamAddr] = data;
            break;
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
        default:
            break;
        }
    }
}

// THIS IS A BIT MESSY
// and probably not very efficient - optimize!
uint8_t PPU::ppuRead(uint16_t addr, bool readOnly)
{
    // char out[100];
    uint8_t data = 0x00;

    addr &= 0x3FFF;

    if (cart->ppuRead(addr, data)) {
        //data = vramBuffer;
        //sprintf(out, "[ppuRead] addr: %04X  data: %02X  vramBuffer: %02X", addr, data, vramBuffer);
        //l.w(out);
    } else if (addr > 0 && addr < 0x2000) {
        // If the cartridge can't handle this address
        data = patterntable[(addr & 0x1000) >> 12][addr & 0x0FFF];
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        if (addr >= 0x3000)
            addr -= 0x1000;
        // Read from nametables
        uint8_t mirrortype = cart->getMirrorType();
        if (mirrortype == Cartridge::HORIZONTAL) {
            // printf("horizontal mirroring\n");
            if (addr < 0x2800) {
                // probably data -> vramBuffer
                data = nametable[0][addr & 0x03FF];
                // printf("Read %02X from nametable 0, address %04X\n", data, addr & 0x03FF);
            } else {
                data = nametable[1][addr & 0x03FF];
            }
        } else if (mirrortype == Cartridge::VERTICAL) {
            // printf("vertical mirroring\n");
            if ((addr & 0x07FF) >= 0x400)
                data = nametable[1][addr & 0x03FF];
            else
                data = nametable[0][addr & 0x03FF];
        } else if (mirrortype == Cartridge::ONESCREEN_LO) {
            // One-screen mirroring: All nametables refer to the same memory at any given time, and the mapper directly manipulates CIRAM address bit 10 (e.g. many Rare games using AxROM)
            data = nametable[0][addr & 0x03FF];
        } else if (mirrortype == Cartridge::ONESCREEN_HI) {
            // One-screen mirroring: All nametables refer to the same memory at any given time, and the mapper directly manipulates CIRAM address bit 10 (e.g. many Rare games using AxROM)
            data = nametable[1][addr & 0x03FF];
        } else {
            printf("WARNING unsupported mirroring mode %d!\n", cart->getMirrorType());
        }
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x001F;
        if (addr == 0x0010)
            addr = 0x0000;
        if (addr == 0x0014)
            addr = 0x0004;
        if (addr == 0x0018)
            addr = 0x0008;
        if (addr == 0x001C)
            addr = 0x000C;
        data = palette[addr] & (mask.grayscale ? 0x30 : 0x3F);
        /*} else if (addr >= 0x3000) {
        data = ppuRead(addr - 0x1000);
        printf("suspicious ppu read from addr %04x\n", addr);
        */
    } else {
        // data = vramBuffer;
    }

    return data;
}

void PPU::ppuWrite(uint16_t addr, uint8_t data)
{
    uint8_t mirrortype = cart->getMirrorType();
    addr &= 0x3FFF;

    //printf("ppuWrite: addr %04X  data %02X\n", addr, data);

    // if (addr <= 0x3EFF)
    //    vramBuffer = data;

    if (cart->ppuWrite(addr, data)) {
    } else if (addr < 0x2000) {
        patterntable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        if (addr >= 0x3000)
            addr -= 0x1000;
        // Write to nametables
        if (mirrortype == Cartridge::HORIZONTAL) {
            if (addr < 0x2800)
                nametable[0][addr & 0x03FF] = data;
            else
                nametable[1][addr & 0x03FF] = data;
        } else if (mirrortype == Cartridge::VERTICAL) {
            if ((addr & 0x07FF) >= 0x400)
                nametable[1][addr & 0x03FF] = data;
            else
                nametable[0][addr & 0x03FF] = data;
        } else if (mirrortype == Cartridge::ONESCREEN_LO) {
            // One-screen mirroring: All nametables refer to the same memory at any given time, and the mapper directly manipulates CIRAM address bit 10 (e.g. many Rare games using AxROM)
            nametable[0][addr & 0x03FF] = data;
        } else if (mirrortype == Cartridge::ONESCREEN_HI) {
            // One-screen mirroring: All nametables refer to the same memory at any given time, and the mapper directly manipulates CIRAM address bit 10 (e.g. many Rare games using AxROM)
            nametable[1][addr & 0x03FF] = data;
        }
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x001F;
        if (addr == 0x0010)
            addr = 0x0000;
        if (addr == 0x0014)
            addr = 0x0004;
        if (addr == 0x0018)
            addr = 0x0008;
        if (addr == 0x001C)
            addr = 0x000C;
        palette[addr] = data;
    } /* else if (addr >= 0x3000) {
        ppuWrite(addr - 0x1000, data);
        printf("suspicious ppu write to addr %04x\n", addr);
    }*/
}

void PPU::connectCartridge(const std::shared_ptr<Cartridge> &cartridge)
{
    this->cart = cartridge;
}

void PPU::reset()
{
    fineX = 0;
    vramAddress.reg = 0;
    tramAddress.reg = 0;
    vramBuffer = 0;
    scanline = 0;
    cycle = 0;
    bgNextTileId = 0;
    bgNextTileAttrib = 0;
    bgNextTileHi = 0;
    bgNextTileLo = 0;
    bgShifterPatternLo = 0;
    bgShifterPatternHi = 0;
    bgShifterAttribLo = 0;
    bgShifterAttribHi = 0;
    status.reg = 0x00;
    ctrl.reg = 0x00;
    mask.reg = 0x00;
    flip = false;
}

// vim: fdm=syntax
