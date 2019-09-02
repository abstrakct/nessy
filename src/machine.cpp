
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
    if (addr >= 0x0000 && addr <= 0xFFFF) {
        //printf("writing %02x to %04x\n", data, addr);
        ram[addr] = data;
    }
}

uint8_t Bus::read(uint16_t addr, bool readonly)
{
    if (addr >= 0x0000 && addr <= 0xFFFF) {
        return ram[addr];
    }

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
