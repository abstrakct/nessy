
#include <fstream>
#include "logger.h"
#include "cartridge.h"
#include "mappers/mapper000.h"
#include "mappers/mapper001.h"

extern Logger l;

Cartridge::Cartridge(const std::string& filename)
{
    struct header_struct {
        char name[4];
        uint8_t prg_rom_chunks;
        uint8_t chr_rom_chunks;
        uint8_t mapper1, mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1, tv_system2;
        char unused[5];   // there can be information here, implement/use later
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
        ifs.read((char*)&header, sizeof(header_struct));

        if (header.mapper1 & 0x04)   // then 512 bytes of trainer data
            ifs.seekg(512, std::ios_base::cur);

        mapperNum = (header.mapper1 >> 4) | ((header.mapper2 >> 4) << 4);

        mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

        // discover file format
        uint8_t filetype = 1;
        if (filetype == 0) {
        } else if (filetype == 1) {
            prgBanks = header.prg_rom_chunks;
            prgMem.resize(prgBanks * 16384);
            ifs.read((char*)prgMem.data(), prgMem.size());

            chrBanks = header.chr_rom_chunks;
            chrMem.resize(chrBanks * 8192);
            ifs.read((char*)chrMem.data(), chrMem.size());
        } else if (filetype == 2) {
        }

        valid = true;

        switch (mapperNum) {
            case 0:
                mapper = std::make_shared<Mapper000>(prgBanks, chrBanks); break;
            case 1:
                mapper = std::make_shared<Mapper001>(prgBanks, chrBanks); break;
            default:
                printf("WARNING! Unknown mapper %d\n", mapperNum);
                mapper = nullptr; 
                valid = false;
                break;
        }

        if (mapper && mapper->implementationStatus()) {
            printf("Error: Emulation of mapper %d is %s\n", mapperNum, mapper->implementationStatus() == 1 ? "not working" : "not usable");
            exit(1);
        }

        ifs.close();

        if (valid) {
            printf("\n");
            printf("Filename:  %s\n", filename.c_str());
            printf("Mapper:    %d\n", mapperNum);
            printf("PRG ROM:   %d x 16 KB\n", prgBanks);
            printf("CHR ROM:   %d x  8 KB\n", chrBanks);
            printf("Mirroring: %s\n\n", mirror == VERTICAL ? "Vertical" : "Horizontal");
        }
    }
}

Cartridge::~Cartridge()
{
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t &data)
{
    uint32_t mapped_addr = 0;
    if (mapper->cpuRead(addr, mapped_addr)) {
        data = prgMem[mapped_addr];
        return true;
    } else
        return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    if (mapper->cpuWrite(addr, mapped_addr)) {
        prgMem[mapped_addr] = data;
        return true;
    } else
        return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t &data)
{
    uint32_t mapped_addr = 0;
    if (mapper->ppuRead(addr, mapped_addr)) {
        data = chrMem[mapped_addr];
        return true;
    } else
        return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    if (mapper->ppuWrite(addr, mapped_addr)) {
        chrMem[mapped_addr] = data;
        return true;
    } else
        return false;
}

