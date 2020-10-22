#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

struct BankInfo {
    uint16_t bankNum;
    uint32_t bankSize;
    int mappable;
    struct {
        uint16_t num;
        uint32_t startAddress;
    } banks[256];
};

struct MemoryBank {
    std::vector<uint8_t> data;
    uint32_t startAddress;
    uint32_t endAddress;
    bool mapped = false;
};

class BankedMemory
{
private:
    int mappable;
    std::vector<uint8_t> data;
    // std::map<uint16_t, std::vector<uint8_t>> bankData;
    // std::map<uint16_t, std::pair<uint32_t, uint32_t>> bank;
    uint16_t bankNum;
    uint32_t bankSize;
    // std::map<uint16_t, struct MemoryBank> banks;
    // std::vector<uint16_t> mappedBanks;
    struct {
        uint32_t startAddress;
        uint32_t endAddress;
        uint32_t offset;
        uint16_t bankNum;
    } bankMap[256];
    std::string name;

public:
    BankedMemory(std::string _name, uint16_t _bankNum, uint32_t _bankSize, int mappable = 0, bool initialize = false);
    ~BankedMemory();

    // Add a bank with data
    void addBank(uint16_t n, std::vector<uint8_t> data);

    // Add all data
    void addData(std::vector<uint8_t> _data);

    // Associate a bank with a memory region
    void setBank(uint16_t startAddress, uint16_t bankNum, int override = -1, bool mirror = false);

    // Set number of mappable banks
    void setMappable(int x) { mappable = x; }

    // Read a byte
    uint8_t read(uint16_t addr);

    // Write a byte
    void write(uint16_t addr, uint8_t data);

    // operator overload to access memory?
    // uint8_t& operator[](uint16_t addr);

    struct BankInfo getBankInfo();
};
