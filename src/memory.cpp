#include "memory.h"

#include <algorithm>
#include <string>
#include <vector>

BankedMemory::BankedMemory(std::string _name, uint16_t _bankNum, uint32_t _bankSize, int _mappable, bool initialize)
{
    name = _name;
    bankNum = _bankNum;
    bankSize = _bankSize;
    mappable = _mappable;

    if (initialize) {
        for (int i = 0; i < bankNum; i++) {
            // bankData[i].resize(bankSize);
        }
    }
}

BankedMemory::~BankedMemory()
{
}

void BankedMemory::addData(std::vector<uint8_t> _data)
{
    data = _data;
}

void BankedMemory::addBank(uint16_t n, std::vector<uint8_t> _data)
{
    if (_data.size() != bankSize) {
        printf("ERROR: wrong bank size!\n");
        exit(1);
    }

    // bankData[n] = _data;
    // banks[n].data = _data;
}

void BankedMemory::setBank(uint16_t startAddress, uint16_t newBank, int override, bool mirror)
{
    int mappedBank = 0;
    if (override >= 0)
        mappedBank = override;
    else if (mappable > 1)
        mappedBank = (startAddress / bankSize);
    bankMap[mappedBank].startAddress = startAddress;
    bankMap[mappedBank].endAddress = startAddress + bankSize - 1;
    bankMap[mappedBank].offset = newBank * bankSize;
    bankMap[mappedBank].bankNum = newBank;

    // printf("%s: Mapped bank %02X [%d] to addresses %04X - %04X (offset = %06X)\n", name.c_str(), newBank, mappedBank, bankMap[mappedBank].startAddress, bankMap[mappedBank].endAddress, bankMap[mappedBank].offset);
}

// TODO: optimize?!
uint8_t BankedMemory::read(uint16_t addr)
{
    for (int i = 0; i < mappable; i++) {
        if (addr >= bankMap[i].startAddress && addr <= bankMap[i].endAddress) {
            return data[bankMap[i].offset + (addr - bankMap[i].startAddress)];
        }
    }
    return 0;
}

void BankedMemory::write(uint16_t addr, uint8_t data)
{
    printf("writing %02X to ROM address %04X\n", data, addr);
    for (int i = 0; i < mappable; i++) {
        if (addr >= bankMap[i].startAddress && addr <= bankMap[i].endAddress) {
            this->data[bankMap[i].offset + (addr - bankMap[i].startAddress)] = data;
        }
    }
    // for (auto it : bank) {
    //     if (addr >= it.second.first && addr <= it.second.second) {
    //         addr -= it.second.first;
    //         bankData[it.first][addr] = data;
    //     }
    // }
}

struct BankInfo BankedMemory::getBankInfo()
{
    struct BankInfo ret;
    ret.bankNum = bankNum;
    ret.bankSize = bankSize;
    ret.mappable = mappable;
    for (int i = 0; i < mappable; i++) {
        ret.banks[i].startAddress = bankMap[i].startAddress;
        ret.banks[i].num = bankMap[i].bankNum;
    }
    return ret;
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
