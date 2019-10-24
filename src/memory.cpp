#include "memory.h"

BankedMemory::BankedMemory(uint16_t _bankNum, uint32_t _bankSize)
{
    bankNum = _bankNum;
    bankSize = _bankSize;

    // Initialize with sane defaults
    bank[0] = { 0x8000, 0xBFFF };
    bank[1] = { 0xC000, 0xFFFF };
}

BankedMemory::~BankedMemory()
{
}

void BankedMemory::addBank(uint16_t n, std::vector<uint8_t> _data)
{
    if (_data.size() != bankSize) {
        printf("ERROR: wrong bank size!\n");
        exit(1);
    }
    
    bankData[n] = _data;
}

void BankedMemory::setBank(uint16_t startAddress, uint16_t bankNum, bool mirror)
{
    if (!mirror) {
        for (auto &it : bank) {
            if (it.second.first == startAddress && it.second.second == (startAddress + bankSize - 1)) {
                it.second = { 0xFFFFFFFF, 0xFFFFFFFF };
            }
        }
    }
    bank[bankNum] = { startAddress, startAddress + bankSize - 1 };
}

// TODO: optimize?!
uint8_t BankedMemory::read(uint16_t addr)
{
    uint8_t ret = 0;

    for (auto it : bank) {
        if (addr >= it.second.first && addr <= it.second.second) {
            addr -= it.second.first;
            ret = bankData[it.first][addr];
        }
    }

    return ret;
}

void BankedMemory::write(uint16_t addr, uint8_t data)
{
    for (auto it : bank) {
        if (addr >= it.second.first && addr <= it.second.second) {
            addr -= it.second.first;
            bankData[it.first][addr] = data;
        }
    }
}

//uint8_t& operator[](std::size_t addr)
//{
//    for (auto it : bank) {
//        if (addr >= it.second.first && addr < it.second.second) {
//            addr -= it.second.first;
//            return bankData[it.first][addr];
//        }
//    }
//}
