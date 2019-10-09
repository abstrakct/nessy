
#include "machine.h"

void Machine::cpuWrite(uint16_t addr, uint8_t data)
{
    if (cart->cpuWrite(addr, data)) {
        // cartridge handles this write
    } else if (addr < 0x2000) {
        cpuRam[addr & 0x07FF] = data;
    } else if (addr >= 0x2000 && addr <= 0x4014) {
        ppu.cpuWrite(addr, data);
    } else if (addr == 0x4016) {
        controller1.write(data & 0x01);
    }
}

uint8_t Machine::cpuRead(uint16_t addr, bool readonly)
{
    uint8_t data;

    if (cart->cpuRead(addr, data)) {
        // cartridge handles this read
    } else if (addr < 0x2000) {
        data = cpuRam[addr & 0x07FF];
    } else if (addr >= 0x2000 && addr < 0x4000) {
        data = ppu.cpuRead(addr, readonly);
    } else if (addr == 0x4016) {
        data = controller1.read();
    } else {
        data = 0x00;
    }

    return data;
}

void Machine::reset()
{
    // TODO: reset mapper!!
    cpu.reset();
    ppu.reset();
    systemClockCounter = 0;
}

void Machine::clock()
{
    ppu.clock();

    if (systemClockCounter % 3 == 0) {
        cpu.clock();
    }

    if (ppu.nmiOccurred) {
        ppu.nmiOccurred = false;
        cpu.nmi();
    }

    systemClockCounter++;
}

void Machine::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
    ppu.connectCartridge(cartridge);
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
