
#include "machine.h"

Bus::Bus()
{
    cpu.ConnectBus(this);
    ppu.ConnectBus(this);

    uint8_t v = 0x00;
    for (auto &i : ram) {
        i = v;
        //v++;
    }
}

Bus::~Bus()
{
}

void Bus::write(uint16_t addr, uint8_t data)
{
    //if (addr >= 0x0000 && addr <= 0xFFFF) {
        //printf("writing %02x to %04x\n", data, addr);
        
        if (addr >= 0x2000 && addr < 0x4000) {
            ppu.reg[addr % 8] = data;
            //if ((addr % 8) == 0)
            //    ppu.ppuctrl = data;
            //else if ((addr % 8) == 1)
            //    ppu.ppumask = data;
            //else if ((addr % 8) == 2)
            //    ppu.ppustatus = data;
            //else if ((addr % 8) == 3)
            //    ppu.oamaddr = data;
            //else if ((addr % 8) == 4)
            //    ppu.oamdata = data;
            //else if ((addr % 8) == 5)
            //    ppu.ppuscroll = data;
            //else if ((addr % 8) == 6)
            //    ppu.ppuaddr = data;
            //else if ((addr % 8) == 7)
            //    ppu.ppudata = data;
        } else if (addr == 0x4014) {
            ppu.oamdma = data;
        } else {
            ram[addr] = data;
        }
    //}
}

uint8_t Bus::read(uint16_t addr, bool readonly)
{
    //if (addr >= 0x0000 && addr <= 0xFFFF) {
        if  (addr >= 0x2000 && addr < 0x4000) {
            return ppu.reg[addr % 8];

            //if ((addr % 8) == 0)
            //    return ppu.ppuctrl;
            //else if ((addr % 8) == 1)
            //    return ppu.ppumask;
            //else if ((addr % 8) == 2)
            //    return ppu.ppustatus;
            //else if ((addr % 8) == 1)
            //    return ppu.oamaddr;
            //else if ((addr % 8) == 4)
            //    return ppu.oamdata;
            //else if ((addr % 8) == 5)
            //    return ppu.ppuscroll;
            //else if ((addr % 8) == 6)
            //    return ppu.ppuaddr;
            //else if ((addr % 8) == 7)
            //    return ppu.ppudata;
        } else {
            return ram[addr];
        }
    //}

    return 0x00;
}

// TODO: error checking
void Machine::load_rom(std::vector<uint8_t> data, uint16_t offset, uint16_t length)
{
    for (int i = 0; i < length; i++) {
        bus.write(offset + i, data[i]);
    }
    //for (auto i : data) {
    //    printf("writing %02x to %04x\n", i, addr);
    //    if (addr > (offset + length)) {
    //        break;
    //    } else {
    //        bus.write(addr, i);
    //        addr++;
    //    }
    //}
}

void Machine::init()
{
    cpu = &bus.cpu;
    ppu = &bus.ppu;
}

Machine::Machine()
{
}

Machine::~Machine()
{
}

//Machine::Machine(int romsize, std::vector<uint8_t> romdata)
//{
//    for (auto i = 0; i < romsize; i++) {
//        mem.write(0x8000 + i, romdata[i]);
//    }
//}
