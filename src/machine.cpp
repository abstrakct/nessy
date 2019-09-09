
#include "machine.h"

void Machine::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr < 0x2000) {
        cpuRam[addr & 0x07FFF] = data;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        ppu.cpuWrite(addr, data);
    } else if (addr == 0x4014) {
        ppu.oamdma = data;
    }
}

uint8_t Machine::cpuRead(uint16_t addr, bool readonly)
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

Machine::Machine()
{
    cpu.ConnectMachine(this);
    ppu.ConnectMachine(this);

    for (auto &i : cpuRam) {
        i = 0x00;
    }
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
