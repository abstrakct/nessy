/*
 * 6502 cpu stuff
 */

#include "cpu.h"
#include "machine.h"

//uint8_t Memory::read(uint16_t address)
//{
//    if (address < 0x2000) {
//        return ram[address & 0x7ff];
//    } else {
//        return rom[address];
//    }
//}
//
//void Memory::write(uint16_t address, uint8_t value)
//{
//    if (address < 0x2000) {
//        ram[address & 0x7ff] = value;
//    } else {
//        rom[address] = value;
//    }
//}
//
//void Memory::resizeRom(int size)
//{
//    rom.resize(size, 0);
//}


// TEORI:
// bit 1 = indirect x
// bit 3 = zero page
// bit 4 = immediate
// bit 5 (& bit 1) = indirect y
//
//
CPU::CPU()
{
    using c = CPU;
    lookup = {
        // CHECKED - OK:
        { 0x00, "BRK", &c::BRK, &c::Implied  , 1, 7, },
        { 0x01, "ORA", &c::ORA, &c::IndirectX, 2, 6, },
        { 0x02, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x03, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x04, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x05, "ORA", &c::ORA, &c::ZeroPage , 2, 3, },
        { 0x06, "ASL", &c::ASL, &c::ZeroPage , 2, 5, },
        { 0x07, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x08, "PHP", &c::PHP, &c::Implied  , 1, 3, },
        { 0x09, "ORA", &c::ORA, &c::Immediate, 2, 2, },
        { 0x0A, "ASL", &c::ASL, &c::Implied,   1, 2, },
        { 0x0B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x0C, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x0D, "ORA", &c::ORA, &c::Absolute , 3, 4, },
        { 0x0E, "ASL", &c::ASL, &c::Absolute , 3, 6, },
        { 0x0F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x10, "BPL", &c::BPL, &c::Relative , 2, 2, },
        { 0x11, "ORA", &c::ORA, &c::IndirectY, 2, 5, },
        { 0x12, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x13, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x14, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x15, "ORA", &c::ORA, &c::ZeroPageX, 2, 4, },
        { 0x16, "ASL", &c::ASL, &c::ZeroPageX, 2, 6, },
        { 0x17, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x18, "CLC", &c::CLC, &c::Implied  , 1, 2, },
        { 0x19, "ORA", &c::ORA, &c::AbsoluteY, 3, 4, },
        { 0x1A, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x1B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x1C, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x1D, "ORA", &c::ORA, &c::AbsoluteX, 3, 4, },
        { 0x1E, "ASL", &c::ASL, &c::AbsoluteX, 3, 7, },
        { 0x1F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x20, "JSR", &c::JSR, &c::Absolute , 3, 6, },
        { 0x21, "AND", &c::AND, &c::IndirectX, 2, 6, },
        { 0x22, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x23, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x24, "BIT", &c::BIT, &c::ZeroPage , 2, 3, },
        { 0x25, "AND", &c::AND, &c::ZeroPage , 2, 3, },
        { 0x26, "ROL", &c::ROL, &c::ZeroPage , 2, 5, },
        { 0x27, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x28, "PLP", &c::PLP, &c::Implied  , 1, 4, },
        { 0x29, "AND", &c::AND, &c::Immediate, 2, 2, },
        { 0x2A, "ROL", &c::ROL, &c::Implied,   1, 2, },
        { 0x2B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x2C, "BIT", &c::BIT, &c::Absolute , 3, 4, },
        { 0x2D, "AND", &c::AND, &c::Absolute , 3, 4, },
        { 0x2E, "ROL", &c::ROL, &c::Absolute , 3, 6, },
        { 0x2F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x30, "BMI", &c::BMI, &c::Relative , 2, 2, },
        { 0x31, "AND", &c::AND, &c::IndirectY, 2, 5, },
        { 0x32, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x33, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x34, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x35, "AND", &c::AND, &c::ZeroPageX, 2, 4, },
        { 0x36, "ROL", &c::ROL, &c::ZeroPageX, 2, 6, },
        { 0x37, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x38, "SEC", &c::SEC, &c::Implied  , 1, 2, },
        { 0x39, "AND", &c::AND, &c::AbsoluteY, 3, 4, },
        { 0x3A, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x3B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x3C, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x3D, "AND", &c::AND, &c::AbsoluteX, 3, 4, },
        { 0x3E, "ROL", &c::ROL, &c::AbsoluteX, 3, 7, },
        { 0x3F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        // NOT CHECKED: (antageligvis korrekt stort sett)
        { 0x40, "RTI", &c::RTI, &c::Implied  , 1, 6, },
        { 0x41, "EOR", &c::EOR, &c::IndirectX, 2, 6, },
        { 0x42, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x43, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x44, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x45, "EOR", &c::EOR, &c::ZeroPage , 2, 3, },
        { 0x46, "LSR", &c::LSR, &c::ZeroPage , 2, 5, },
        { 0x47, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x48, "PHA", &c::PHA, &c::Implied  , 1, 3, },
        { 0x49, "EOR", &c::EOR, &c::Immediate, 2, 2, },
        { 0x4A, "LSR", &c::LSR, &c::Implied,   1, 2, },
        { 0x4B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x4C, "JMP", &c::JMP, &c::Absolute , 3, 3, },
        { 0x4D, "EOR", &c::EOR, &c::Absolute , 3, 4, },
        { 0x4E, "LSR", &c::LSR, &c::Absolute , 3, 6, },
        { 0x4F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x50, "BVC", &c::BVC, &c::Relative , 2, 3, },
        { 0x51, "EOR", &c::EOR, &c::IndirectY, 2, 5, },
        { 0x52, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x53, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x54, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x55, "EOR", &c::EOR, &c::ZeroPageX, 2, 4, },
        { 0x56, "LSR", &c::LSR, &c::ZeroPageX, 2, 6, },
        { 0x57, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x58, "CLI", &c::CLI, &c::Implied  , 1, 2, },
        { 0x59, "EOR", &c::EOR, &c::AbsoluteY, 3, 4, },
        { 0x5A, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x5B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x5C, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x5D, "EOR", &c::EOR, &c::AbsoluteX, 3, 4, },
        { 0x5E, "LSR", &c::LSR, &c::AbsoluteX, 3, 7, },
        { 0x5F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x60, "RTS", &c::RTS, &c::Implied  , 1, 6, },
        { 0x61, "ADC", &c::ADC, &c::IndirectX, 2, 6, },
        { 0x62, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x63, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x64, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x65, "ADC", &c::ADC, &c::ZeroPage,  2, 3, },
        { 0x66, "ROR", &c::ROR, &c::ZeroPage , 2, 5, },
        { 0x67, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x68, "PLA", &c::PLA, &c::Implied  , 1, 4, },
        { 0x69, "ADC", &c::ADC, &c::Immediate, 2, 2, },
        { 0x6A, "ROR", &c::ROR, &c::Implied,   1, 2, },
        { 0x6B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x6C, "JMP", &c::JMP, &c::Indirect , 3, 5, },
        { 0x6D, "ADC", &c::ADC, &c::Absolute , 3, 4, },
        { 0x6E, "ROR", &c::ROR, &c::Absolute , 3, 6, },
        { 0x6F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x70, "BVS", &c::BVS, &c::Relative , 2, 3, },
        { 0x71, "ADC", &c::ADC, &c::IndirectY, 2, 5, },
        { 0x72, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x73, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x74, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x75, "ADC", &c::ADC, &c::ZeroPageX, 2, 4, },
        { 0x76, "ROR", &c::ROR, &c::ZeroPageX, 2, 6, },
        { 0x77, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x78, "SEI", &c::SEI, &c::Implied  , 1, 2, },
        { 0x79, "ADC", &c::ADC, &c::AbsoluteY, 3, 4, },
        { 0x7A, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x7B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x7C, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x7D, "ADC", &c::ADC, &c::AbsoluteX, 3, 4, },
        { 0x7E, "ROR", &c::ROR, &c::AbsoluteX, 3, 7, },
        { 0x7F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x80, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x81, "STA", &c::STA, &c::IndirectX, 2, 6, },
        { 0x82, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x83, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x84, "STY", &c::STY, &c::ZeroPage , 2, 3, },
        { 0x85, "STA", &c::STA, &c::ZeroPage , 2, 3, },
        { 0x86, "STX", &c::STX, &c::ZeroPage , 2, 3, },
        { 0x87, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x88, "DEY", &c::DEY, &c::Implied  , 1, 2, },
        { 0x89, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x8A, "TXA", &c::TXA, &c::Implied  , 1, 2, },
        { 0x8B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x8C, "STY", &c::STY, &c::Absolute , 3, 4, },
        { 0x8D, "STA", &c::STA, &c::Absolute , 3, 4, },
        { 0x8E, "STX", &c::STX, &c::Absolute , 3, 4, },
        { 0x8F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x90, "BCC", &c::BCC, &c::Relative , 2, 3, },
        { 0x91, "STA", &c::STA, &c::IndirectY, 2, 6, },
        { 0x92, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x93, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x94, "STY", &c::STY, &c::ZeroPageY, 2, 4, },
        { 0x95, "STA", &c::STA, &c::ZeroPageX, 2, 4, },
        { 0x96, "STX", &c::STX, &c::ZeroPageY, 2, 4, },
        { 0x97, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x98, "TYA", &c::TYA, &c::Implied  , 1, 2, },
        { 0x99, "STA", &c::STA, &c::AbsoluteY, 3, 5, },
        { 0x9A, "TXS", &c::TXS, &c::Implied  , 1, 2, },
        { 0x9B, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x9C, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x9D, "STA", &c::STA, &c::AbsoluteX, 3, 5, },
        { 0x9E, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0x9F, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xA0, "LDY", &c::LDY, &c::Immediate, 2, 2, },
        { 0xA1, "LDA", &c::LDA, &c::IndirectX, 2, 6, },
        { 0xA2, "LDX", &c::LDX, &c::Immediate, 2, 2, },
        { 0xA3, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xA4, "LDY", &c::LDY, &c::ZeroPage , 2, 3, },
        { 0xA5, "LDA", &c::LDA, &c::ZeroPage , 2, 3, },
        { 0xA6, "LDX", &c::LDX, &c::ZeroPage , 2, 3, },
        { 0xA7, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xA8, "TAY", &c::TAY, &c::Implied  , 1, 2, },
        { 0xA9, "LDA", &c::LDA, &c::Immediate, 2, 2, },
        { 0xAA, "TAX", &c::TAX, &c::Implied  , 1, 2, },
        { 0xAB, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xAC, "LDY", &c::LDY, &c::Absolute , 3, 4, },
        { 0xAD, "LDA", &c::LDA, &c::Absolute , 3, 4, },
        { 0xAE, "LDX", &c::LDX, &c::Absolute , 3, 4, },
        { 0xAF, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xB0, "BCS", &c::BCS, &c::Relative , 2, 3, },
        { 0xB1, "LDA", &c::LDA, &c::IndirectY, 2, 5, },
        { 0xB2, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xB3, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xB4, "LDY", &c::LDY, &c::ZeroPageX, 2, 4, },
        { 0xB5, "LDA", &c::LDA, &c::ZeroPageX, 2, 4, },
        { 0xB6, "LDX", &c::LDX, &c::ZeroPageY, 2, 4, },
        { 0xB7, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xB8, "CLV", &c::CLV, &c::Implied  , 1, 2, },
        { 0xB9, "LDA", &c::LDA, &c::AbsoluteY, 3, 4, },
        { 0xBA, "TSX", &c::TSX, &c::Implied  , 1, 2, },
        { 0xBB, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xBC, "LDY", &c::LDY, &c::AbsoluteX, 3, 4, },
        { 0xBD, "LDA", &c::LDA, &c::AbsoluteX, 3, 4, },
        { 0xBE, "LDX", &c::LDX, &c::AbsoluteY, 3, 4, },
        { 0xBF, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xC0, "CPY", &c::CPY, &c::Immediate, 2, 2, },
        { 0xC1, "CMP", &c::CMP, &c::IndirectX, 2, 6, },
        { 0xC2, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xC3, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xC4, "CPY", &c::CPY, &c::ZeroPage , 2, 3, },
        { 0xC5, "CMP", &c::CMP, &c::ZeroPage , 2, 3, },
        { 0xC6, "DEC", &c::DEC, &c::ZeroPage , 2, 5, },
        { 0xC7, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xC8, "INY", &c::INY, &c::Implied  , 1, 2, },
        { 0xC9, "CMP", &c::CMP, &c::Immediate, 2, 2, },
        { 0xCA, "DEX", &c::DEX, &c::Implied  , 1, 2, },
        { 0xCB, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xCC, "CPY", &c::CPY, &c::Absolute , 3, 4, },
        { 0xCD, "CMP", &c::CMP, &c::Absolute , 3, 4, },
        { 0xCE, "DEC", &c::DEC, &c::Absolute , 3, 6, },
        { 0xCF, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xD0, "BNE", &c::BNE, &c::Relative , 2, 3, },
        { 0xD1, "CMP", &c::CMP, &c::IndirectY, 2, 5, },
        { 0xD2, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xD3, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xD4, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xD5, "CMP", &c::CMP, &c::ZeroPageX, 2, 4, },
        { 0xD6, "DEC", &c::DEC, &c::ZeroPageX, 2, 6, },
        { 0xD7, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xD8, "CLD", &c::CLD, &c::Implied  , 1, 2, },
        { 0xD9, "CMP", &c::CMP, &c::AbsoluteY, 3, 4, },
        { 0xDA, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xDB, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xDC, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xDD, "CMP", &c::CMP, &c::AbsoluteX, 3, 4, },
        { 0xDE, "DEC", &c::DEC, &c::AbsoluteX, 3, 7, },
        { 0xDF, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xE0, "CPX", &c::CPX, &c::Immediate, 2, 2, },
        { 0xE1, "SBC", &c::SBC, &c::IndirectX, 2, 6, },
        { 0xE2, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xE3, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xE4, "CPX", &c::CPX, &c::ZeroPage , 2, 3, },
        { 0xE5, "SBC", &c::SBC, &c::ZeroPage , 2, 3, },
        { 0xE6, "INC", &c::INC, &c::ZeroPage , 2, 5, },
        { 0xE7, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xE8, "INX", &c::INX, &c::Implied  , 1, 2, },
        { 0xE9, "SBC", &c::SBC, &c::Immediate, 2, 2, },
        { 0xEA, "NOP", &c::NOP, &c::Implied  , 1, 2, },
        { 0xEB, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xEC, "CPX", &c::CPX, &c::Absolute , 3, 4, },
        { 0xED, "SBC", &c::SBC, &c::Absolute , 3, 4, },
        { 0xEE, "INC", &c::INC, &c::Absolute , 3, 6, },
        { 0xEF, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xF0, "BEQ", &c::BEQ, &c::Relative , 2, 3, },
        { 0xF1, "SBC", &c::SBC, &c::IndirectY, 2, 5, },
        { 0xF2, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xF3, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xF4, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xF5, "SBC", &c::SBC, &c::ZeroPageX, 2, 4, },
        { 0xF6, "INC", &c::INC, &c::ZeroPageX, 2, 6, },
        { 0xF7, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xF8, "SED", &c::SED, &c::Implied  , 1, 2, },
        { 0xF9, "SBC", &c::SBC, &c::AbsoluteY, 3, 4, },
        { 0xFA, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xFB, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xFC, "XXX", &c::XXX, &c::Implied  , 1, 2, },
        { 0xFD, "SBC", &c::SBC, &c::AbsoluteX, 3, 4, },
        { 0xFE, "INC", &c::INC, &c::AbsoluteX, 3, 7, },
        { 0xFF, "XXX", &c::XXX, &c::Implied  , 1, 2, },
    };
}

CPU::~CPU()
{
}

void CPU::write(uint16_t addr, uint8_t data)
{
    bus->write(addr, data);
}

uint8_t CPU::read(uint16_t addr)
{
    return bus->read(addr, false);
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
uint8_t CPU::pop()
{
    return read(++sp);
}

// pop 16 bits from the stack
uint16_t CPU::pop16()
{
    return (pop() | (pop() << 8));
}

bool CPU::GetFlag(Flag f)
{
    return (flags & f);
}

void CPU::SetFlag(Flag f, bool v)
{
    if (v)
        flags |= f;
    else
        flags &= ~f;
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

bool CPU::complete()
{
    return (cycles == 0);
}

void CPU::clock()
{
    if (cycles == 0) {
        // We are ready for the next instruction!
        // Read next opcode:
        opcode = read(pc);

        // Unused flag should always be true
        SetFlag(U);

        // Upgrade your PC! 
        pc++;

        // How many cycles are we doing today?
        cycles = lookup[opcode].cycles;

        int add1 = (this->*lookup[opcode].addrmode)();
        int add2 = (this->*lookup[opcode].operate)();

        cycles += (add1 & add2);
    }

    total_cycles++;
    cycles--;
}

uint8_t CPU::fetch()
{
    // only read if needed
    if (!(lookup[opcode].addrmode == &CPU::Implied))
        operand = read(address);

    return operand;
}

// Addressing modes
// These should set up stuff for use later...
uint8_t CPU::Implied()
{
    operand = a;
    return 0;
}

uint8_t CPU::Immediate()
{
    // ++ postfix means:
    // address = pc; pc += 1;
    address = pc++;
    return 0;
}

uint8_t CPU::Relative()
{
    address_rel = read(pc);
    pc++;
    if (address_rel & 0b10000000)
        address_rel |= 0xFF00;
    return 0;
}

uint8_t CPU::ZeroPage()
{
    address = (read(pc) & 0x00FF);
    pc++;
    return 0;
}

uint8_t CPU::ZeroPageX()
{
    address = (read(pc) + x) & 0x00FF;
    pc++;
    return 0;
}

uint8_t CPU::ZeroPageY()
{
    address = (read(pc) + y) & 0x00FF;
    pc++;
    return 0;
}

uint8_t CPU::Absolute()
{
    address = read(pc);
    pc++;
    address |= (read(pc) << 8);
    pc++;
    return 0;
}

uint8_t CPU::AbsoluteX()
{
    address = read(pc);
    pc++;
    temp = read(pc);            // hi 8 bits
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
    temp = read(pc);            // hi 8 bits
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
    temp = 0x0000 | read(pc);
    pc++;
    temp += x;

    address = read(temp & 0x00FF) | (read((temp + 1) & 0x00FF) << 8);

    return 0;
}

uint8_t CPU::IndirectY()
{
    temp = read(pc);
    pc++;

    uint16_t hi = read((temp + 1) & 0x00FF);
    address = ((hi << 8) | read(temp & 0x00FF)) + y;

    if ((address & 0xFF00) != (hi << 8))
        return 1;
    else
        return 0;
}



////////////////
// Operations //
////////////////
uint8_t CPU::NOP()
{
    // TODO: add all unofficial NOPs
    return 0;
}

uint8_t CPU::XXX()
{
    return 0;
}

// Like a lot in this project, this is mainly taken from OLC
uint8_t CPU::ADC()
{
    fetch();
    temp = (uint16_t) a + (uint16_t) operand + (uint16_t) GetFlag(C);
    SetFlag(C, temp > 255);
    SetFlag(Z, (temp & 0x00FF) == 0);
    // I have no idea what's going on here lol
    SetFlag(V, (~((uint16_t) a ^ (uint16_t) operand) & ((uint16_t) a ^ (uint16_t) temp)) & 0x0080);
    SetFlag(N, temp & 0x80);
    a = temp & 0x00FF;
    return 1;
}

uint8_t CPU::AND()
{
    // Fetch the operand (M)
    fetch();

    // OPERATE
    a = a & operand;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0b10000000);

    return 1;
}

uint8_t CPU::ASL()
{
    fetch();

    temp = (uint16_t) operand << 1;
    SetFlag(C, operand & 0b10000000);
    // litt usikker på Z her...
    SetFlag(Z, (temp & 0x00FF) == 0x00);
    SetFlag(N, (temp & 0x0080));

    if (lookup[opcode].addrmode == &CPU::Implied)
        a = temp & 0x00FF;
    else
        write(address, temp & 0x00FF);

    return 0;
}

uint8_t CPU::BCC()
{
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

    return 0;
}

uint8_t CPU::BCS()
{
    if (GetFlag(C)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BEQ()
{
    if (GetFlag(Z)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BIT()
{
    fetch();
    SetFlag(N, operand & 0b10000000);
    SetFlag(V, operand & 0b01000000);
    SetFlag(Z, !(a & operand));

    return 0;
}

uint8_t CPU::BMI()
{
    if (GetFlag(N)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BNE()
{
    if (!GetFlag(Z)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BPL()
{
    if (!GetFlag(N)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BVC()
{
    if (!GetFlag(V)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BVS()
{
    if (GetFlag(V)) {
        cycles++;
        // Set address to branch to
        address = pc + address_rel;
        // Check for page boundary crossed
        if ((address & 0xFF00) != (pc & 0xFF00))
            cycles++;

        pc = address;
    }

    return 0;
}

uint8_t CPU::BRK()
{
    pc++;

    SetFlag(B, true);
    push16(pc);
    push(flags);
    pc = (((uint16_t)read(0xFFFF) << 8) | (uint16_t)read(0xFFFE));

    return 0;
}

uint8_t CPU::CLC()
{
    SetFlag(C, false);
    return 0;
}

uint8_t CPU::CLD()
{
    SetFlag(D, false);
    return 0;
}

uint8_t CPU::CLI()
{
    SetFlag(I, false);
    return 0;
}

uint8_t CPU::CLV()
{
    SetFlag(V, false);
    return 0;
}

// TODO: cast til 16-bit?
uint8_t CPU::CMP()
{
    fetch();
    SetFlag(C, a >= operand);
    SetFlag(Z, (a - operand) == 0x00);
    SetFlag(N, (a - operand) & 0b10000000);
    return 1;
}

uint8_t CPU::CPX()
{
    fetch();
    SetFlag(C, x >= operand);
    SetFlag(Z, (x - operand) == 0x00);
    SetFlag(N, (x - operand) & 0b10000000);
    return 1;
}

uint8_t CPU::CPY()
{
    fetch();
    SetFlag(C, y >= operand);
    SetFlag(Z, (y - operand) == 0x00);
    SetFlag(N, (y - operand) & 0b10000000);
    return 1;
}

uint8_t CPU::DEC()
{
    fetch();
    temp = operand - 1;
    SetFlag(Z, temp == 0x00);
    SetFlag(N, temp & 0b10000000);
    write(address, temp);
    return 0;
}

uint8_t CPU::DEX()
{
    x--;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0b10000000);
    return 0;
}

uint8_t CPU::DEY()
{
    y--;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0b10000000);
    return 0;
}

uint8_t CPU::EOR()
{
    fetch();
    a = a ^ operand;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0b10000000);
    return 1;
}

uint8_t CPU::INC()
{
    fetch();
    temp = operand + 1;
    SetFlag(Z, temp == 0x00);
    SetFlag(N, temp & 0b10000000);
    write(address, temp);
    return 0;
}

uint8_t CPU::INX()
{
    x++;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0b10000000);
    return 0;
}

uint8_t CPU::INY()
{
    y++;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0b10000000);
    return 0;
}

uint8_t CPU::JMP()
{
    pc = address;
    return 0;
}

uint8_t CPU::JSR()
{
    pc--;
    push16(pc);
    pc = address;
    return 0;
}

uint8_t CPU::LDA()
{
    fetch();
    a = operand;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0b10000000);
    return 1;
}

uint8_t CPU::LDX()
{
    fetch();
    x = operand;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0b10000000);
    return 1;
}

uint8_t CPU::LDY()
{
    fetch();
    y = operand;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0b10000000);
    return 1;
}

uint8_t CPU::LSR()
{
    fetch();

    temp = (uint16_t) operand >> 1;
    SetFlag(C, operand & 0b00000001);
    // litt usikker på Z her...
    SetFlag(Z, (temp & 0x00FF) == 0x00);
    SetFlag(N, (temp & 0x0080));

    if (lookup[opcode].addrmode == &CPU::Implied)
        a = temp & 0x00FF;
    else
        write(address, temp & 0x00FF);

    return 0;
}

uint8_t CPU::ORA()
{
    fetch();
    a = a | operand;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0b10000000);
    return 1;
}

// Push A
uint8_t CPU::PHA()
{
    push(a);
    return 0;
}

// Push flags/status
uint8_t CPU::PHP()
{
    // apparently PHP always pushes B as '1' (but doesn't actually set it)
    push(flags | (1 << 4));
    return 0;
}

uint8_t CPU::PLA()
{
    a = pop();
    SetFlag(Z, a == 0x00);
    SetFlag(N, a  & 0x80);
    return 0;
}

uint8_t CPU::PLP()
{
    flags = pop();
    SetFlag(U);
    return 0;
}

uint8_t CPU::ROL()
{
    fetch();
    temp = (uint16_t) (operand << 1) | GetFlag(C);
    SetFlag(C, temp & 0x100);
    SetFlag(Z, (temp & 0x00FF) == 0x00);
    SetFlag(N, temp & 0x0080);
    if (lookup[opcode].addrmode == &CPU::Implied)
        a = temp & 0x00FF;
    else
        write(address, temp & 0x00FF);

    return 0;
}

uint8_t CPU::ROR()
{
    fetch();
    temp = (uint16_t)(GetFlag(C) << 7) | (operand >> 1);
    SetFlag(C, operand & 0x01);
    SetFlag(Z, (temp & 0x00FF) == 0x00);
    SetFlag(N, temp & 0x0080);
    if (lookup[opcode].addrmode == &CPU::Implied)
        a = temp & 0x00FF;
    else
        write(address, temp & 0x00FF);

    return 0;
}

uint8_t CPU::RTI()
{
    // TODO: set/clear B+U flags??
    flags = pop();
    pc = pop16();
    return 0;
}

uint8_t CPU::RTS()
{
    pc = pop16() + 1;
    return 0;
}

// Taken from OLC. I haven't made an effort to fully understand it...
uint8_t CPU::SBC()
{
    fetch();

    // Invert the bottom 8 bits (makes the operand negative)
    uint16_t value = ((uint16_t) operand) ^ 0x00FF;
    // After that it's exactly the same as ADC ?!
    // (adding a negative number is equal to subtracting the positive equivalent of that number)
    temp = (uint16_t) a + (uint16_t) operand + (uint16_t) GetFlag(C);
    SetFlag(C, temp > 255);
    SetFlag(Z, (temp & 0x00FF) == 0);
    // I have no idea what's going on here lol
    SetFlag(V, (~((uint16_t) a ^ (uint16_t) operand) & ((uint16_t) a ^ (uint16_t) temp)) & 0x0080);
    SetFlag(N, temp & 0x80);
    a = temp & 0x00FF;
    
    return 1;
}

uint8_t CPU::SEC()
{
    SetFlag(C);
    return 0;
}

uint8_t CPU::SED()
{
    SetFlag(D);
    return 0;
}

uint8_t CPU::SEI()
{
    SetFlag(I);
    return 0;
}

uint8_t CPU::STA()
{
    write(address, a);
    return 0;
}

uint8_t CPU::STX()
{
    write(address, x);
    return 0;
}

uint8_t CPU::STY()
{
    write(address, y);
    return 0;
}

uint8_t CPU::TAX()
{
    x = a;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x  & 0x80);
    return 0;
}

uint8_t CPU::TAY()
{
    y = a;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y  & 0x80);
    return 0;
}

uint8_t CPU::TSX()
{
    x = sp;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x  & 0x80);
    return 0;
}

uint8_t CPU::TXA()
{
    a = x;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a  & 0x80);
    return 0;
}

uint8_t CPU::TXS()
{
    sp = x;
    return 0;
}

uint8_t CPU::TYA()
{
    a = y;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a  & 0x80);
    return 0;
}


// Disassembler
// Heavily based on OLC's work

std::map<uint16_t, std::string> CPU::disassemble(uint16_t start, uint16_t end)
{
    uint32_t addr = start;
    uint16_t line = 0;
    uint8_t value = 0, lo = 0, hi = 0;
    std::map<uint16_t, std::string> lines;

    auto hex = [](uint32_t n, uint8_t d)
    {
        std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
    };

    while (addr <= (uint32_t) end) {
        line = addr;
        std::string inst = "$" + hex(addr, 4) + ": ";
        uint8_t opcode = bus->read(addr, true);
        addr++;
        inst += lookup[opcode].mnemonic + " ";

        if (lookup[opcode].addrmode == &CPU::Implied) {
            inst += " ";
        } else if (lookup[opcode].addrmode == &CPU::Immediate) {
            value = bus->read(addr, true);
            addr++;
            inst += "#$"+ hex(value, 2);
        } else if (lookup[opcode].addrmode == &CPU::ZeroPage) {
            lo = bus->read(addr, true);
            addr++;
            inst += "$" + hex(lo, 2);
        } else if (lookup[opcode].addrmode == &CPU::ZeroPageX) {
            lo = bus->read(addr, true);
            addr++;
            inst += "$" + hex(lo, 2) + ", X";
        } else if (lookup[opcode].addrmode == &CPU::ZeroPageY) {
            lo = bus->read(addr, true);
            addr++;
            inst += "$" + hex(lo, 2) + ", Y";
        } else if (lookup[opcode].addrmode == &CPU::IndirectX) {
            lo = bus->read(addr, true);
            addr++;
            inst += "($" + hex(lo, 2) + ", X)";
        } else if (lookup[opcode].addrmode == &CPU::IndirectY) {
            lo = bus->read(addr, true);
            addr++;
            inst += "($" + hex(lo, 2) + "), Y";
        } else if (lookup[opcode].addrmode == &CPU::Absolute) {
            lo = bus->read(addr, true); addr++;
            hi = bus->read(addr, true); addr++;
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4);
        } else if (lookup[opcode].addrmode == &CPU::AbsoluteX) {
            lo = bus->read(addr, true); addr++;
            hi = bus->read(addr, true); addr++;
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X";
        } else if (lookup[opcode].addrmode == &CPU::AbsoluteY) {
            lo = bus->read(addr, true); addr++;
            hi = bus->read(addr, true); addr++;
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y";
        } else if (lookup[opcode].addrmode == &CPU::Indirect) {
            lo = bus->read(addr, true); addr++;
            hi = bus->read(addr, true); addr++;
            inst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ")";
        } else if (lookup[opcode].addrmode == &CPU::Relative) {
            value = bus->read(addr, true);
            addr++;
            inst += "$" + hex(value, 2) + " [$" + hex(addr + value, 4) + "]";
        } else {
            inst += "ERROR: Unknown addressing mode!";
        }

        lines[line] = inst;
    }

    return lines;
}















































// prerequisite:
// RAM 0x00 - 0xFF = 0x00 - 0xFF ......
void CPU::TestOpcodes()
{
    opcode = read(pc);
    printf("PC: %04x - ", pc);
    if (opcode == lookup[opcode].opcode) {
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

