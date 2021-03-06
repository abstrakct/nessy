/*
 * 6502 cpu stuff
 */

#include <stdio.h>

#include "cpu.h"
#include "logger.h"
#include "machine.h"

extern Logger l;

CPU::CPU()
{
    using c = CPU;
    lookupTable = {
        // CHECKED and OK!
        // clang-format off
        { 0x00, "BRK", &c::Implied  , 1, 7, },
        { 0x01, "ORA", &c::IndirectX, 2, 6, },
        { 0x02, "XXX", &c::Implied  , 1, 2, },
        { 0x03, "XXX", &c::Implied  , 1, 2, },
        { 0x04, "XXX", &c::Implied  , 1, 2, },
        { 0x05, "ORA", &c::ZeroPage , 2, 3, },
        { 0x06, "ASL", &c::ZeroPage , 2, 5, },
        { 0x07, "XXX", &c::Implied  , 1, 2, },
        { 0x08, "PHP", &c::Implied  , 1, 3, },
        { 0x09, "ORA", &c::Immediate, 2, 2, },
        { 0x0A, "ASL", &c::Implied,   1, 2, },
        { 0x0B, "XXX", &c::Implied  , 1, 2, },
        { 0x0C, "XXX", &c::Implied  , 1, 2, },
        { 0x0D, "ORA", &c::Absolute , 3, 4, },
        { 0x0E, "ASL", &c::Absolute , 3, 6, },
        { 0x0F, "XXX", &c::Implied  , 1, 2, },
        { 0x10, "BPL", &c::Relative , 2, 2, },
        { 0x11, "ORA", &c::IndirectY, 2, 5, },
        { 0x12, "XXX", &c::Implied  , 1, 2, },
        { 0x13, "XXX", &c::Implied  , 1, 2, },
        { 0x14, "XXX", &c::Implied  , 1, 2, },
        { 0x15, "ORA", &c::ZeroPageX, 2, 4, },
        { 0x16, "ASL", &c::ZeroPageX, 2, 6, },
        { 0x17, "XXX", &c::Implied  , 1, 2, },
        { 0x18, "CLC", &c::Implied  , 1, 2, },
        { 0x19, "ORA", &c::AbsoluteY, 3, 4, },
        { 0x1A, "XXX", &c::Implied  , 1, 2, },
        { 0x1B, "XXX", &c::Implied  , 1, 2, },
        { 0x1C, "XXX", &c::Implied  , 1, 2, },
        { 0x1D, "ORA", &c::AbsoluteX, 3, 4, },
        { 0x1E, "ASL", &c::AbsoluteX, 3, 7, },
        { 0x1F, "XXX", &c::Implied  , 1, 2, },
        { 0x20, "JSR", &c::Absolute , 3, 6, },
        { 0x21, "AND", &c::IndirectX, 2, 6, },
        { 0x22, "XXX", &c::Implied  , 1, 2, },
        { 0x23, "XXX", &c::Implied  , 1, 2, },
        { 0x24, "BIT", &c::ZeroPage , 2, 3, },
        { 0x25, "AND", &c::ZeroPage , 2, 3, },
        { 0x26, "ROL", &c::ZeroPage , 2, 5, },
        { 0x27, "XXX", &c::Implied  , 1, 2, },
        { 0x28, "PLP", &c::Implied  , 1, 4, },
        { 0x29, "AND", &c::Immediate, 2, 2, },
        { 0x2A, "ROL", &c::Implied,   1, 2, },
        { 0x2B, "XXX", &c::Implied  , 1, 2, },
        { 0x2C, "BIT", &c::Absolute , 3, 4, },
        { 0x2D, "AND", &c::Absolute , 3, 4, },
        { 0x2E, "ROL", &c::Absolute , 3, 6, },
        { 0x2F, "XXX", &c::Implied  , 1, 2, },
        { 0x30, "BMI", &c::Relative , 2, 2, },
        { 0x31, "AND", &c::IndirectY, 2, 5, },
        { 0x32, "XXX", &c::Implied  , 1, 2, },
        { 0x33, "XXX", &c::Implied  , 1, 2, },
        { 0x34, "XXX", &c::Implied  , 1, 2, },
        { 0x35, "AND", &c::ZeroPageX, 2, 4, },
        { 0x36, "ROL", &c::ZeroPageX, 2, 6, },
        { 0x37, "XXX", &c::Implied  , 1, 2, },
        { 0x38, "SEC", &c::Implied  , 1, 2, },
        { 0x39, "AND", &c::AbsoluteY, 3, 4, },
        { 0x3A, "XXX", &c::Implied  , 1, 2, },
        { 0x3B, "XXX", &c::Implied  , 1, 2, },
        { 0x3C, "XXX", &c::Implied  , 1, 2, },
        { 0x3D, "AND", &c::AbsoluteX, 3, 4, },
        { 0x3E, "ROL", &c::AbsoluteX, 3, 7, },
        { 0x3F, "XXX", &c::Implied  , 1, 2, },
        { 0x40, "RTI", &c::Implied  , 1, 6, },
        { 0x41, "EOR", &c::IndirectX, 2, 6, },
        { 0x42, "XXX", &c::Implied  , 1, 2, },
        { 0x43, "XXX", &c::Implied  , 1, 2, },
        { 0x44, "XXX", &c::Implied  , 1, 2, },
        { 0x45, "EOR", &c::ZeroPage , 2, 3, },
        { 0x46, "LSR", &c::ZeroPage , 2, 5, },
        { 0x47, "XXX", &c::Implied  , 1, 2, },
        { 0x48, "PHA", &c::Implied  , 1, 3, },
        { 0x49, "EOR", &c::Immediate, 2, 2, },
        { 0x4A, "LSR", &c::Implied,   1, 2, },
        { 0x4B, "XXX", &c::Implied  , 1, 2, },
        { 0x4C, "JMP", &c::Absolute , 3, 3, },
        { 0x4D, "EOR", &c::Absolute , 3, 4, },
        { 0x4E, "LSR", &c::Absolute , 3, 6, },
        { 0x4F, "XXX", &c::Implied  , 1, 2, },
        { 0x50, "BVC", &c::Relative , 2, 2, },
        { 0x51, "EOR", &c::IndirectY, 2, 5, },
        { 0x52, "XXX", &c::Implied  , 1, 2, },
        { 0x53, "XXX", &c::Implied  , 1, 2, },
        { 0x54, "XXX", &c::Implied  , 1, 2, },
        { 0x55, "EOR", &c::ZeroPageX, 2, 4, },
        { 0x56, "LSR", &c::ZeroPageX, 2, 6, },
        { 0x57, "XXX", &c::Implied  , 1, 2, },
        { 0x58, "CLI", &c::Implied  , 1, 2, },
        { 0x59, "EOR", &c::AbsoluteY, 3, 4, },
        { 0x5A, "XXX", &c::Implied  , 1, 2, },
        { 0x5B, "XXX", &c::Implied  , 1, 2, },
        { 0x5C, "XXX", &c::Implied  , 1, 2, },
        { 0x5D, "EOR", &c::AbsoluteX, 3, 4, },
        { 0x5E, "LSR", &c::AbsoluteX, 3, 7, },
        { 0x5F, "XXX", &c::Implied  , 1, 2, },
        { 0x60, "RTS", &c::Implied  , 1, 6, },
        { 0x61, "ADC", &c::IndirectX, 2, 6, },
        { 0x62, "XXX", &c::Implied  , 1, 2, },
        { 0x63, "XXX", &c::Implied  , 1, 2, },
        { 0x64, "XXX", &c::Implied  , 1, 2, },
        { 0x65, "ADC", &c::ZeroPage,  2, 3, },
        { 0x66, "ROR", &c::ZeroPage , 2, 5, },
        { 0x67, "XXX", &c::Implied  , 1, 2, },
        { 0x68, "PLA", &c::Implied  , 1, 4, },
        { 0x69, "ADC", &c::Immediate, 2, 2, },
        { 0x6A, "ROR", &c::Implied,   1, 2, },
        { 0x6B, "XXX", &c::Implied  , 1, 2, },
        { 0x6C, "JMP", &c::Indirect , 3, 5, },
        { 0x6D, "ADC", &c::Absolute , 3, 4, },
        { 0x6E, "ROR", &c::Absolute , 3, 6, },
        { 0x6F, "XXX", &c::Implied  , 1, 2, },
        { 0x70, "BVS", &c::Relative , 2, 2, },
        { 0x71, "ADC", &c::IndirectY, 2, 5, },
        { 0x72, "XXX", &c::Implied  , 1, 2, },
        { 0x73, "XXX", &c::Implied  , 1, 2, },
        { 0x74, "XXX", &c::Implied  , 1, 2, },
        { 0x75, "ADC", &c::ZeroPageX, 2, 4, },
        { 0x76, "ROR", &c::ZeroPageX, 2, 6, },
        { 0x77, "XXX", &c::Implied  , 1, 2, },
        { 0x78, "SEI", &c::Implied  , 1, 2, },
        { 0x79, "ADC", &c::AbsoluteY, 3, 4, },
        { 0x7A, "XXX", &c::Implied  , 1, 2, },
        { 0x7B, "XXX", &c::Implied  , 1, 2, },
        { 0x7C, "XXX", &c::Implied  , 1, 2, },
        { 0x7D, "ADC", &c::AbsoluteX, 3, 4, },
        { 0x7E, "ROR", &c::AbsoluteX, 3, 7, },
        { 0x7F, "XXX", &c::Implied  , 1, 2, },
        { 0x80, "XXX", &c::Implied  , 1, 2, },
        { 0x81, "STA", &c::IndirectX, 2, 6, },
        { 0x82, "XXX", &c::Implied  , 1, 2, },
        { 0x83, "XXX", &c::Implied  , 1, 2, },
        { 0x84, "STY", &c::ZeroPage , 2, 3, },
        { 0x85, "STA", &c::ZeroPage , 2, 3, },
        { 0x86, "STX", &c::ZeroPage , 2, 3, },
        { 0x87, "XXX", &c::Implied  , 1, 2, },
        { 0x88, "DEY", &c::Implied  , 1, 2, },
        { 0x89, "XXX", &c::Implied  , 1, 2, },
        { 0x8A, "TXA", &c::Implied  , 1, 2, },
        { 0x8B, "XXX", &c::Implied  , 1, 2, },
        { 0x8C, "STY", &c::Absolute , 3, 4, },
        { 0x8D, "STA", &c::Absolute , 3, 4, },
        { 0x8E, "STX", &c::Absolute , 3, 4, },
        { 0x8F, "XXX", &c::Implied  , 1, 2, },
        { 0x90, "BCC", &c::Relative , 2, 2, },
        { 0x91, "STA", &c::IndirectY, 2, 6, },
        { 0x92, "XXX", &c::Implied  , 1, 2, },
        { 0x93, "XXX", &c::Implied  , 1, 2, },
        { 0x94, "STY", &c::ZeroPageX, 2, 4, },
        { 0x95, "STA", &c::ZeroPageX, 2, 4, },
        { 0x96, "STX", &c::ZeroPageY, 2, 4, },
        { 0x97, "XXX", &c::Implied  , 1, 2, },
        { 0x98, "TYA", &c::Implied  , 1, 2, },
        { 0x99, "STA", &c::AbsoluteY, 3, 5, },
        { 0x9A, "TXS", &c::Implied  , 1, 2, },
        { 0x9B, "XXX", &c::Implied  , 1, 2, },
        { 0x9C, "XXX", &c::Implied  , 1, 2, },
        { 0x9D, "STA", &c::AbsoluteX, 3, 5, },
        { 0x9E, "XXX", &c::Implied  , 1, 2, },
        { 0x9F, "XXX", &c::Implied  , 1, 2, },
        { 0xA0, "LDY", &c::Immediate, 2, 2, },
        { 0xA1, "LDA", &c::IndirectX, 2, 6, },
        { 0xA2, "LDX", &c::Immediate, 2, 2, },
        { 0xA3, "XXX", &c::Implied  , 1, 2, },
        { 0xA4, "LDY", &c::ZeroPage , 2, 3, },
        { 0xA5, "LDA", &c::ZeroPage , 2, 3, },
        { 0xA6, "LDX", &c::ZeroPage , 2, 3, },
        { 0xA7, "XXX", &c::Implied  , 1, 2, },
        { 0xA8, "TAY", &c::Implied  , 1, 2, },
        { 0xA9, "LDA", &c::Immediate, 2, 2, },
        { 0xAA, "TAX", &c::Implied  , 1, 2, },
        { 0xAB, "XXX", &c::Implied  , 1, 2, },
        { 0xAC, "LDY", &c::Absolute , 3, 4, },
        { 0xAD, "LDA", &c::Absolute , 3, 4, },
        { 0xAE, "LDX", &c::Absolute , 3, 4, },
        { 0xAF, "XXX", &c::Implied  , 1, 2, },
        { 0xB0, "BCS", &c::Relative , 2, 2, },
        { 0xB1, "LDA", &c::IndirectY, 2, 5, },
        { 0xB2, "XXX", &c::Implied  , 1, 2, },
        { 0xB3, "XXX", &c::Implied  , 1, 2, },
        { 0xB4, "LDY", &c::ZeroPageX, 2, 4, },
        { 0xB5, "LDA", &c::ZeroPageX, 2, 4, },
        { 0xB6, "LDX", &c::ZeroPageY, 2, 4, },
        { 0xB7, "XXX", &c::Implied  , 1, 2, },
        { 0xB8, "CLV", &c::Implied  , 1, 2, },
        { 0xB9, "LDA", &c::AbsoluteY, 3, 4, },
        { 0xBA, "TSX", &c::Implied  , 1, 2, },
        { 0xBB, "XXX", &c::Implied  , 1, 2, },
        { 0xBC, "LDY", &c::AbsoluteX, 3, 4, },
        { 0xBD, "LDA", &c::AbsoluteX, 3, 4, },
        { 0xBE, "LDX", &c::AbsoluteY, 3, 4, },
        { 0xBF, "XXX", &c::Implied  , 1, 2, },
        { 0xC0, "CPY", &c::Immediate, 2, 2, },
        { 0xC1, "CMP", &c::IndirectX, 2, 6, },
        { 0xC2, "XXX", &c::Implied  , 1, 2, },
        { 0xC3, "XXX", &c::Implied  , 1, 2, },
        { 0xC4, "CPY", &c::ZeroPage , 2, 3, },
        { 0xC5, "CMP", &c::ZeroPage , 2, 3, },
        { 0xC6, "DEC", &c::ZeroPage , 2, 5, },
        { 0xC7, "XXX", &c::Implied  , 1, 2, },
        { 0xC8, "INY", &c::Implied  , 1, 2, },
        { 0xC9, "CMP", &c::Immediate, 2, 2, },
        { 0xCA, "DEX", &c::Implied  , 1, 2, },
        { 0xCB, "XXX", &c::Implied  , 1, 2, },
        { 0xCC, "CPY", &c::Absolute , 3, 4, },
        { 0xCD, "CMP", &c::Absolute , 3, 4, },
        { 0xCE, "DEC", &c::Absolute , 3, 6, },
        { 0xCF, "XXX", &c::Implied  , 1, 2, },
        { 0xD0, "BNE", &c::Relative , 2, 2, },
        { 0xD1, "CMP", &c::IndirectY, 2, 5, },
        { 0xD2, "XXX", &c::Implied  , 1, 2, },
        { 0xD3, "XXX", &c::Implied  , 1, 2, },
        { 0xD4, "XXX", &c::Implied  , 1, 2, },
        { 0xD5, "CMP", &c::ZeroPageX, 2, 4, },
        { 0xD6, "DEC", &c::ZeroPageX, 2, 6, },
        { 0xD7, "XXX", &c::Implied  , 1, 2, },
        { 0xD8, "CLD", &c::Implied  , 1, 2, },
        { 0xD9, "CMP", &c::AbsoluteY, 3, 4, },
        { 0xDA, "XXX", &c::Implied  , 1, 2, },
        { 0xDB, "XXX", &c::Implied  , 1, 2, },
        { 0xDC, "XXX", &c::Implied  , 1, 2, },
        { 0xDD, "CMP", &c::AbsoluteX, 3, 4, },
        { 0xDE, "DEC", &c::AbsoluteX, 3, 7, },
        { 0xDF, "XXX", &c::Implied  , 1, 2, },
        { 0xE0, "CPX", &c::Immediate, 2, 2, },
        { 0xE1, "SBC", &c::IndirectX, 2, 6, },
        { 0xE2, "XXX", &c::Implied  , 1, 2, },
        { 0xE3, "XXX", &c::Implied  , 1, 2, },
        { 0xE4, "CPX", &c::ZeroPage , 2, 3, },
        { 0xE5, "SBC", &c::ZeroPage , 2, 3, },
        { 0xE6, "INC", &c::ZeroPage , 2, 5, },
        { 0xE7, "XXX", &c::Implied  , 1, 2, },
        { 0xE8, "INX", &c::Implied  , 1, 2, },
        { 0xE9, "SBC", &c::Immediate, 2, 2, },
        { 0xEA, "NOP", &c::Implied  , 1, 2, },
        { 0xEB, "XXX", &c::Implied  , 1, 2, },
        { 0xEC, "CPX", &c::Absolute , 3, 4, },
        { 0xED, "SBC", &c::Absolute , 3, 4, },
        { 0xEE, "INC", &c::Absolute , 3, 6, },
        { 0xEF, "XXX", &c::Implied  , 1, 2, },
        { 0xF0, "BEQ", &c::Relative , 2, 2, },
        { 0xF1, "SBC", &c::IndirectY, 2, 5, },
        { 0xF2, "XXX", &c::Implied  , 1, 2, },
        { 0xF3, "XXX", &c::Implied  , 1, 2, },
        { 0xF4, "XXX", &c::Implied  , 1, 2, },
        { 0xF5, "SBC", &c::ZeroPageX, 2, 4, },
        { 0xF6, "INC", &c::ZeroPageX, 2, 6, },
        { 0xF7, "XXX", &c::Implied  , 1, 2, },
        { 0xF8, "SED", &c::Implied  , 1, 2, },
        { 0xF9, "SBC", &c::AbsoluteY, 3, 4, },
        { 0xFA, "XXX", &c::Implied  , 1, 2, },
        { 0xFB, "XXX", &c::Implied  , 1, 2, },
        { 0xFC, "XXX", &c::Implied  , 1, 2, },
        { 0xFD, "SBC", &c::AbsoluteX, 3, 4, },
        { 0xFE, "INC", &c::AbsoluteX, 3, 7, },
        { 0xFF, "XXX", &c::Implied  , 1, 2, },
        // clang-format on
    };
}

CPU::~CPU()
{
}

void CPU::write(uint16_t addr, uint8_t data)
{
    nes->cpuWrite(addr, data);
}

uint8_t CPU::read(uint16_t addr) const
{
    return nes->cpuRead(addr, false);
}

// push a value onto the stack
void CPU::push(uint8_t data, uint16_t offset)
{
    write(offset + sp, data);
    sp--;
}

// push a 16 bit value onto the stack
void CPU::push16(uint16_t data)
{
    push((data >> 8) & 0x00FF);
    push(data & 0x00FF);
}

// pop a value from the stack
uint8_t CPU::pop(uint16_t offset)
{
    sp++;
    return read(sp + offset);
}

// pop 16 bits from the stack
uint16_t CPU::pop16()
{
    uint16_t ret = pop();
    ret |= (uint16_t)(pop() << 8);
    return ret;
}

void CPU::reset()
{
    a = 0;
    x = 0;
    y = 0;
    pc = ((read(0xFFFD) << 8) | read(0xFFFC));
    sp = 0xFD;
    flags = 0;
    SetFlag(U);
    operand = 0;
    address = 0;
    cycles = 8;
}

void CPU::interrupt(uint16_t addr)
{
    push16(pc);
    SetFlag(B, 0);
    SetFlag(U, 1);
    push(flags);
    pc = ((((uint16_t)read(addr + 1)) << 8) | (uint16_t)read(addr));
    SetFlag(I, 1);
    cycles = 7;
}

void CPU::irq()
{
    // Are interrupts not disabled?
    if (!GetFlag(I)) {
        interrupt(0xFFFE);
    }
}

void CPU::nmi()
{
    // NMI cannot be ignored. Reads new PC from 0xFFFA
    interrupt(0xFFFA);
}

bool CPU::complete()
{
    return (cycles == 0);
}

void CPU::clock()
{
    if (cycles == 0) {
#if LOG_LEVEL > LOG_LEVEL_NOP
        uint16_t log_pc;
        char log[100];
        log_pc = pc;
#endif
        // We are ready for the next instruction!
        // Read next opcode:
        opcode = read(pc);

        // Unused flag should always be true
        SetFlag(U);

        // Upgrade your PC!
        pc++;

        // How many cycles are we doing today?
        cycles = lookupTable[opcode].cycles;

        int add1 = (this->*lookupTable[opcode].addrmode)();
        int add2 = 0; //(this->*lookupTable[opcode].operate)();

        uint16_t value = 0;

        // THE BIG SWITCH
        switch (opcode) {
        case 0x00: // BRK
            pc++;

            SetFlag(I, true);
            push16(pc);

            SetFlag(B, true);
            push(flags);

            pc = (((uint16_t)read(0xFFFF) << 8) | (uint16_t)read(0xFFFE));
            break;
        case 0x01: // ORA
        case 0x05: // ORA
        case 0x09: // ORA
        case 0x0D: // ORA
        case 0x11: // ORA
        case 0x15: // ORA
        case 0x19: // ORA
        case 0x1D: // ORA
            fetch();
            a |= operand;
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            add2 = 1;
            break;
        case 0x06: // ASL
        case 0x0A: // ASL
        case 0x0E: // ASL
        case 0x16: // ASL
        case 0x1E: // ASL
            fetch();
            temp = (uint16_t)operand << 1;
            SetFlag(C, (temp & 0xFF00) > 0);
            SetFlag(Z, (temp & 0x00FF) == 0x00);
            SetFlag(N, (temp & 0x0080));

            if (lookupTable[opcode].addrmode == &CPU::Implied)
                a = temp & 0x00FF;
            else
                write(address, temp & 0x00FF);
            break;
        case 0x08: // PHP
            // apparently PHP always pushes B as '1' (but doesn't actually set it)
            push(flags | B | U);
            SetFlag(B, false);
            SetFlag(U, false);
            break;
        case 0x10: // BPL
            if (!GetFlag(N)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0x18: // CLC
            SetFlag(C, false);
            break;
        case 0x20: // JSR
            pc--;
            push16(pc);
            pc = address;
            break;
        case 0x21: // AND
        case 0x25: // AND
        case 0x29: // AND
        case 0x2D: // AND
        case 0x31: // AND
        case 0x35: // AND
        case 0x39: // AND
        case 0x3D: // AND
            fetch();
            a = a & operand;
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            add2 = 1;
            break;
        case 0x24: // BIT
        case 0x2C: // BIT
            fetch();
            temp = a & operand;
            SetFlag(Z, (temp & 0x00FF) == 0x00);
            SetFlag(N, operand & 0x80);
            SetFlag(V, operand & 0x40);
            break;
        case 0x26: // ROL
        case 0x2A: // ROL
        case 0x2E: // ROL
        case 0x36: // ROL
        case 0x3E: // ROL
            fetch();
            temp = (uint16_t)(operand << 1) | GetFlag(C);
            SetFlag(C, temp & 0xFF00);
            SetFlag(Z, (temp & 0x00FF) == 0x0000);
            SetFlag(N, temp & 0x0080);
            if (lookupTable[opcode].addrmode == &CPU::Implied)
                a = temp & 0x00FF;
            else
                write(address, temp & 0x00FF);
            break;
        case 0x28: // PLP
            flags = pop();
            SetFlag(U);
            break;
        case 0x30: // BMI
            if (GetFlag(N)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0x38: // SEC
            SetFlag(C);
            break;
        case 0x40: // RTI
            flags = pop();
            SetFlag(B, false);
            SetFlag(U, false);
            pc = pop16();
            break;
        case 0x41: // EOR
        case 0x45: // EOR
        case 0x49: // EOR
        case 0x4D: // EOR
        case 0x51: // EOR
        case 0x55: // EOR
        case 0x59: // EOR
        case 0x5D: // EOR
            fetch();
            a = a ^ operand;
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            break;
        case 0x46: // LSR
        case 0x4A: // LSR
        case 0x4E: // LSR
        case 0x56: // LSR
        case 0x5E: // LSR
            fetch();

            temp = (uint16_t)operand >> 1;
            SetFlag(C, operand & 0x0001);
            SetFlag(Z, (temp & 0x00FF) == 0x00);
            SetFlag(N, (temp & 0x0080));

            if (lookupTable[opcode].addrmode == &CPU::Implied)
                a = temp & 0x00FF;
            else
                write(address, temp & 0x00FF);

            break;
        case 0x48: // PHA
            push(a);
            break;
        case 0x4C: // JMP
        case 0x6C: // JMP
            pc = address;
            break;
        case 0x50: // BVC
            if (!GetFlag(V)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0x58: // CLI
            SetFlag(I, false);
            break;
        case 0x60: // RTS
            pc = pop16();
            pc++;
            break;
        case 0x61: // ADC
        case 0x65: // ADC
        case 0x69: // ADC
        case 0x6D: // ADC
        case 0x71: // ADC
        case 0x75: // ADC
        case 0x79: // ADC
        case 0x7D: // ADC
            fetch();
            temp = (uint16_t)a + (uint16_t)operand + (uint16_t)(GetFlag(C) ? 1 : 0);
            SetFlag(C, temp > 255);
            SetFlag(Z, (temp & 0x00FF) == 0);
            // I have no idea what's going on here lol
            // Taken from OneLoneCoder
            SetFlag(V, (~((uint16_t)a ^ (uint16_t)operand) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080);
            SetFlag(N, temp & 0x0080);
            a = temp & 0x00FF;
            add2 = 1;
            break;
        case 0x66: // ROR
        case 0x6A: // ROR
        case 0x6E: // ROR
        case 0x76: // ROR
        case 0x7E: // ROR
            fetch();
            temp = (uint16_t)(GetFlag(C) << 7) | (operand >> 1);
            SetFlag(C, operand & 0x01);
            SetFlag(Z, (temp & 0x00FF) == 0x00);
            SetFlag(N, temp & 0x0080);
            if (lookupTable[opcode].addrmode == &CPU::Implied)
                a = temp & 0x00FF;
            else
                write(address, temp & 0x00FF);
            break;
        case 0x68: // PLA
            a = pop();
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            break;
        case 0x70: // BVS
            if (GetFlag(V)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0x78: // SEI
            SetFlag(I);
            break;
        case 0x81: // STA
        case 0x85: // STA
        case 0x8D: // STA
        case 0x91: // STA
        case 0x95: // STA
        case 0x9D: // STA
        case 0x99: // STA
            write(address, a);
            break;
        case 0x84: // STY
        case 0x8C: // STY
        case 0x94: // STY
            write(address, y);
            break;
        case 0x86: // STX
        case 0x8E: // STX
        case 0x96: // STX
            write(address, x);
            break;
        case 0x88: // DEY
            y--;
            SetFlag(Z, y == 0x00);
            SetFlag(N, y & 0x80);
            break;
        case 0x8A: // TXA
            a = x;
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            break;
        case 0x98: // TYA
            a = y;
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            break;
        case 0x9A: // TXS
            sp = x;
            break;
        case 0x90: // BCC
            if (!GetFlag(C)) {
                // cycles++ if branch succeeds
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0xA0: // LDY
        case 0xA4: // LDY
        case 0xAC: // LDY
        case 0xB4: // LDY
        case 0xBC: // LDY
            fetch();
            y = operand;
            SetFlag(Z, y == 0x00);
            SetFlag(N, y & 0x80);
            add2 = 1;
            break;
        case 0xA1: // LDA
        case 0xA5: // LDA
        case 0xA9: // LDA
        case 0xAD: // LDA
        case 0xB1: // LDA
        case 0xB5: // LDA
        case 0xB9: // LDA
        case 0xBD: // LDA
            fetch();
            a = operand;
            SetFlag(Z, a == 0x00);
            SetFlag(N, a & 0x80);
            add2 = 1;
            break;
        case 0xA2: // LDX
        case 0xA6: // LDX
        case 0xAE: // LDX
        case 0xB6: // LDX
        case 0xBE: // LDX
            fetch();
            x = operand;
            SetFlag(Z, x == 0x00);
            SetFlag(N, x & 0x80);
            add2 = 1;
            break;
        case 0xA8: // TAY
            y = a;
            SetFlag(Z, y == 0x00);
            SetFlag(N, y & 0x80);
            break;
        case 0xAA: // TAX
            x = a;
            SetFlag(Z, x == 0x00);
            SetFlag(N, x & 0x80);
            break;
        case 0xBA: // TSX
            x = sp;
            SetFlag(Z, x == 0x00);
            SetFlag(N, x & 0x80);
            break;
        case 0xB0: // BCS
            if (GetFlag(C)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0xB8: // CLV
            SetFlag(V, false);
            break;
        case 0xC0: // CPY
        case 0xC4: // CPY
        case 0xCC: // CPY
            fetch();
            temp = (uint16_t)y - (uint16_t)operand;
            SetFlag(C, y >= operand);
            SetFlag(Z, y == operand);
            SetFlag(N, temp & 0x0080);
            add2 = 1;
            break;
        case 0xC1: // CMP
        case 0xC5: // CMP
        case 0xC9: // CMP
        case 0xCD: // CMP
        case 0xD1: // CMP
        case 0xD5: // CMP
        case 0xD9: // CMP
        case 0xDD: // CMP
            fetch();
            temp = (uint16_t)a - (uint16_t)operand;
            //printf("temp: %04X  a: %02X  operand: %02X\n", temp, a, operand);
            SetFlag(C, a >= operand);
            SetFlag(Z, a == operand);
            SetFlag(N, temp & 0x0080);
            add2 = 1;
            break;
        case 0xE0: // CPX
        case 0xE4: // CPX
        case 0xEC: // CPX
            fetch();
            temp = (uint16_t)x - (uint16_t)operand;
            SetFlag(C, x >= operand);
            SetFlag(Z, x == operand);
            SetFlag(N, temp & 0x0080);
            add2 = 1;
            break;
        case 0xC6: // DEC
        case 0xCE: // DEC
        case 0xD6: // DEC
        case 0xDE: // DEC
            fetch();
            temp = operand - 1;
            SetFlag(Z, (temp & 0x00FF) == 0x00);
            SetFlag(N, temp & 0x0080);
            write(address, temp);
            break;
        case 0xC8: // INY
            y++;
            SetFlag(Z, y == 0x00);
            SetFlag(N, y & 0x80);
            break;
        case 0xCA: // DEX
            x--;
            SetFlag(Z, x == 0x00);
            SetFlag(N, x & 0x80);
            break;
        case 0xD0: // BNE
            if (!GetFlag(Z)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0xD8: // CLD
            SetFlag(D, false);
            break;
        case 0xE1: // SBC
        case 0xE5: // SBC
        case 0xE9: // SBC
        case 0xED: // SBC
        case 0xF1: // SBC
        case 0xF5: // SBC
        case 0xF9: // SBC
        case 0xFD: // SBC
            // Taken from OLC. I haven't made an effort to fully understand it...
            fetch();

            // Invert the bottom 8 bits (makes the operand negative)
            value = ((uint16_t)operand) ^ 0x00FF;
            // After that it's exactly the same as ADC ?!??
            // (adding a negative number is equal to subtracting the positive equivalent of that number)
            temp = (uint16_t)a + value + (uint16_t)(GetFlag(C) ? 1 : 0);
            SetFlag(C, temp & 0xFF00);
            SetFlag(Z, (temp & 0x00FF) == 0);
            // I have no idea what's going on here lol
            SetFlag(V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080);
            SetFlag(N, temp & 0x0080);
            a = temp & 0x00FF;

            add2 = 1;
            break;
        case 0xE6: // INC
        case 0xEE: // INC
        case 0xF6: // INC
        case 0xFE: // INC
            fetch();
            temp = operand + 1;
            SetFlag(Z, (temp & 0x00FF) == 0x00);
            SetFlag(N, temp & 0x0080);
            write(address, temp);
            break;
        case 0xE8: // INX
            x++;
            SetFlag(Z, x == 0x00);
            SetFlag(N, x & 0x80);
            break;
        case 0xEA: // NOP
            break;
        case 0xF0: // BEQ
            if (GetFlag(Z)) {
                cycles++;
                // Set address to branch to
                address = pc + address_rel;
                // Check for page boundary crossed
                if ((address & 0xFF00) != (pc & 0xFF00))
                    cycles++;

                pc = address;
            }
            break;
        case 0xF8: // SED
            SetFlag(D);
            break;
            //case 0x5A: // XXX
            //case 0x5B: // XXX
            //case 0x5C: // XXX
            //case 0x5F: // XXX
            //case 0x62: // XXX
            //case 0x63: // XXX
            //case 0x64: // XXX
            //case 0x67: // XXX
            //case 0x6B: // XXX
            //case 0x6F: // XXX
            //case 0x72: // XXX
            //case 0x73: // XXX
            //case 0x74: // XXX
            //case 0x77: // XXX
            //case 0x7A: // XXX
            //case 0x7B: // XXX
            //case 0x7C: // XXX
            //case 0x7F: // XXX
            //case 0x80: // XXX
            //case 0x82: // XXX
            //case 0x83: // XXX
            //case 0x87: // XXX
            //case 0x89: // XXX
            //case 0x8B: // XXX
            //case 0x8F: // XXX
            //case 0x92: // XXX
            //case 0x93: // XXX
            //case 0x97: // XXX
            //case 0x9B: // XXX
            //case 0x9C: // XXX
            //case 0x9E: // XXX
            //case 0x9F: // XXX
            //case 0xA3: // XXX
            //case 0xA7: // XXX
            //case 0xAB: // XXX
            //case 0xAF: // XXX
            //case 0xB2: // XXX
            //case 0xB3: // XXX
            //case 0xB7: // XXX
            //case 0xBB: // XXX
            //case 0xBF: // XXX
            //case 0xC2: // XXX
            //case 0xC3: // XXX
            //case 0xC7: // XXX
            //case 0xCB: // XXX
            //case 0xCF: // XXX
            //case 0xD2: // XXX
            //case 0xD3: // XXX
            //case 0xD4: // XXX
            //case 0xD7: // XXX
            //case 0xDA: // XXX
            //case 0xDB: // XXX
            //case 0xDC: // XXX
            //case 0xDF: // XXX
            //case 0xE2: // XXX
            //case 0xE3: // XXX
            //case 0xE7: // XXX
            //case 0xEB: // XXX
            //case 0xEF: // XXX
            //case 0xF2: // XXX
            //case 0xF3: // XXX
            //case 0xF4: // XXX
            //case 0xF7: // XXX
            //case 0xFA: // XXX
            //case 0xFB: // XXX
            //case 0xFC: // XXX
            //case 0xFF: // XXX

        default:
            printf("Illegal opcode %02X encountered!\n", opcode);
            add2 = 0;
            break;
        }

        cycles += (add1 & add2);
#if LOG_LEVEL > LOG_LEVEL_NOP
        sprintf(log, "%10ld: PC:%04X [Opcode %02X] A:%02X X:%02X Y:%02X", total_cycles, log_pc, opcode, a, x, y);
        l.w(std::string(log));
#endif
    }

    total_cycles++;
    cycles--;
}

uint8_t CPU::Relative()
{
    address_rel = read(pc) & 0x00FF;
    pc++;
    if (address_rel & 0x80)
        address_rel |= 0xFF00;
    //printf("address_rel set to %04X (%d)\n", address_rel, (int8_t)address_rel);
    return 0;
}

uint8_t CPU::AbsoluteX()
{
    address = read(pc);
    pc++;
    temp = read(pc); // hi 8 bits
    address |= (temp << 8);
    pc++;
    address += x;

    if ((address & 0xFF00) != (temp << 8))
        return 1;
    else
        return 0;
}

uint8_t CPU::AbsoluteY()
{
    address = read(pc);
    pc++;
    temp = read(pc); // hi 8 bits
    address |= (temp << 8);
    pc++;
    address += y;

    if ((address & 0xFF00) != (temp << 8))
        return 1;
    else
        return 0;
}

uint8_t CPU::Indirect()
{
    bool bug = false;

    temp = read(pc);
    if (temp == 0x00FF) // we must emulate the hardware bug
        bug = true;
    pc++;
    temp |= (read(pc) << 8);
    pc++;

    // now temp == an address
    // we need to read that address to get the real address
    // so the JMP instruction can jump to 'address'
    //

    if (bug) {
        address = read(temp) | (read(temp & 0xFF00) << 8);
    } else {
        address = read(temp) | (read(temp + 1) << 8);
    }

    return 0;
}

uint8_t CPU::IndirectX()
{
    temp = read(pc);
    pc++;
    temp += x;

    address = (uint16_t)read(temp & 0x00FF) | (uint16_t)(read((temp + 1) & 0x00FF) << 8);

    //uint16_t lo = read((uint16_t)(temp + (uint16_t)x) & 0x00FF);
    //uint16_t hi = read((uint16_t)(temp + (uint16_t)x + 1) & 0x00FF);
    //address = (hi << 8) | lo;

    return 0;
}

uint8_t CPU::IndirectY()
{
    temp = read(pc);
    pc++;

    uint16_t lo = read(temp & 0x00FF);
    uint16_t hi = read((temp + 1) & 0x00FF);
    address = ((hi << 8) | lo) + y;

    if ((address & 0xFF00) != (hi << 8))
        return 1;
    else
        return 0;
}

// Get effective addresses for various modes (for the disassembler)

uint16_t CPU::GetAddrZP(uint16_t addr)
{
    return (addr & 0x00FF);
}

uint16_t CPU::GetAddrZPX(uint16_t addr)
{
    uint16_t ret = 0;
    ret = (addr + x) & 0x00FF;
    return ret;
}

uint16_t CPU::GetAddrZPY(uint16_t addr)
{
    uint16_t ret = 0;
    ret = (addr + y) & 0x00FF;
    return ret;
}

uint16_t CPU::GetAddrIDX(uint16_t addr)
{
    uint16_t ret = 0;
    temp = read(addr) + x;
    ret = read(temp & 0x00FF) | (read((temp + 1) & 0x00FF) << 8);
    return ret;
}

uint16_t CPU::GetAddrIDY(uint16_t addr)
{
    uint16_t ret = 0;
    uint16_t lo = read(addr & 0x00FF);
    uint16_t hi = read((addr + 1) & 0x00FF);
    ret = ((hi << 8) | lo) + y;
    return ret;
}

// Disassembler
// Heavily based on OLC's work
// TODO: for indirect/indexed addressing show actual values as well!

DisassemblyType CPU::disassemble(uint16_t start, uint16_t end)
{
    uint32_t addr = start;
    uint16_t line = 0, effective_address = 0;
    uint8_t value = 0, lo = 0, hi = 0;
    DisassemblyType lines;

    auto hex = [](uint32_t n, uint8_t d) {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    while (addr <= (uint32_t)end) {
        line = addr;
        std::string inst = "$" + hex(addr, 4) + ": ";
        uint8_t opcode = nes->cpuRead(addr, true);
        addr++;
        inst += lookupTable[opcode].mnemonic + " ";

        if (lookupTable[opcode].addrmode == &CPU::Implied) {
            inst += " ";
        } else if (lookupTable[opcode].addrmode == &CPU::Immediate) {
            value = nes->cpuRead(addr, true);
            addr++;
            inst += "#$" + hex(value, 2);
        } else if (lookupTable[opcode].addrmode == &CPU::ZeroPage) {
            lo = nes->cpuRead(addr, true);
            effective_address = GetAddrZP(lo);
            addr++;
            inst += "$" + hex(lo, 2) + " [$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2) + "]";
        } else if (lookupTable[opcode].addrmode == &CPU::ZeroPageX) {
            lo = nes->cpuRead(addr, true);
            effective_address = GetAddrZPX(lo);
            addr++;
            inst += "$" + hex(lo, 2) + ", X [$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2) + "]";
        } else if (lookupTable[opcode].addrmode == &CPU::ZeroPageY) {
            lo = nes->cpuRead(addr, true);
            effective_address = GetAddrZPY(lo);
            addr++;
            inst += "$" + hex(lo, 2) + ", Y [$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2) + "]";
        } else if (lookupTable[opcode].addrmode == &CPU::IndirectX) {
            lo = nes->cpuRead(addr, true);
            effective_address = GetAddrIDX(lo);
            addr++;
            inst += "($" + hex(lo, 2) + ", X) [$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2) + "]";
        } else if (lookupTable[opcode].addrmode == &CPU::IndirectY) {
            lo = nes->cpuRead(addr, true);
            effective_address = GetAddrIDY(lo);
            addr++;
            inst += "($" + hex(lo, 2) + "), Y [$" + hex(effective_address, 4) + " = $" + hex(nes->cpuRead(effective_address), 2) + "]";
        } else if (lookupTable[opcode].addrmode == &CPU::Absolute) {
            lo = nes->cpuRead(addr, true);
            addr++;
            hi = nes->cpuRead(addr, true);
            addr++;
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4);
        } else if (lookupTable[opcode].addrmode == &CPU::AbsoluteX) {
            lo = nes->cpuRead(addr, true);
            addr++;
            hi = nes->cpuRead(addr, true);
            addr++;
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X";
        } else if (lookupTable[opcode].addrmode == &CPU::AbsoluteY) {
            lo = nes->cpuRead(addr, true);
            addr++;
            hi = nes->cpuRead(addr, true);
            addr++;
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y";
        } else if (lookupTable[opcode].addrmode == &CPU::Indirect) {
            lo = nes->cpuRead(addr, true);
            addr++;
            hi = nes->cpuRead(addr, true);
            addr++;
            inst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ")";
        } else if (lookupTable[opcode].addrmode == &CPU::Relative) {
            value = nes->cpuRead(addr, true);
            addr++;
            inst += "$" + hex(value, 2) + "      [$" + hex(addr + (int8_t)value, 4) + "]";
        } else {
            inst += "ERROR: Unknown addressing mode!";
        }

        // lines[line] = inst;
        std::map<uint16_t, std::string> temp;
        temp[line] = inst;
        lines.push_back(temp);
    }

    return lines;
}

// prerequisite:
// RAM 0x00 - 0xFF = 0x00 - 0xFF ......
void CPU::TestOpcodes()
{
    opcode = read(pc);
    printf("PC: %04x - ", pc);
    if (opcode == lookupTable[opcode].opcode) {
        printf("Opcode 0x%02x matches index!\n", opcode);
    } else {
        printf("Opcode 0x%02x DOES NOT MATCH index!\n", opcode);
    }

    pc++;
}
//std::unordered_map<int, Opcode> CPU::CreateOpcodes()
//{
//    std::unordered_map<int, Opcode> opcodes;

//opcodes[0x69] = Opcode("ADC", 2, 2, "NVbdiZC", Immediate);
//opcodes[0x65] = Opcode("ADC", 2, 3, "NVbdiZC", ZeroPage );
//opcodes[0x75] = Opcode("ADC", 2, 4, "NVbdiZC", ZeroPageX);
//opcodes[0x6D] = Opcode("ADC", 3, 4, "NVbdiZC", Absolute );
//opcodes[0x7D] = Opcode("ADC", 3, 4, "NVbdiZC", AbsoluteX);
//opcodes[0x79] = Opcode("ADC", 3, 4, "NVbdiZC", AbsoluteY);
//opcodes[0x61] = Opcode("ADC", 2, 6, "NVbdiZC", IndirectX);
//opcodes[0x71] = Opcode("ADC", 2, 5, "NVbdiZC", IndirectY);

//opcodes[0x29] = Opcode("AND", 2, 2, "NvbdiZc", Immediate);
//opcodes[0x25] = Opcode("AND", 2, 3, "NvbdiZc", ZeroPage );
//opcodes[0x35] = Opcode("AND", 2, 4, "NvbdiZc", ZeroPageX);
//opcodes[0x2D] = Opcode("AND", 3, 4, "NvbdiZc", Absolute );
//opcodes[0x3D] = Opcode("AND", 3, 4, "NvbdiZc", AbsoluteX);
//opcodes[0x39] = Opcode("AND", 3, 4, "NvbdiZc", AbsoluteY);
//opcodes[0x21] = Opcode("AND", 2, 6, "NvbdiZc", IndirectX);
//opcodes[0x31] = Opcode("AND", 2, 5, "NvbdiZc", IndirectY);

//opcodes[0x0A] = Opcode("ASL", 1, 2, "NvbdiZC", Accumulator);
//opcodes[0x06] = Opcode("ASL", 2, 5, "NvbdiZC", ZeroPage );
//opcodes[0x16] = Opcode("ASL", 2, 6, "NvbdiZC", ZeroPageX);
//opcodes[0x0E] = Opcode("ASL", 3, 6, "NvbdiZC", Absolute );
//opcodes[0x1E] = Opcode("ASL", 3, 7, "NvbdiZC", AbsoluteX);

//opcodes[0x24] = Opcode("BIT", 2, 3, "NVbdiZc", ZeroPage );
//opcodes[0x2C] = Opcode("BIT", 3, 4, "NVbdiZc", Absolute );

//opcodes[0x10] = Opcode("BPL", 2, 3, "nvbdizc", Relative );
//opcodes[0x30] = Opcode("BMI", 2, 3, "nvbdizc", Relative );
//opcodes[0x50] = Opcode("BVC", 2, 3, "nvbdizc", Relative );
//opcodes[0x70] = Opcode("BVS", 2, 3, "nvbdizc", Relative );
//opcodes[0x90] = Opcode("BCC", 2, 3, "nvbdizc", Relative );
//opcodes[0xB0] = Opcode("BCS", 2, 3, "nvbdizc", Relative );
//opcodes[0xD0] = Opcode("BNE", 2, 3, "nvbdizc", Relative );
//opcodes[0xF0] = Opcode("BEQ", 2, 3, "nvbdizc", Relative );

//opcodes[0x00] = Opcode("BRK", 1, 7, "nvBdizc", Implied  );

//opcodes[0xC9] = Opcode("CMP", 2, 2, "NvbdiZC", Immediate);
//opcodes[0xC5] = Opcode("CMP", 2, 3, "NvbdiZC", ZeroPage );
//opcodes[0xD5] = Opcode("CMP", 2, 4, "NvbdiZC", ZeroPageX);
//opcodes[0xCD] = Opcode("CMP", 3, 4, "NvbdiZC", Absolute );
//opcodes[0xDD] = Opcode("CMP", 3, 4, "NvbdiZC", AbsoluteX);
//opcodes[0xD9] = Opcode("CMP", 3, 4, "NvbdiZC", AbsoluteY);
//opcodes[0xC1] = Opcode("CMP", 2, 6, "NvbdiZC", IndirectX);
//opcodes[0xD1] = Opcode("CMP", 2, 5, "NvbdiZC", IndirectY);

//opcodes[0xE0] = Opcode("CPX", 2, 2, "NvbdiZC", Immediate);
//opcodes[0xE4] = Opcode("CPX", 2, 3, "NvbdiZC", ZeroPage );
//opcodes[0xEC] = Opcode("CPX", 3, 4, "NvbdiZC", Absolute );

//opcodes[0xC0] = Opcode("CPY", 2, 2, "NvbdiZC", Immediate);
//opcodes[0xC4] = Opcode("CPY", 2, 3, "NvbdiZC", ZeroPage );
//opcodes[0xCC] = Opcode("CPY", 3, 4, "NvbdiZC", Absolute );

//opcodes[0xC6] = Opcode("DEC", 2, 5, "NvbdiZc", ZeroPage );
//opcodes[0xD6] = Opcode("DEC", 2, 6, "NvbdiZc", ZeroPageX);
//opcodes[0xCE] = Opcode("DEC", 3, 6, "NvbdiZc", Absolute );
//opcodes[0xDE] = Opcode("DEC", 3, 7, "NvbdiZc", AbsoluteX);

//opcodes[0x49] = Opcode("EOR", 2, 2, "NvbdiZc", Immediate);
//opcodes[0x45] = Opcode("EOR", 2, 3, "NvbdiZc", ZeroPage );
//opcodes[0x55] = Opcode("EOR", 2, 4, "NvbdiZc", ZeroPageX);
//opcodes[0x4D] = Opcode("EOR", 3, 4, "NvbdiZc", Absolute );
//opcodes[0x5D] = Opcode("EOR", 3, 4, "NvbdiZc", AbsoluteX);
//opcodes[0x59] = Opcode("EOR", 3, 4, "NvbdiZc", AbsoluteY);
//opcodes[0x41] = Opcode("EOR", 2, 6, "NvbdiZc", IndirectX);
//opcodes[0x51] = Opcode("EOR", 2, 5, "NvbdiZc", IndirectY);

//// flags are probably wrong here
//opcodes[0x18] = Opcode("CLC", 1, 2, "nvbdizc", Implied  );
//opcodes[0x38] = Opcode("SEC", 1, 2, "nvbdizc", Implied  );
//opcodes[0x58] = Opcode("CLI", 1, 2, "nvbdizc", Implied  );
//opcodes[0x78] = Opcode("SEI", 1, 2, "nvbdizc", Implied  );
//opcodes[0xB8] = Opcode("CLV", 1, 2, "nvbdizc", Implied  );
//opcodes[0xD8] = Opcode("CLD", 1, 2, "nvbdizc", Implied  );
//opcodes[0xF8] = Opcode("SED", 1, 2, "nvbdizc", Implied  );

//opcodes[0xE6] = Opcode("INC", 2, 5, "NvbdiZc", ZeroPage );
//opcodes[0xF6] = Opcode("INC", 2, 6, "NvbdiZc", ZeroPageX);
//opcodes[0xEE] = Opcode("INC", 3, 6, "NvbdiZc", Absolute );
//opcodes[0xFE] = Opcode("INC", 3, 7, "NvbdiZc", AbsoluteX);

//opcodes[0x4C] = Opcode("JMP", 3, 3, "nvbdizc", Absolute );
//opcodes[0x6C] = Opcode("JMP", 3, 5, "nvbdizc", Indirect );

//opcodes[0x20] = Opcode("JSR", 3, 6, "nvbdizc", Absolute );

//opcodes[0xA9] = Opcode("LDA", 2, 2, "NvbdiZc", Immediate);
//opcodes[0xA5] = Opcode("LDA", 2, 3, "NvbdiZc", ZeroPage );
//opcodes[0xB5] = Opcode("LDA", 2, 4, "NvbdiZc", ZeroPageX);
//opcodes[0xAD] = Opcode("LDA", 3, 4, "NvbdiZc", Absolute );
//opcodes[0xBD] = Opcode("LDA", 3, 4, "NvbdiZc", AbsoluteX);
//opcodes[0xB9] = Opcode("LDA", 3, 4, "NvbdiZc", AbsoluteY);
//opcodes[0xA1] = Opcode("LDA", 2, 6, "NvbdiZc", IndirectX);
//opcodes[0xB1] = Opcode("LDA", 2, 5, "NvbdiZc", IndirectY);

//opcodes[0xA2] = Opcode("LDX", 2, 2, "NvbdiZc", Immediate);
//opcodes[0xA6] = Opcode("LDX", 2, 3, "NvbdiZc", ZeroPage );
//opcodes[0xB6] = Opcode("LDX", 2, 4, "NvbdiZc", ZeroPageY);
//opcodes[0xAE] = Opcode("LDX", 3, 4, "NvbdiZc", Absolute );
//opcodes[0xBE] = Opcode("LDX", 3, 4, "NvbdiZc", AbsoluteY);

//opcodes[0xA0] = Opcode("LDY", 2, 2, "NvbdiZc", Immediate);
//opcodes[0xA4] = Opcode("LDY", 2, 3, "NvbdiZc", ZeroPage );
//opcodes[0xB4] = Opcode("LDY", 2, 4, "NvbdiZc", ZeroPageX);
//opcodes[0xAC] = Opcode("LDY", 3, 4, "NvbdiZc", Absolute );
//opcodes[0xBC] = Opcode("LDY", 3, 4, "NvbdiZc", AbsoluteX);

//opcodes[0x4A] = Opcode("LSR", 1, 2, "NvbdiZC", Accumulator);
//opcodes[0x46] = Opcode("LSR", 2, 5, "NvbdiZC", ZeroPage );
//opcodes[0x56] = Opcode("LSR", 2, 6, "NvbdiZC", ZeroPageX);
//opcodes[0x4E] = Opcode("LSR", 3, 6, "NvbdiZC", Absolute );
//opcodes[0x5E] = Opcode("LSR", 3, 7, "NvbdiZC", AbsoluteX);

//opcodes[0xEA] = Opcode("NOP", 1, 2, "nvbdizc", Implied  );

//opcodes[0x09] = Opcode("ORA", 2, 2, "NvbdiZc", Immediate);
//opcodes[0x05] = Opcode("ORA", 2, 3, "NvbdiZc", ZeroPage );
//opcodes[0x15] = Opcode("ORA", 2, 4, "NvbdiZc", ZeroPageX);
//opcodes[0x0D] = Opcode("ORA", 3, 4, "NvbdiZc", Absolute );
//opcodes[0x1D] = Opcode("ORA", 3, 4, "NvbdiZc", AbsoluteX);
//opcodes[0x19] = Opcode("ORA", 3, 4, "NvbdiZc", AbsoluteY);
//opcodes[0x01] = Opcode("ORA", 2, 6, "NvbdiZc", IndirectX);
//opcodes[0x11] = Opcode("ORA", 2, 5, "NvbdiZc", IndirectY);

//opcodes[0xAA] = Opcode("TAX", 1, 2, "NvbdiZc", Implied  );
//opcodes[0x8A] = Opcode("TXA", 1, 2, "NvbdiZc", Implied  );
//opcodes[0xCA] = Opcode("DEX", 1, 2, "NvbdiZc", Implied  );
//opcodes[0xE8] = Opcode("INX", 1, 2, "NvbdiZc", Implied  );
//opcodes[0xA8] = Opcode("TAY", 1, 2, "NvbdiZc", Implied  );
//opcodes[0x98] = Opcode("TYA", 1, 2, "NvbdiZc", Implied  );
//opcodes[0x88] = Opcode("DEY", 1, 2, "NvbdiZc", Implied  );
//opcodes[0xC8] = Opcode("INY", 1, 2, "NvbdiZc", Implied  );

//opcodes[0x2A] = Opcode("ROL", 1, 2, "NvbdiZC", Accumulator);
//opcodes[0x26] = Opcode("ROL", 2, 5, "NvbdiZC", ZeroPage );
//opcodes[0x36] = Opcode("ROL", 2, 6, "NvbdiZC", ZeroPageX);
//opcodes[0x2E] = Opcode("ROL", 3, 6, "NvbdiZC", Absolute );
//opcodes[0x3E] = Opcode("ROL", 3, 7, "NvbdiZC", AbsoluteX);

//opcodes[0x6A] = Opcode("ROR", 1, 2, "NvbdiZC", Accumulator);
//opcodes[0x66] = Opcode("ROR", 2, 5, "NvbdiZC", ZeroPage );
//opcodes[0x76] = Opcode("ROR", 2, 6, "NvbdiZC", ZeroPageX);
//opcodes[0x6E] = Opcode("ROR", 3, 6, "NvbdiZC", Absolute );
//opcodes[0x7E] = Opcode("ROR", 3, 7, "NvbdiZC", AbsoluteX);

//opcodes[0x40] = Opcode("RTI", 1, 6, "NVBDIZC", Implied  );
//opcodes[0x60] = Opcode("RTS", 1, 6, "nvbdizc", Implied  );

//opcodes[0xE9] = Opcode("SBC", 2, 2, "NVbdiZC", Immediate);
//opcodes[0xE5] = Opcode("SBC", 2, 3, "NVbdiZC", ZeroPage );
//opcodes[0xF5] = Opcode("SBC", 2, 4, "NVbdiZC", ZeroPageX);
//opcodes[0xED] = Opcode("SBC", 3, 4, "NVbdiZC", Absolute );
//opcodes[0xFD] = Opcode("SBC", 3, 4, "NVbdiZC", AbsoluteX);
//opcodes[0xF9] = Opcode("SBC", 3, 4, "NVbdiZC", AbsoluteY);
//opcodes[0xE1] = Opcode("SBC", 2, 6, "NVbdiZC", IndirectX);
//opcodes[0xF1] = Opcode("SBC", 2, 5, "NVbdiZC", IndirectY);

//opcodes[0x85] = Opcode("STA", 2, 3, "nvbdizc", ZeroPage );
//opcodes[0x95] = Opcode("STA", 2, 4, "nvbdizc", ZeroPageX);
//opcodes[0x8D] = Opcode("STA", 3, 4, "nvbdizc", Absolute );
//opcodes[0x9D] = Opcode("STA", 3, 5, "nvbdizc", AbsoluteX);
//opcodes[0x99] = Opcode("STA", 3, 5, "nvbdizc", AbsoluteY);
//opcodes[0x81] = Opcode("STA", 2, 6, "nvbdizc", IndirectX);
//opcodes[0x91] = Opcode("STA", 2, 6, "nvbdizc", IndirectY);

//opcodes[0x9A] = Opcode("TXS", 1, 2, "NvbdiZc", Implied  );
//opcodes[0xBA] = Opcode("TSX", 1, 2, "NvbdiZc", Implied  );
//opcodes[0x48] = Opcode("PHA", 1, 3, "NvbdiZc", Implied  );
//opcodes[0x68] = Opcode("PLA", 1, 4, "NvbdiZc", Implied  );
//opcodes[0x08] = Opcode("PHP", 1, 3, "NvbdiZc", Implied  );
//opcodes[0x28] = Opcode("PLP", 1, 4, "NvbdiZc", Implied  );

//opcodes[0x86] = Opcode("STX", 2, 3, "nvbdizc", ZeroPage );
//opcodes[0x96] = Opcode("STX", 2, 4, "nvbdizc", ZeroPageY);
//opcodes[0x8E] = Opcode("STX", 3, 4, "nvbdizc", Absolute );

//opcodes[0x84] = Opcode("STY", 2, 3, "nvbdizc", ZeroPage );
//opcodes[0x94] = Opcode("STY", 2, 4, "nvbdizc", ZeroPageY);
//opcodes[0x8C] = Opcode("STY", 3, 4, "nvbdizc", Absolute );

//    return opcodes;
//}

// vim: foldmethod=syntax
