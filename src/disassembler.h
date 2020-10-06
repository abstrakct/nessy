#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "machine.h"

class DisassemblyLine
{
private:
    uint16_t address;
    std::string instruction;
    std::string extraInfo;

public:
    DisassemblyLine(){};
    DisassemblyLine(uint16_t _address, std::string _instruction, std::string _extraInfo) : address(_address), instruction(_instruction), extraInfo(_extraInfo) {}
};

class Disassembler
{
private:
    std::shared_ptr<Machine> nes = nullptr;
    // std::vector<DisassemblyLine> lines;
    std::map<uint16_t, DisassemblyLine> disassemblyData;
    DisassemblyLine DisassembleLine(uint16_t address);

public:
    Disassembler();
    ~Disassembler(){};

    void ConnectMachine(std::shared_ptr<Machine> n) { nes = n; }
    void disassemble(uint16_t address);
    void disassemble(uint16_t start, uint16_t end);
    DisassemblyLine &get(uint16_t address);
    std::vector<DisassemblyLine> get(uint16_t start, uint16_t end);
};