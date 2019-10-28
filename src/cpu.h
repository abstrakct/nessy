
#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <bitset>

class Machine;

//enum eAddressingModes {
//    Immediate,
//    Implied,
//    Absolute,
//    AbsoluteX,
//    AbsoluteY,
//    ZeroPage,
//    ZeroPageX,
//    ZeroPageY,
//    Indirect,
//    IndirectX,
//    IndirectY,
//    Relative,
//    Accumulator,
//};



//class Memory {
//    private:
//        std::vector<uint8_t> ram;
//        std::vector<uint8_t> rom;
//    public:
//        uint8_t read(uint16_t address);
//        void write(uint16_t address, uint8_t value);
//        void resizeRom(int size);
//
//        Memory() : ram(0x800, 0), rom(0x10000, 0) {};
//        ~Memory() { }
//};

class CPU {
    public:
        CPU();
        ~CPU();

        const uint8_t C = 0x01;
        const uint8_t Z = 0x02;
        const uint8_t I = 0x04;
        const uint8_t D = 0x08;
        const uint8_t B = 0x10;
        const uint8_t U = 0x20;
        const uint8_t V = 0x40;
        const uint8_t N = 0x80;

        // Program Counter
        uint16_t pc    = 0x0000;
        // Registers
        uint8_t  a     = 0x00;
        uint8_t  x     = 0x00;
        uint8_t  y     = 0x00;
        uint8_t  flags = 0x00;      // aka processor status
        uint8_t  sp    = 0x00;      // stack pointer

        uint64_t total_cycles = 0;

        void ConnectMachine(Machine *n) { nes = n; }
        void clock();
        void TestOpcodes();

        inline bool GetFlag(int f) {
            return ((flags & f) > 0) ? 1 : 0;
            //return (flags & f);
        }
        inline void SetFlag(int f, bool v = true) {
            if (v)
                flags |= f;
            else
                flags &= ~(f);
        }

        void reset();
        void irq();
        void nmi();
        void interrupt(uint16_t addr);
        bool complete();

        std::map<uint16_t, std::string> disassemble(uint16_t start, uint16_t end);

    private:
        Machine *nes = nullptr;
        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr);

        struct Opcode {
                uint8_t opcode;
                std::string mnemonic;
                uint8_t (CPU::*operate) (void) = nullptr;
                uint8_t (CPU::*addrmode)(void) = nullptr;
                uint8_t bytes, cycles;
        };
        std::vector<Opcode> lookup;
        uint8_t opcode, operand;
        uint8_t cycles = 0;
        uint16_t address = 0;
        uint16_t address_rel = 0;
        uint16_t temp = 0;
        
        inline uint8_t fetch() {
            // only read if needed
            if (!(lookup[opcode].addrmode == &CPU::Implied))
                operand = read(address);

            return operand;
        }


        // Adressing Modes
        // (partially stolen from OneLoneCoder)
        // Implied: no operand
        // Immediate: operand is the value for the operation
        // ZeroPage: fetches value from 8-bit address on the zero page
        // ZeroPageX: value = operand + x (on zero page)
        // ZeroPageY: value = operand + y (on zero page)
        // Relative: 8-bit signed offset relative to current PC (for branching)
        // Absolute: fetches value from 16-bit address anywhere in memory
        // AbsoluteX: fetches value from operand + X
        // AbsoluteY: fetches value from operand + Y
        // Indirect: JMP instruction has a special addrmode that can jump to the address
        //           stored in a 16-bit pointer anywhere in memory
        // IndirectX: madness
        // IndirectY: more madness
        inline uint8_t Implied() {
            operand = a;
            return 0;
        }

        inline uint8_t Immediate() {
            address = pc++;
            return 0;
        }

        inline uint8_t ZeroPage() {
            address = (read(pc) & 0x00FF);
            pc++;
            return 0;
        }
        
        inline uint8_t ZeroPageX() {
            address = (read(pc) + x) & 0x00FF;
            pc++;
            return 0;
        }

        inline uint8_t ZeroPageY() {
            address = (read(pc) + y) & 0x00FF;
            pc++;
            return 0;
        }

        inline uint8_t Absolute() {
            address = (uint16_t) read(pc++);
            address |= (uint16_t) (read(pc++) << 8);
            return 0;
        }

        uint8_t Relative();  uint8_t AbsoluteX();
        uint8_t Indirect();  uint8_t AbsoluteY();
        uint8_t IndirectX(); uint8_t IndirectY();

        // Utility functions for getting effective address for indexed/indirect addressing modes
        // These are used in the disassembler
        uint16_t GetAddrZP (uint16_t addr);
        uint16_t GetAddrZPX(uint16_t addr); uint16_t GetAddrZPY(uint16_t addr);
        uint16_t GetAddrIDX(uint16_t addr); uint16_t GetAddrIDY(uint16_t addr);

        // Utility functions
        void push(uint8_t data, uint16_t offset = 0x100);
        void push16(uint16_t data);
        uint8_t pop(uint16_t offset = 0x100);
        uint16_t pop16();
};
