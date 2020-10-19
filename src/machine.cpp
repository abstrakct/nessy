
#include "machine.h"

void Machine::cpuWrite(uint16_t addr, uint8_t data)
{
    if (cart->cpuWrite(addr, data)) {
        // cartridge handles this write
    } else if (addr < 0x2000) {
        cpuRam[addr & 0x07FF] = data;
    } else if (addr >= 0x2000 && addr <= 0x4000) {
        ppu.cpuWrite(addr, data);
    } else if (addr == 0x4014) {
        dma_page = data;
        dma_addr = ppu.oamAddr;
        dma_count = 0;
        dma_transfer = true;
    } else if (addr == 0x4016 || addr == 0x4017) {
        controller[addr & 0x0001].write(data);
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
    } else if (addr == 0x4016 || addr == 0x4017) {
        data = controller[addr & 0x0001].read();
    } else {
        data = 0x00;
    }

    return data;
}

uint8_t Machine::editorCpuRead(const uint8_t *data, size_t address)
{
    return this->cpuRead(address, true);
}

void Machine::editorCpuWrite(const uint8_t *data, size_t address, uint8_t input)
{
    if (address < 0x2000) {
        cpuRam[address & 0x07FF] = input;
    } else if (address >= 0x2000 && address < 0x4000) {
        ppu.cpuWrite(address, input);
    } else if (address >= 0x8000) {
        // Overwrite data in ROM!
        cart->romOverwrite(address, input);
    }
}

void Machine::reset()
{
    // TODO: reset mapper!!
    cpu.reset();
    ppu.reset();
    cart->reset();
    systemClockCounter = 0;
}

void Machine::clock()
{
    ppu.clock();

    // DMA stuff taken from OLC
    if (systemClockCounter % 3 == 0) {
        if (dma_transfer) {
            if (dma_dummy) {
                if (systemClockCounter % 2 == 1) {
                    dma_dummy = false;
                }
            } else {
                if (systemClockCounter % 2 == 0) {
                    dma_data = cpuRead(((uint16_t)dma_page << 8) | dma_addr);
                } else {
                    ppu.oamWrite(dma_addr, dma_data);
                    dma_addr++;
                    dma_count++;
                    if (dma_count == 0x00) {
                        dma_transfer = false;
                        dma_dummy = true;
                    }
                }
            }
        } else {
            cpu.clock();
        }
    }

    if (ppu.nmiOccurred) {
        ppu.nmiOccurred = false;
        cpu.nmi();
    }

    // Has mapper/cartridge requested IRQ?
    // if (cart->getMapper()->irqState()) {
    //     cart->getMapper()->irqClear();
    //     cpu.irq();
    // }

    systemClockCounter++;
}

void Machine::insertCartridge(const std::shared_ptr<Cartridge> &cartridge)
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
