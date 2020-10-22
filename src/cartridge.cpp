
#include "cartridge.h"
#include "logger.h"
#include "mappers/mapper000.h"
#include "mappers/mapper001.h"
#include "mappers/mapper002.h"
#include "mappers/mapper003.h"
#include "mappers/mapper004.h"
#include "mappers/mapper007.h"
#include "mappers/mapper011.h"
#include "mappers/mapper013.h"
#include "mappers/mapper033.h"
#include "mappers/mapper066.h"
#include <fstream>

extern Logger l;

Cartridge::Cartridge(const std::string &filename)
{
    struct header_struct {
        char name[4];
        uint8_t prg_rom_chunks;
        uint8_t chr_rom_chunks;
        uint8_t mapper1, mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1, tv_system2;
        char unused[5]; // there can be information here, implement/use later
    } header;

    valid = false;
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::binary);

    if (!ifs) {
        printf("ERROR: couldn't open file!\n");
        //exit(1);
    }

    if (ifs.is_open()) {
        // TODO: check first 4 bytes
        ifs.read((char *)&header, sizeof(header_struct));

        if (header.mapper1 & 0x04) // then 512 bytes of trainer data
            ifs.seekg(512, std::ios_base::cur);

        mapperNum = (header.mapper1 >> 4) | ((header.mapper2 >> 4) << 4);

        if (header.mapper1 & 0x08) {
            mirrorType = FOUR_SCREEN;
        } else {
            mirrorType = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;
        }

        // discover file format
        uint8_t filetype = 1;
        if (filetype == 0) {
        } else if (filetype == 1) {
            prgBanks = header.prg_rom_chunks;
            chrBanks = header.chr_rom_chunks;
            firstBank = 0;
            lastBank = prgBanks - 1;

            const int defaultPrgBankSize = 0x4000;
            int bankSize = defaultPrgBankSize;

            // TODO: ask mapper what bank size it wants
            if (mapperNum == 4 || mapperNum == 33)
                bankSize = 0x2000;
            if (mapperNum == 0)
                bankSize = 0x8000;

            prgBanks = (header.prg_rom_chunks * defaultPrgBankSize) / bankSize;
            prgROM = std::make_shared<BankedMemory>("PRG ROM", prgBanks, bankSize);

            printf("\n\tLoading %d PRG ROM banks of size 0x%04X (%d bytes)", prgBanks, bankSize, prgBanks * bankSize);
            // for (int i = 0; i < prgBanks; i++) {
            std::vector<uint8_t> tmp;
            tmp.resize(bankSize * prgBanks);
            ifs.read((char *)tmp.data(), bankSize * prgBanks);
            prgROM->addData(tmp);

            // printf(".");

            // Special "workaround" for games with only 1 bank of prg rom
            // if (prgBanks == 1 && (mapperNum == 0 || mapperNum == 3)) {
            //     prgROM->addBank(1, tmp);
            // }
            // }
            printf("\n");

            const int defaultChrBankSize = 0x2000;
            bankSize = defaultChrBankSize;

            // TODO: ask mapper what bank size it wants
            if (mapperNum == 4 || mapperNum == 33)
                bankSize = 0x400;
            else if (mapperNum == 0)
                bankSize = 0x2000;
            else
                bankSize = 0x1000;

            chrBanks = (header.chr_rom_chunks * defaultChrBankSize) / bankSize;
            chrROM = std::make_shared<BankedMemory>("CHR ROM", chrBanks, bankSize);

            printf("\tLoading %d CHR ROM banks of size 0x%04X (%d bytes)", chrBanks, bankSize, chrBanks * bankSize);
            // for (int i = 0; i < chrBanks; i++) {
            // std::vector<uint8_t> tmp;
            tmp.resize(bankSize * chrBanks);
            // ifs.read((char *)tmp.data(), bankSize);
            // chrROM->addBank(i, tmp);
            ifs.read((char *)tmp.data(), bankSize * chrBanks);
            chrROM->addData(tmp);
            // printf(".");
            // }
            printf("\n");

            prgRamSize = header.prg_ram_size ? (header.prg_ram_size * 0x2000) : 0x2000;
            prgRAM.resize(prgRamSize);
        } else if (filetype == 2) {
        }

        valid = true;

        switch (mapperNum) {
        case 0:
            mapper = std::make_shared<Mapper000>(prgBanks, chrBanks);
            break;
        case 1:
            mapper = std::make_shared<Mapper001>(prgBanks, chrBanks);
            break;
        case 2:
            mapper = std::make_shared<Mapper002>(prgBanks, chrBanks);
            break;
        case 3:
            mapper = std::make_shared<Mapper003>(prgBanks, chrBanks);
            break;
        case 4:
            mapper = std::make_shared<Mapper004>(prgBanks, chrBanks);
            break;
        case 7:
            mapper = std::make_shared<Mapper007>(prgBanks, chrBanks);
            break;
        case 11:
            mapper = std::make_shared<Mapper011>(prgBanks, chrBanks);
            break;
        case 12:
        case 13:
            mapper = std::make_shared<Mapper013>(prgBanks, chrBanks);
            break;
        case 33:
            mapper = std::make_shared<Mapper033>(prgBanks, chrBanks);
            break;
        case 66:
            mapper = std::make_shared<Mapper066>(prgBanks, chrBanks);
            break;
        default:
            printf("ERROR! Mapper %d is not implemented!\n", mapperNum);
            mapper = nullptr;
            valid = false;
            break;
        }

        if (valid) {
            mapper->setPrgROM(prgROM);
            mapper->setChrROM(chrROM);
            mapper->reset();

            if (mapper && mapper->implementationStatus() >= 0) {
                if (mapper->implementationStatus() == MI_DEVELOPMENT) {
                    printf("\tWARNING: Emulation of mapper %d is reported as IN DEVELOPMENT!\n", mapperNum);
                } else if (mapper->implementationStatus() == MI_NOT_WORKING) {
                    printf("\tWARNING: Emulation of mapper %d is reported as NOT WORKING!\n", mapperNum);
                } else if (mapper->implementationStatus() == MI_WORKING) {
                    printf("\tEmulation of mapper %d is reported as working - continuing.\n", mapperNum);
                } else {
                    printf("ERROR: Emulation of mapper %d is not implemented!\n", mapperNum);
                    exit(1);
                }
            }

            printf("\n");
            printf("\tFilename:  %s\n", filename.c_str());
            printf("\tMapper:    %d\n", mapperNum);
            printf("\tPRG ROM:   %d x 16 KB [0x%06X bytes]\n", header.prg_rom_chunks, header.prg_rom_chunks * 0x4000);
            printf("\tCHR ROM:   %d x  8 KB [0x%06X bytes]\n", header.chr_rom_chunks, header.chr_rom_chunks * 0x2000);
            printf("\tPRG RAM:   0x%04X bytes\n", prgRamSize);
            printf("\tMirroring: ");
            if (mirrorType == HORIZONTAL)
                printf("Horizontal");
            else if (mirrorType == VERTICAL)
                printf("Vertical");
            else if (mirrorType == FOUR_SCREEN)
                printf("Four Screen");
            else
                printf("????");

            printf("\n\n");
        }

        ifs.close();
    }
}

Cartridge::~Cartridge()
{
}

Cartridge::Mirror Cartridge::getMirrorType()
{
    int tmp;

    if (mapper->getMirrorType(tmp)) {
        mirrorType = (Cartridge::Mirror)tmp;
    }

    return mirrorType;
}

void Cartridge::reset()
{
    mapper->reset();
}

std::vector<std::string> Cartridge::getMapperInfo()
{
    return mapper->getInfoStrings();
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t &data)
{
    uint32_t mapped_addr = 0;
    bool prgram = false;

    if (mapper->cpuRead(addr, mapped_addr, prgram)) {
        if (prgram) {
            data = prgRAM[mapped_addr];
        } else {
            data = prgROM->read(addr);
            //printf("addr: %04X  data: %02X\n", addr, data);
        }
        return true;
    }

    return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    //printf("hello from Cartridge::cpuWrite! addr = 0x%04x  data = 0x%02x\n", addr, data);
    if (mapper->cpuWrite(addr, mapped_addr)) {
        //printf("write to PRG RAM  %04X \n", mapped_addr);
        // TODO: check mapped_addr is within bounds
        prgRAM[mapped_addr] = data;
        return true;
    } else if (mapper->cpuWriteData(addr, data)) {
        return true;
    }
    return false;
}

void Cartridge::romOverwrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    if (mapper->cpuWrite(addr, mapped_addr)) {
        mapper->romOverwrite(addr, data);
    }
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t &data)
{
    uint32_t mapped_addr = 0;
    if (mapper->ppuRead(addr, mapped_addr)) {
        data = chrROM->read(addr);
        return true;
    } else if (mapper->ppuReadData(addr, data)) {
        return true;
    } else {
        return false;
    }
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    if (mapper->ppuWrite(addr, mapped_addr)) {
        //printf("writing to CHR ROM?!?! addr = %04X  data = %02X\n", addr, data);
        //chrROM[mapped_addr] = data;
        return true;
    } else if (mapper->ppuWriteData(addr, data)) {
        return true;
    } else {
        return false;
    }
}
