
#include "machine.h"

Bus::Bus()
{
    cpu.ConnectBus(this);
    ppu.ConnectBus(this);

    uint8_t v = 0x00;
    for (auto &i : cpuRam) {
        i = v;
        //v++;
    }
}

Bus::~Bus()
{
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr < 0x2000) {
        cpuRam[addr & 0x07FFF] = data;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        ppu.cpuWrite(addr, data);
    } else if (addr == 0x4014) {
        ppu.oamdma = data;
    }
}

uint8_t Bus::cpuRead(uint16_t addr, bool readonly)
{
    uint8_t data;

    if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = cpuRam[addr & 0x07FFF];
    } else if (addr >= 0x2000 && addr < 0x4000) {
        data = ppu.cpuRead(addr, readonly);
    } else {
        data = 0x00;
    }

    return data;
}

//void Machine::load_rom(std::vector<uint8_t> data, uint16_t offset, uint16_t length)
//{
//    for (int i = 0; i < length; i++) {
//        bus.write(offset + i, data[i]);
//    }
//    //for (auto i : data) {
//    //    printf("writing %02x to %04x\n", i, addr);
//    //    if (addr > (offset + length)) {
//    //        break;
//    //    } else {
//    //        bus.write(addr, i);
//    //        addr++;
//    //    }
//    //}
//}

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
