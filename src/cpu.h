
#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <bitset>

class Bus;

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

        // Program Counter
        uint16_t pc    = 0x0000;
        // Registers
        uint8_t  a     = 0x00;
        uint8_t  x     = 0x00;
        uint8_t  y     = 0x00;
        uint8_t  flags = 0x00;      // aka processor status - use c++ bitfield?
        uint8_t  sp    = 0x00;      // stack pointer

        uint64_t total_cycles = 0;

        void ConnectBus(Bus *n) { bus = n; }
        void clock();
        void TestOpcodes();

        enum Flag {
            C = (1 << 0),   // Carry
            Z = (1 << 1),   // Zero
            I = (1 << 2),   // Disable Interrupts
            D = (1 << 3),   // Decimal Mode
            B = (1 << 4),   // Break
            U = (1 << 5),   // Unused
            V = (1 << 6),   // oVerflow
            N = (1 << 7),   // Negative
        };

        bool GetFlag(Flag f);
        void SetFlag(Flag f, bool v = true);

        void reset();
        void irq();
        void nmi();
        void interrupt(uint16_t addr);
        bool complete();

        std::map<uint16_t, std::string> disassemble(uint16_t start, uint16_t end);

    private:
        Bus *bus = nullptr;
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
        
        uint8_t fetch();


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
        uint8_t Implied();   uint8_t Immediate();
        uint8_t ZeroPage();  uint8_t ZeroPageX();
        uint8_t Relative();  uint8_t ZeroPageY();
        uint8_t Absolute();  uint8_t AbsoluteX();
        uint8_t Indirect();  uint8_t AbsoluteY();
        uint8_t IndirectX(); uint8_t IndirectY();

        // Utility functions for getting effective address for indexed/indirect addressing modes
        // These are used in the disassembler
        uint16_t GetAddrZPX(uint16_t addr); uint16_t GetAddrZPY(uint16_t addr);
        uint16_t GetAddrIDX(uint16_t addr); uint16_t GetAddrIDY(uint16_t addr);

        // Utility functions
        void push(uint8_t data, uint16_t offset = 0x100);
        void push16(uint16_t data);
        uint8_t pop();
        uint16_t pop16();
        
        // Operations
        // NOP = NOP // XXX = undefined/illegal opcode
        uint8_t NOP(); uint8_t XXX();
        uint8_t ADC(); uint8_t AND(); uint8_t ASL();
        uint8_t BCC(); uint8_t BCS(); uint8_t BEQ(); uint8_t BMI();
        uint8_t BNE(); uint8_t BPL(); uint8_t BVC(); uint8_t BVS();
        uint8_t BIT(); uint8_t BRK();
        uint8_t CLC(); uint8_t CLD(); uint8_t CLI(); uint8_t CLV();
        uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
        uint8_t DEC(); uint8_t DEX(); uint8_t DEY();
        uint8_t EOR();
        uint8_t INC(); uint8_t INX(); uint8_t INY();
        uint8_t JMP(); uint8_t JSR();
        uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
        uint8_t LSR(); uint8_t ORA();
        uint8_t PHA(); uint8_t PHP(); uint8_t PLA(); uint8_t PLP();
        uint8_t ROL(); uint8_t ROR(); uint8_t RTI(); uint8_t RTS();
        uint8_t SBC(); uint8_t SEC(); uint8_t SED(); uint8_t SEI();
        uint8_t STA(); uint8_t STX(); uint8_t STY();
        uint8_t TAX(); uint8_t TAY(); uint8_t TSX();
        uint8_t TXA(); uint8_t TXS(); uint8_t TYA();
};
