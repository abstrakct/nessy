
#include "disassembler.h"

Disassembler::Disassembler()
{
}

DisassemblyLine Disassembler::DisassembleLine(uint16_t address)
{
    uint32_t addr = address;
    uint16_t /*line = 0,*/ effective_address = 0;
    uint8_t value = 0, lo = 0, hi = 0;
    std::string instruction;
    std::string extraInfo;

    auto hex = [](uint32_t n, uint8_t d) {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    // line = addr;
    // std::string inst = "$" + hex(addr, 4) + ": ";
    // addr++;
    // inst += nes->cpu.lookup(opcode).mnemonic + " ";

    uint8_t opcode = nes->cpuRead(addr, true);
    instruction = nes->cpu.lookup(opcode).mnemonic;
    auto addrmode = nes->cpu.lookup(opcode).addrmode;

    if (addrmode == &CPU::Implied) {
        instruction += " ";
    } else if (addrmode == &CPU::Immediate) {
        value = nes->cpuRead(addr, true);
        addr++;
        instruction += " #$" + hex(value, 2);
    } else if (addrmode == &CPU::ZeroPage) {
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrZP(lo);
        addr++;
        instruction += " $" + hex(lo, 2);
        extraInfo = "$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2);
    } else if (addrmode == &CPU::ZeroPageX) {
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrZPX(lo);
        addr++;
        instruction += " $" + hex(lo, 2) + ", X";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
    } else if (addrmode == &CPU::ZeroPageY) {
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrZPY(lo);
        addr++;
        instruction += " $" + hex(lo, 2) + ", Y";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
    } else if (addrmode == &CPU::IndirectX) {
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrIDX(lo);
        addr++;
        instruction += " ($" + hex(lo, 2) + ", X)";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
    } else if (addrmode == &CPU::IndirectY) {
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrIDY(lo);
        addr++;
        instruction += " ($" + hex(lo, 2) + ", Y)";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
    } else if (addrmode == &CPU::Absolute) {
        lo = nes->cpuRead(addr, true);
        addr++;
        hi = nes->cpuRead(addr, true);
        addr++;
        instruction += " $" + hex((uint16_t)(hi << 8) | lo, 4);
    } else if (addrmode == &CPU::AbsoluteX) {
        lo = nes->cpuRead(addr, true);
        addr++;
        hi = nes->cpuRead(addr, true);
        addr++;
        instruction += " $" + hex((uint16_t)(hi << 8) | lo, 4) + ", X";
    } else if (addrmode == &CPU::AbsoluteY) {
        lo = nes->cpuRead(addr, true);
        addr++;
        hi = nes->cpuRead(addr, true);
        addr++;
        instruction += " $" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y";
    } else if (addrmode == &CPU::Indirect) {
        lo = nes->cpuRead(addr, true);
        addr++;
        hi = nes->cpuRead(addr, true);
        addr++;
        instruction += " ($" + hex((uint16_t)(hi << 8) | lo, 4) + ")";
    } else if (addrmode == &CPU::Relative) {
        value = nes->cpuRead(addr, true);
        addr++;
        instruction += " $" + hex(value, 2);
        extraInfo = "$" + hex(addr + (int8_t)value, 4);
    } else {
        instruction += "ERROR: Unknown addressing mode!";
    }

    return DisassemblyLine(address, instruction, extraInfo);
}

void Disassembler::disassemble(uint16_t address)
{
    disassemblyData[address] = DisassembleLine(address);
}

void Disassembler::disassemble(uint16_t start, uint16_t end)
{
    for (int i = start; i <= end; i++) {
        disassemblyData[i] = DisassembleLine(i);
    }
}

DisassemblyLine &Disassembler::get(uint16_t address)
{
    if (disassemblyData.find(address) != disassemblyData.end()) {
        return disassemblyData[address];
    } else {
        disassemblyData[address] = DisassembleLine(address);
        return disassemblyData[address];
    }
}

std::vector<DisassemblyLine> Disassembler::get(uint16_t start, uint16_t end)
{
    std::vector<DisassemblyLine> result;
    for (int address = start; address <= end; address++) {
        if (disassemblyData.find(address) != disassemblyData.end()) {
            result.push_back(disassemblyData[address]);
        } else {
            disassemblyData[address] = DisassembleLine(address);
            result.push_back(disassemblyData[address]);
        }
    }

    return result;
}
