
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
    std::string hexValue;

    auto hex = [](uint32_t n, uint8_t d) {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    uint8_t opcode = nes->cpuRead(addr, true);
    instruction = nes->cpu.lookup(opcode).mnemonic;
    auto addrmode = nes->cpu.lookup(opcode).addrmode;

    int bytes = nes->cpu.lookup(opcode).bytes;

    if (addrmode == &CPU::Implied) {
        instruction += " ";
        hexValue = hex(opcode, 2) + "      ";
    } else if (addrmode == &CPU::Immediate) {
        addr++;
        value = nes->cpuRead(addr, true);
        instruction += " #$" + hex(value, 2);
        hexValue = hex(opcode, 2) + " " + hex(value, 2) + "   ";
    } else if (addrmode == &CPU::ZeroPage) {
        addr++;
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrZP(lo);
        instruction += " $" + hex(lo, 2);
        extraInfo = "$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2);
        hexValue = hex(opcode, 2) + " " + hex(lo, 2) + "   ";
    } else if (addrmode == &CPU::ZeroPageX) {
        addr++;
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrZPX(lo);
        instruction += " $" + hex(lo, 2) + ", X";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
        hexValue = hex(opcode, 2) + " " + hex(lo, 2) + "   ";
    } else if (addrmode == &CPU::ZeroPageY) {
        addr++;
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrZPY(lo);
        instruction += " $" + hex(lo, 2) + ", Y";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
        hexValue = hex(opcode, 2) + " " + hex(lo, 2) + "   ";
    } else if (addrmode == &CPU::IndirectX) {
        addr++;
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrIDX(lo);
        instruction += " ($" + hex(lo, 2) + ", X)";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
        hexValue = hex(opcode, 2) + " " + hex(lo, 2) + "   ";
    } else if (addrmode == &CPU::IndirectY) {
        addr++;
        lo = nes->cpuRead(addr, true);
        effective_address = nes->cpu.GetAddrIDY(lo);
        instruction += " ($" + hex(lo, 2) + ", Y)";
        extraInfo = "$ " + hex(effective_address, 4) + " = $ " + hex(nes->cpuRead(effective_address), 2);
        hexValue = hex(opcode, 2) + " " + hex(lo, 2) + "   ";
    } else if (addrmode == &CPU::Absolute) {
        hexValue = hex(opcode, 2) + " ";
        addr++;
        lo = nes->cpuRead(addr, true);
        hexValue += hex(lo, 2) + " ";
        addr++;
        hi = nes->cpuRead(addr, true);
        hexValue += hex(hi, 2);
        instruction += " $" + hex((uint16_t)(hi << 8) | lo, 4);
    } else if (addrmode == &CPU::AbsoluteX) {
        hexValue = hex(opcode, 2) + " ";
        addr++;
        lo = nes->cpuRead(addr, true);
        hexValue += hex(lo, 2) + " ";
        addr++;
        hi = nes->cpuRead(addr, true);
        hexValue += hex(hi, 2);
        instruction += " $" + hex((uint16_t)(hi << 8) | lo, 4) + ", X";
    } else if (addrmode == &CPU::AbsoluteY) {
        hexValue = hex(opcode, 2) + " ";
        addr++;
        lo = nes->cpuRead(addr, true);
        hexValue += hex(lo, 2) + " ";
        addr++;
        hi = nes->cpuRead(addr, true);
        hexValue += hex(hi, 2);
        instruction += " $" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y";
    } else if (addrmode == &CPU::Indirect) {
        hexValue = hex(opcode, 2) + " ";
        addr++;
        lo = nes->cpuRead(addr, true);
        // hi = nes->cpuRead(addr, true);
        hexValue += hex(lo, 2) + "    ";
        addr++;
        instruction += " ($" + hex((uint16_t)(hi << 8) | lo, 4) + ")";
    } else if (addrmode == &CPU::Relative) {
        addr++;
        value = nes->cpuRead(addr, true);
        instruction += " $" + hex(value, 2);
        extraInfo = "$" + hex(addr + (int8_t)value, 4);
        hexValue = hex(opcode, 2) + " " + hex(value, 2) + "   ";
    } else {
        instruction += "ERROR: Unknown addressing mode!";
    }

    return DisassemblyLine(address, instruction, extraInfo, hexValue, bytes);
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
    int address = start;
    while (address <= end) {
        if (disassemblyData.find(address) != disassemblyData.end()) {
            result.push_back(disassemblyData[address]);
            address += disassemblyData[address].bytes;
        } else {
            disassemblyData[address] = DisassembleLine(address);
            result.push_back(disassemblyData[address]);
            address += disassemblyData[address].bytes;
        }
    }

    return result;
}
