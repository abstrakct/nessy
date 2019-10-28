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

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "version.h"
#include "logger.h"
#include "machine.h"

using namespace std;

// Configuration
bool cfgDisplayRam = false;
bool cfgDisplayDisasm = false;
bool cfgDisplayCpu = false;
bool cfgDisplayHelp = false;
bool cfgDisplayPPU = false;
bool cfgDisplayMapper = false;

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

class NessyApplication {
    private:
        sf::RenderWindow window;
        //sf::RenderTexture renderTex;
        sf::Texture nesTex, ppuTex;
        sf::Sprite nesScreen, ppuSprite;
        sf::Clock clock;
        sf::Font sfmlFont;
        sf::Text t;
        sf::RectangleShape rect;
        sf::Event event;
        sf::Time startTime, endTime, elapsedTime, residualTime;

        int windowWidth, windowHeight;
        float scaleX, scaleY;
        int cSize = 18;

        bool running = false, emuRunning = false;

    public:
        std::shared_ptr<Machine> nes;
        std::map<uint16_t, std::string> disasm;
        std::string appName, nesFilename;
        uint16_t ram2start = 0x8000;
        bool debugmode, runmode = false;
        int execspeed = 100;

        // I don't know why I have to do this!
#ifdef NDEBUG
        float targetFPS = 180.0f;
#else
        float targetFPS = 60.0f;
#endif

        char log[100];
        uint8_t selectedPalette = 0;

        std::shared_ptr<Cartridge> cart;

        NessyApplication(std::shared_ptr<Machine> m, std::string filename, bool d = false)
        { 
            appName = "Nessy"; 
            nes = m;
            nesFilename = filename;
            debugmode = d;
        }

        ~NessyApplication() { }

        bool construct(int width, int height, float sX, float sY) {
            windowWidth = width;
            windowHeight = height;
            scaleX = sX;
            scaleY = sY;

            window.create(sf::VideoMode(width, height), appName);
            window.setVerticalSyncEnabled(true);
            // TODO: NES color scheme? light/dark grey, black, red
            window.clear(sf::Color::Blue);

            nesTex.create(width, height);
            nesScreen.setTexture(nesTex);
            nesScreen.setTextureRect(sf::Rect(0, 0, 256, 240));
            nesScreen.setPosition((width / 2) - 224 , cSize);
            nesScreen.scale(scaleX, scaleY);

            ppuTex.create(128, 128);
            ppuSprite.setTexture(ppuTex);
            ppuSprite.scale(scaleX, scaleY);

            t.setFillColor(sf::Color::White);
            t.setFont(sfmlFont);
            t.setCharacterSize(cSize);
            t.setStyle(sf::Text::Bold);

            //rect.setSize(sf::Vector2f(pixelWidth, pixelHeight));
            
            if (!sfmlFont.loadFromFile("Courier Prime Code.ttf")) {
                printf("ERROR: couldn't load font file!\n");
                return false;
            }

            startTime = clock.getElapsedTime();
            endTime = clock.getElapsedTime();

            running = true;
            return true;
        };

        void drawString(float x, float y, std::string text)
        {
            t.setPosition(x, y);
            t.setString(text);
            window.draw(t);
        }

        void drawString(float x, float y, std::string text, sf::Color color)
        {
            t.setFillColor(color);
            t.setPosition(x, y);
            t.setString(text);
            window.draw(t);
        }

        void drawRAM(int x, int y, uint16_t addr, int rows, int cols)
        {
            // TODO: highlight current PC (and read/writes????!!!!) !!!
            int ramx = x, ramy = y;
            for (int row = 0; row < rows; row++) {
                std::string s = "$" + hex(addr, 4) + ":";
                for (int col = 0; col < cols; col++) {
                    s += " " + hex(nes->cpuRead(addr, true), 2);
                    addr++;
                }
                drawString(ramx, ramy, s);
                ramy += cSize;
            }
        }

        void drawCPU(int x, int y)
        {
            int xs = 32;
            int space = cSize;
            drawString(x, y, "CPU:", sf::Color::White);
            drawString(x + (xs + space * 1), y, "N", nes->cpu.GetFlag(nes->cpu.N) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 2), y, "V", nes->cpu.GetFlag(nes->cpu.V) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 3), y, "-", nes->cpu.GetFlag(nes->cpu.U) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 4), y, "B", nes->cpu.GetFlag(nes->cpu.B) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 5), y, "D", nes->cpu.GetFlag(nes->cpu.D) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 6), y, "I", nes->cpu.GetFlag(nes->cpu.I) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 7), y, "Z", nes->cpu.GetFlag(nes->cpu.Z) ? sf::Color::Green : sf::Color::Red);
            drawString(x + (xs + space * 8), y, "C", nes->cpu.GetFlag(nes->cpu.C) ? sf::Color::Green : sf::Color::Red);
            drawString(x, y + cSize * 1, " PC: $" + hex(nes->cpu.pc, 4), sf::Color::White);
            drawString(x, y + cSize * 2, " SP: $" + hex(nes->cpu.sp, 4));
            drawString(x, y + cSize * 3, "  A: $" + hex(nes->cpu.a,  2) + "  [" + bin(nes->cpu.a) + "]");
            drawString(x, y + cSize * 4, "  X: $" + hex(nes->cpu.x,  2) + "  [" + bin(nes->cpu.x) + "]");
            drawString(x, y + cSize * 5, "  Y: $" + hex(nes->cpu.y,  2) + "  [" + bin(nes->cpu.y) + "]");
            //drawString(x, y + cSize * 6, "Total cycles: " + std::to_string(nes->cpu.total_cycles));
        }

        void drawMapper(int x, int y)
        {
            std::vector<std::string> mapperInfo = nes->cart->getMapperInfo();
            for (auto it : mapperInfo) {
                drawString(x, y, it);
                y += cSize;
            }
        }

        void drawDisasm(int x, int y, int lines)
        {
            auto it = disasm.find(nes->cpu.pc);
            std::map<uint16_t, std::string> next;
            next = nes->cpu.disassemble(nes->cpu.pc, nes->cpu.pc);

            int liney = (lines >> 1) * cSize + y;

            // Draw "live" disassembly of next instruction
            drawString(x, liney, next[nes->cpu.pc], sf::Color::Cyan);

            if (it != disasm.end()) {
                while (liney < (lines * cSize) + y) {
                    liney += cSize;
                    if (++it != disasm.end()) {
                        drawString(x, liney, (*it).second, sf::Color::White);
                    }
                }
            }

            it = disasm.find(nes->cpu.pc);
            liney = (lines >> 1) * cSize + y;
            if (it != disasm.end()) {
                while (liney > y) {
                    liney -= cSize;
                    if (--it != disasm.end()) {
                        drawString(x, liney, (*it).second);
                    }
                }
            }
        }

        void drawPPU(int x, int y)
        {
            ppuTex.update(nes->ppu.GetPatterntable(0, selectedPalette));
            ppuSprite.setPosition(x, y);
            window.draw(ppuSprite);

            ppuTex.update(nes->ppu.GetPatterntable(1, selectedPalette));
            ppuSprite.setPosition(x + 256 + 16, y);
            window.draw(ppuSprite);
        }

        void mainLoop()
        {
            endTime = clock.getElapsedTime();
            elapsedTime = endTime - startTime;
            startTime = endTime;
            
            while (window.isOpen() && running) {

                if (emuRunning) {
                    if (residualTime.asSeconds() > 0.0f) {
                        residualTime -= elapsedTime;
                    } else {
                        residualTime += sf::seconds(1.0f / targetFPS) - elapsedTime;

                        do {
                            nes->clock();
                        } while (!nes->ppu.frame_complete);

                        nes->ppu.frame_complete = false;

                        if (cfgDisplayDisasm)
                            //disasm = nes->cpu.disassemble(0x8000, 0xFFFF);
                            disasm = nes->cpu.disassemble(nes->cpu.pc - 0x20, nes->cpu.pc + 0x20);
                    }
                }

                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        running = false;
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                        running = false;
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                        emuRunning = !emuRunning;

                    if (!emuRunning) {
                        if (event.type == sf::Event::KeyPressed) {
                            if (event.key.code == sf::Keyboard::S) {
                                do {
                                    nes->clock();
                                } while (!nes->cpu.complete());

                                do {
                                    nes->clock();
                                } while (nes->cpu.complete());

                                if (cfgDisplayDisasm)
                                    disasm = nes->cpu.disassemble(nes->cpu.pc - 0x20, nes->cpu.pc + 0x20);
                            }

                            if (event.key.code == sf::Keyboard::F) {
                                do {
                                    nes->clock();
                                } while (!nes->ppu.frame_complete);

                                do {
                                    nes->clock();
                                } while (!nes->cpu.complete());

                                nes->ppu.frame_complete = false;

                                if (cfgDisplayDisasm)
                                    disasm = nes->cpu.disassemble(nes->cpu.pc - 0x20, nes->cpu.pc + 0x20);
                            }
                        }
                    }

                    if (event.type == sf::Event::KeyReleased) {
                        switch (event.key.code) {
                            case sf::Keyboard::Enter:
                                nes->controller[0].releaseButton(Controller::Button::Start); break;
                            case sf::Keyboard::Tab:
                                nes->controller[0].releaseButton(Controller::Button::Select); break;
                            case sf::Keyboard::W:
                                nes->controller[0].releaseButton(Controller::Button::Up); break;
                            case sf::Keyboard::A:
                                nes->controller[0].releaseButton(Controller::Button::Left); break;
                            case sf::Keyboard::S:
                                nes->controller[0].releaseButton(Controller::Button::Down); break;
                            case sf::Keyboard::D:
                                nes->controller[0].releaseButton(Controller::Button::Right); break;
                            case sf::Keyboard::K:
                                nes->controller[0].releaseButton(Controller::Button::B); break;
                            case sf::Keyboard::L:
                                nes->controller[0].releaseButton(Controller::Button::A); break;
                            default: break;
                        }
                    }

                    if (event.type == sf::Event::KeyPressed) {
                        switch (event.key.code) {
                            // Controller 1
                            case sf::Keyboard::Enter:
                                nes->controller[0].pressButton(Controller::Button::Start); break;
                            case sf::Keyboard::Tab:
                                nes->controller[0].pressButton(Controller::Button::Select); break;
                            case sf::Keyboard::W:
                                nes->controller[0].pressButton(Controller::Button::Up); break;
                            case sf::Keyboard::A:
                                nes->controller[0].pressButton(Controller::Button::Left); break;
                            case sf::Keyboard::S:
                                nes->controller[0].pressButton(Controller::Button::Down); break;
                            case sf::Keyboard::D:
                                nes->controller[0].pressButton(Controller::Button::Right); break;
                            case sf::Keyboard::K:
                                nes->controller[0].pressButton(Controller::Button::B); break;
                            case sf::Keyboard::L:
                                nes->controller[0].pressButton(Controller::Button::A); break;

                            // UI
                            case sf::Keyboard::F1:
                                cfgDisplayHelp = !cfgDisplayHelp; break;
                            case sf::Keyboard::Num1:
                                cfgDisplayRam = !cfgDisplayRam; break;
                            case sf::Keyboard::Num2:
                                cfgDisplayCpu = !cfgDisplayCpu; break;
                            case sf::Keyboard::Num3:
                                cfgDisplayMapper = !cfgDisplayMapper; break;
                            case sf::Keyboard::Num4:
                                cfgDisplayDisasm = !cfgDisplayDisasm; break;
                            case sf::Keyboard::Num5:
                                cfgDisplayPPU = !cfgDisplayPPU; break;
                            case sf::Keyboard::Num9:
                                targetFPS -= 5; break;
                            case sf::Keyboard::Num0:
                                targetFPS += 5; break;
                            case sf::Keyboard::P:
                                (++selectedPalette) &= 0x07; break;
                            case sf::Keyboard::Up:
                                ram2start += 0x100; break;
                            case sf::Keyboard::PageDown:
                                ram2start += 0x1000; break;
                            case sf::Keyboard::Down:
                                ram2start -= 0x100; break;
                            case sf::Keyboard::PageUp:
                                ram2start -= 0x1000; break;

                            case sf::Keyboard::R:
                                nes->reset(); break;
                            case sf::Keyboard::I:
                                nes->cpu.irq(); break;
                            case sf::Keyboard::N:
                                nes->cpu.nmi(); break;


                            default: break;
                        }
                    }
                }

                //renderTex.clear(sf::Color::Blue);

                window.clear(sf::Color::Blue);

                // Update and draw NES Screen
                nesTex.update(nes->ppu.GetNesScreen());
                window.draw(nesScreen);

                int x = 10;

                // Draw help text
                // TODO: update!
                if (cfgDisplayHelp) {
                    drawString(x, 580, "s = step     r = reset   i = irq  n = nmi  up/down/pgup/pgdn = change ram view");
                    drawString(x, 600, "f = frame  spc = run   9/0 = +/- fps (" + std::to_string((int)targetFPS) + " fps)  ESC = quit");
                }

                // Draw RAM
                if (cfgDisplayRam) {
                    drawRAM(x + 10, 12,  0x0000,    16, 16);
                    drawRAM(x + 10, 320, ram2start, 16, 16);
                }

                // Draw CPU status
                if (cfgDisplayCpu)
                    drawCPU(x + 1200, 12);

                // Draw mapper info
                if (cfgDisplayMapper)
                    drawMapper(x + 1200, cSize * 8);

                // Draw disassembly
                if (cfgDisplayDisasm)
                    drawDisasm(x + 1200, cSize * 16, 16);

                // Draw PPU
                if (cfgDisplayPPU)
                    drawPPU(x + 658, 518);
                
                // Display the window
                window.display();

                //sf::sleep(sf::microseconds(10));
            }
        };

        void start()
        {
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
        }
};

int main(int argc, char *argv[])
{

    printf("\nNESSY v%s\n\n", VERSION_STRING);

    if (argc < 2) {
        printf("Provide ROM filename.\n");
        exit(0);
    }

    printf("[ Constructing Machine   ]\n");
    TheNES = make_shared<Machine>();

    printf("[ Constructing Interface ]\n");

    // SFML
    
    NessyApplication nessy(TheNES, argv[1]);

    if (nessy.construct(1800, 960, 2, 2)) {
        printf("[ Starting Emulation     ]\n");
        nessy.start();
        nessy.mainLoop();
    } else {
        printf("ERROR during application initialization!\n");
        exit(1);
    }


    printf("\n");
    return 0;
}

