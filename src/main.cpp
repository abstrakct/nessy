/*
 * NESSY - an NES emulator/disassembler/debugger
 *
 * Yeah, that ambitious.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <bitset>
#include <chrono>
#include <thread>

#include "version.h"
#include "logger.h"
#include "machine.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;

// this is silly, move into Nessy class?!
std::shared_ptr<Machine> TheNES;
Logger l;

// from OLC
std::string hex(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
};

// u8 -> binary string
std::string bin(uint8_t n)
{
    std::bitset<8> b = n;
    return b.to_string();
}

class Nessy : public olc::PixelGameEngine
{
    public:
        std::shared_ptr<Machine> nes;
        std::map<uint16_t, std::string> disasm;
        std::string nesFilename;
        uint16_t ram2start = 0x8000;
        bool debugmode, runmode = false;
        int execspeed = 100;
        float residualTime = 0.0f, targetFPS = 60.0f;
        char log[100];

        std::shared_ptr<Cartridge> cart;

        Nessy(std::shared_ptr<Machine> m, std::string filename, bool d = false)
        { 
            sAppName = "Nessy"; 
            nes = m;
            debugmode = d;
            nesFilename = filename;
        }

        void DrawRAM(int x, int y, uint16_t addr, int rows, int cols)
        {
            // TODO: highlight current PC (også read/writes????!!!!) !!!
            int ramx = x, ramy = y;
            for (int row = 0; row < rows; row++) {
                std::string s = "$" + hex(addr, 4) + ":";
                for (int col = 0; col < cols; col++) {
                    s += " " + hex(nes->cpuRead(addr, true), 2);
                    addr++;
                }
                //if (addr == nes->cpu->pc) {
                //    DrawString(ramx, ramy, s, olc::GREEN);
                //} else {
                    DrawString(ramx, ramy, s);
                //}
                ramy += 10;
            }
        }

        void DrawCPU(int x, int y)
        {
            //std::string s = "CPU: ";
            
            int xs = 32;
            int space = 8;
            DrawString(x, y, "CPU:", olc::WHITE);
            DrawString(x + (xs + space * 1), y, "N", nes->cpu.GetFlag(CPU::N) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 2), y, "V", nes->cpu.GetFlag(CPU::V) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 3), y, "-", nes->cpu.GetFlag(CPU::U) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 4), y, "B", nes->cpu.GetFlag(CPU::B) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 5), y, "D", nes->cpu.GetFlag(CPU::D) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 6), y, "I", nes->cpu.GetFlag(CPU::I) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 7), y, "Z", nes->cpu.GetFlag(CPU::Z) ? olc::GREEN : olc::RED);
            DrawString(x + (xs + space * 8), y, "C", nes->cpu.GetFlag(CPU::C) ? olc::GREEN : olc::RED);
            DrawString(x, y + 10, " PC: $" + hex(nes->cpu.pc, 4));
            DrawString(x, y + 20, " SP: $" + hex(nes->cpu.sp, 4));
            DrawString(x, y + 30, "  A: $" + hex(nes->cpu.a,  2) + "  [" + bin(nes->cpu.a) + "]");
            DrawString(x, y + 40, "  X: $" + hex(nes->cpu.x,  2) + "  [" + bin(nes->cpu.x) + "]");
            DrawString(x, y + 50, "  Y: $" + hex(nes->cpu.y,  2) + "  [" + bin(nes->cpu.y) + "]");
            DrawString(x, y + 80, "Total cycles: " + std::to_string(nes->cpu.total_cycles));
        }

        void DrawDisasm(int x, int y, int lines)
        {
            auto it = disasm.find(nes->cpu.pc);
            std::map<uint16_t, std::string> next;
            next = nes->cpu.disassemble(nes->cpu.pc, nes->cpu.pc);

            int liney = (lines >> 1) * 10 + y;

            // Draw "live" disassembly of next instruction
            DrawString(x, liney, next[nes->cpu.pc], olc::CYAN);

            if (it != disasm.end()) {
                //DrawString(x, liney, (*it).second, olc::CYAN);
                while (liney < (lines * 10) + y) {
                    liney += 10;
                    if (++it != disasm.end()) {
                        DrawString(x, liney, (*it).second);
                    }
                }
            }

            it = disasm.find(nes->cpu.pc);
            liney = (lines >> 1) * 10 + y;
            if (it != disasm.end()) {
                //DrawString(x, liney, (*it).second, olc::CYAN);
                while (liney > y) {
                    liney -= 10;
                    if (--it != disasm.end()) {
                        DrawString(x, liney, (*it).second);
                    }
                }
            }
        }

        // Called once at start
        bool OnUserCreate() override
        {
            
            //// Set reset vector to 0x8000
            //nes->cpuWrite(0xFFFC, 0x00);
            //nes->cpuWrite(0xFFFD, 0x80);

            //// put some instructions in ram if we are in debug/development mode
            //if (debugmode) {
            //    std::stringstream ss;
            //    ss << "A2 02 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 B4 00 A1 01 B1 02 EA EA EA";
            //    uint16_t offset = 0x8000;
            //    while(!ss.eof()) {
            //        std::string b;
            //        ss >> b;
            //        nes->cpuWrite(offset++, (uint8_t)std::stoul(b, nullptr, 16));
            //    }
            //}

            //disasm = nes->cpu.disassemble(0x0000, 0xFFFF);

            //nes->cpu.reset();

            //// run the clock one cycle so the reset executes
            //do {
            //    nes->cpu.clock();
            //    nes->ppu.clock();
            //} while(!nes->cpu.complete());

            printf("[ Loading Cartridge      ]\n");
            cart = std::make_shared<Cartridge>(nesFilename);

            printf("[ Inserting Cartridge    ]\n");
            nes->insertCartridge(cart);

            if (!cart->isValid()) {
                printf("Invalid or unusable NES file!\n");
                exit(1);
            }

            printf("[ Disassembling code     ]\n");
            disasm = nes->cpu.disassemble(0x8000, 0xFFFF);

            printf("[ Resetting NES          ]\n");
            nes->reset();

            return true;
        }

        // Called once per frame
        bool OnUserUpdate(float fElapsedTime) override
        {
            Clear(olc::DARK_BLUE);

            if (GetKey(olc::Key::ESCAPE).bReleased)
                return false;

            if (runmode) {
                if (residualTime > 0.0f) {
                    residualTime -= fElapsedTime;
                } else {
                    residualTime += (1.0f / targetFPS) - fElapsedTime;

                    do {
                        nes->clock();
                    } while (!nes->ppu.frame_complete);

                    nes->ppu.frame_complete = false;
                    disasm = nes->cpu.disassemble(0x8000, 0xFFFF);
                }
            } else {
                // Advance one instruction
                if (GetKey(olc::Key::S).bPressed || GetKey(olc::Key::ENTER).bPressed) {
                    do {
                        nes->clock();
                    } while (!nes->cpu.complete());

                    do {
                        nes->clock();
                    } while (nes->cpu.complete());

                    disasm = nes->cpu.disassemble(0x8000, 0xFFFF);
                }

                // Advance one frame
                if (GetKey(olc::Key::F).bPressed) {
                    do {
                        nes->clock();
                    } while (!nes->ppu.frame_complete);

                    do {
                        nes->clock();
                    } while (!nes->cpu.complete());

                    nes->ppu.frame_complete = false;
                    disasm = nes->cpu.disassemble(0x8000, 0xFFFF);
                }
            }

            if (GetKey(olc::Key::SPACE).bPressed) {
                // Run Mode! Run machine until stopped!
                runmode = !runmode;
            }

            if (GetKey(olc::Key::R).bPressed)
                nes->cpu.reset();

            if (GetKey(olc::Key::I).bPressed)
                nes->cpu.irq();

            if (GetKey(olc::Key::N).bPressed)
                nes->cpu.nmi();

            if (GetKey(olc::Key::UP).bPressed) {
                ram2start += 0x100;
            }

            if (GetKey(olc::Key::PGDN).bPressed) {
                ram2start += 0x1000;
            }

            if (GetKey(olc::Key::DOWN).bPressed) {
                ram2start -= 0x100;
            }

            if (GetKey(olc::Key::PGUP).bPressed) {
                ram2start -= 0x1000;
            }

            if (GetKey(olc::Key::L).bPressed) {
                // increase execution speed which means decrease sleep time
                if (targetFPS > 5)
                    targetFPS -= 5;
                else if (targetFPS > 1)   // zero fps makes no sense...
                    targetFPS --;

            }

            if (GetKey(olc::Key::P).bPressed) {
                if (targetFPS < 5)
                    targetFPS ++;
                else
                    targetFPS += 5;
            }

            DrawRAM(2,   2, 0x0000,    16, 16);
            DrawRAM(2, 182, ram2start, 16, 16);
            DrawCPU(448, 2);
            DrawDisasm(448, 102, 23);

            DrawString(10, 460, "s = step     r = reset   i = irq  n = nmi  up/down/pgup/pgdn = change ram view");
            DrawString(10, 470, "f = frame  spc = run   p/l = +/- fps (" + std::to_string((int)targetFPS) + " fps)  ESC = quit");

            //if (runmode)
            //    std::this_thread::sleep_for(std::chrono::milliseconds(execspeed));

            return true;
        }
};

int main(int argc, char *argv[])
{
    //std::vector<uint8_t> data;
    //char *header;
    //int mapper;
    //ifstream file(argv[1], ios::binary);
    //streampos size;
    //streampos headersize = 16;
    //int prgromsize = 0;

    //file.seekg(headersize, ios::end);
    //size = file.tellg();
    //cout << "ROM size is " << size << " bytes." << endl;
    //file.seekg(0, ios::beg);
    //
    //header = new char[headersize];
    //file.read(header, headersize);

    //std::vector<uint8_t> contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //for (auto it : contents) {
    //    data.push_back(it);
    //}
    ////file.read(data, size);
    //
    //if(header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A) {
    //    cout << "Header OK" << endl;
    //    prgromsize = (int) header[4] * 16384;
    //    cout << "PRG ROM: " << (int) header[4] << " x 16 KB (" << prgromsize << ")" << endl;
    //    if (header[5]) {
    //        cout << "CHR ROM: " << (int) header[5] << " x  8 KB" << endl;
    //    } else {
    //        cout << "Board uses CHR RAM" << endl;
    //    }
    // 
    //    cout << "Byte 6 flags:" << endl;
    //    if (header[6] & 0b00000001) {
    //        cout << "Mirroring: vertical (horizontal arrangement) (CIRAM A10 = PPU A10)" << endl;
    //    } else {
    //        cout << "Mirroring: horizontal (vertical arrangement) (CIRAM A10 = PPU A11)" << endl;
    //    }
    //    if (header[6] & 0b00000010) {
    //        cout << "Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory" << endl;
    //    }
    //    if (header[6] & 0b00000100) {
    //        cout << "512-byte trainer at $7000-$71FF (stored before PRG data)" << endl;
    //    }
    //    if (header[6] & 0b00001000) {
    //        cout << "Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM" << endl;
    //    }

    //    mapper = (header[7] & 0xF0) | ((header[6] & 0xF0) >> 4);
    //    cout << "Mapper number: " << mapper << endl;


    //    cout << "Byte 7 flags:" << endl;
    //    if (header[7] & 0b00000001) {
    //        cout << "VS Unisystem ROM" << endl;
    //    }
    //    if (header[7] & 0b00000010) {
    //        cout << "PlayChoice-10 ROM (8KB of Hint Screen data stored after CHR data)" << endl;
    //    }
    //}

    printf("\nNESSY v%s\n\n", VERSION_STRING);

    if (argc < 2) {
        printf("Provide ROM filename.\n");
        exit(0);
    }

    printf("[ Constructing Machine   ]\n");
    TheNES = make_shared<Machine>();

    printf("[ Constructing Interface ]\n");
    Nessy test(TheNES, argv[1]);

    if (test.Construct(720, 480, 2, 2)) {
        printf("[ Starting Emulation     ]\n");
        test.Start();
    }

    printf("\n");
    return 0;
}

