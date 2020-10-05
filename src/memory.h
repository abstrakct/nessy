#pragma once

#include <map>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

class BankedMemory
{
private:
    std::map<uint16_t, std::vector<uint8_t>> bankData;
    std::map<uint16_t, std::pair<uint32_t, uint32_t>> bank;
    uint16_t bankNum;
    uint32_t bankSize;

public:
    BankedMemory(uint16_t _bankNum, uint32_t _bankSize, bool initialize = false);
    ~BankedMemory();

    // Add a bank with data
    void addBank(uint16_t n, std::vector<uint8_t> data);

    // Associate a bank with a memory region
    void setBank(uint16_t startAddress, uint16_t bankNum, bool mirror = false);

    // Change bank size

    // Read a byte
    uint8_t read(uint16_t addr);

    // Write a byte
    void write(uint16_t addr, uint8_t data);

    // operator overload to access memory?
    // uint8_t& operator[](uint16_t addr);
};
