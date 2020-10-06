/*
 * NESSY - an NES emulator/disassembler/debugger
 *
 * Yeah, that ambitious.
 *
 */

#include <bitset>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "logger.h"
#include "machine.h"
#include "version.h"

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

class NessyApplication
{
private:
    sf::RenderWindow window;
    //sf::RenderTexture renderTex;
    sf::Texture nesTex, ppuTex;
    sf::Sprite nesScreen, ppuSprite;
    sf::Font sfmlFont;
    sf::Text t;
    sf::RectangleShape rect;
    sf::Event event;
    sf::Joystick joystick;

    int windowWidth, windowHeight;
    float scaleX, scaleY;
    int cSize = 18;

    bool running = false, emuRunning = false;
    bool breakpoint = false;
    int breakpointAddress;

public:
    std::shared_ptr<Machine> nes;
    DisassemblyType disasm;
    Disassembler disassembler;
    std::string appName, nesFilename;
    uint16_t ram2start = 0x8000;
    bool debugmode, runmode = false;
    int execspeed = 100;
    float targetFPS = 60.0f;
    char log[100];
    uint8_t selectedPalette = 0;

    std::shared_ptr<Cartridge> cart;

    NessyApplication(std::shared_ptr<Machine> m, std::string filename, bool _breakpoint, int _breakpointAddress, bool d = false)
    {
        appName = "Nessy";
        nes = m;
        nesFilename = filename;
        debugmode = d;
        breakpoint = _breakpoint;
        breakpointAddress = _breakpointAddress;
    }

    ~NessyApplication() {}

    bool construct(int width, int height, float sX, float sY)
    {
        windowWidth = width;
        windowHeight = height;
        scaleX = sX;
        scaleY = sY;

        window.create(sf::VideoMode(width, height), appName);
        window.setVerticalSyncEnabled(true);
        ImGui::SFML::Init(window);

        // TODO: NES color scheme? light/dark grey, black, red
        window.clear(sf::Color::Blue);

        nesTex.create(width, height);
        nesScreen.setTexture(nesTex);
        nesScreen.setTextureRect(sf::Rect(0, 0, 256, 240));
        nesScreen.setPosition((width / 2) - 256, cSize);
        nesScreen.scale(scaleX, scaleY);

        ppuTex.create(128, 128);
        ppuSprite.setTexture(ppuTex);
        ppuSprite.scale(2.0, 2.0);

        t.setFillColor(sf::Color::White);
        t.setFont(sfmlFont);
        t.setCharacterSize(cSize);
        t.setStyle(sf::Text::Bold);

        //rect.setSize(sf::Vector2f(pixelWidth, pixelHeight));

        if (!sfmlFont.loadFromFile("Courier Prime Code.ttf")) {
            printf("ERROR: couldn't load font file!\n");
            return false;
        }

        // TODO: more checking
        if (joystick.isConnected(0)) {
            std::cout << "        Found controller: " << std::string(joystick.getIdentification(0).name) << std::endl;
            printf("\tController has %d buttons\n", joystick.getButtonCount(0));
            if (joystick.hasAxis(0, sf::Joystick::Axis::X)) {
                printf("\tController has X axis\n");
            }
            if (joystick.hasAxis(0, sf::Joystick::Axis::Y)) {
                printf("\tController has Y axis\n");
            }
        }

        disassembler.ConnectMachine(nes);

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

    void drawCPU()
    {
        ImGui::Begin("CPU"); // begin window
        ImGui::Text("Flags: ");
        ImGui::TextColored(nes->cpu.flags & nes->cpu.N ? sf::Color::Green : sf::Color::Red, "N");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.V ? sf::Color::Green : sf::Color::Red, "V");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.U ? sf::Color::Green : sf::Color::Red, "-");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.B ? sf::Color::Green : sf::Color::Red, "B");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.D ? sf::Color::Green : sf::Color::Red, "D");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.I ? sf::Color::Green : sf::Color::Red, "I");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.Z ? sf::Color::Green : sf::Color::Red, "Z");
        ImGui::SameLine();
        ImGui::TextColored(nes->cpu.flags & nes->cpu.C ? sf::Color::Green : sf::Color::Red, "C");

        ImGui::Text("PC: %04X", nes->cpu.pc);
        ImGui::Text("SP: %04X", nes->cpu.sp);
        ImGui::Text(" A: %02X [%s]", nes->cpu.a, bin(nes->cpu.a).c_str());
        ImGui::Text(" X: %02X [%s]", nes->cpu.x, bin(nes->cpu.x).c_str());
        ImGui::Text(" Y: %02X [%s]", nes->cpu.y, bin(nes->cpu.y).c_str());

        ImGui::End();
    }

    void drawMapper(int x, int y)
    {
        std::vector<std::string> mapperInfo = nes->cart->getMapperInfo();
        ImGui::Begin("Mapper");
        for (auto it : mapperInfo) {
            ImGui::Text("%s", it.c_str());
        }
        ImGui::End();
    }

    void drawDisasm()
    {
        // int lines = 31;
        // auto it = disasm.find(nes->cpu.pc);
        // auto next = nes->cpu.disassemble(nes->cpu.pc, nes->cpu.pc);

        // int liney = (lines >> 1) * cSize + y;

        ImGui::Begin("Disassembly", NULL, ImGuiWindowFlags_HorizontalScrollbar);

        // auto newdisasm = disassembler.get(nes->cpu.pc, nes->cpu.pc + 16);
        // auto it = disasm.find(nes->cpu.pc);
        // auto it = newdisasm.begin();
        // while ((*it).begin()->first != nes->cpu.pc)
        //     it++;

        // it -= lines / 2;
        // if (it != newdisasm.end()) {
        //     for (int i = 0; i < lines; i++) {
        //         if (++it != newdisasm.end()) {
        //             if (it->address == nes->cpu.pc) {
        //                 ImGui::TextColored(sf::Color::Cyan, "%s", next.begin()->begin()->second.c_str());
        //             } else {
        //                 ImGui::Text("%s", (*it).begin()->second.c_str());
        //             }
        //         }
        //     }
        // }

        ImGui::End();
        // Draw "live" disassembly of next instruction
        // drawString(x, liney, next[nes->cpu.pc], sf::Color::Cyan);

        // it = disasm.find(nes->cpu.pc);
        // if (it != disasm.end()) {
        //     while (liney < (lines * cSize) + y) {
        //         liney += cSize;
        //         if (++it != disasm.end()) {
        //             drawString(x, liney, (*it).second, sf::Color::White);
        //         }
        //     }
        // }

        // it = disasm.find(nes->cpu.pc);
        // liney = (lines >> 1) * cSize + y;
        // if (it != disasm.end()) {
        //     while (liney > y) {
        //         liney -= cSize;
        //         if (--it != disasm.end()) {
        //             drawString(x, liney, (*it).second);
        //         }
        //     }
        // }
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
        auto time1 = std::chrono::system_clock::now();
        auto time2 = std::chrono::system_clock::now();
        float residualTime = 0.0f, elapsed = 0.0f;

        char windowTitle[255] = "ImGui + NESsy!";
        float color[3] = {0.f, 0.f, 0.f};
        sf::Clock deltaClock;
        sf::Color bgColor;

        // window.resetGLStates();
        while (window.isOpen() && running) {
            time2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = time2 - time1;
            elapsed = elapsedTime.count();
            time1 = time2;

            if (emuRunning) {
                if (residualTime > 0.0f) {
                    residualTime -= elapsed;
                } else {
                    residualTime += (1.0f / targetFPS) - elapsed;

                    do {
                        nes->clock();

                        if (breakpoint && (nes->cpu.pc == breakpointAddress)) {
                            emuRunning = false;
                            break;
                        }

                    } while (!nes->ppu.frame_complete);

                    nes->ppu.frame_complete = false;

                    if (cfgDisplayDisasm)
                        //disasm = nes->cpu.disassemble(0x8000, 0xFFFF);
                        disasm = nes->cpu.disassemble(nes->cpu.pc - 0x20, nes->cpu.pc + 0x20);
                }
            }

            while (window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(event);
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

                Controller::Button b;
                if (event.type == sf::Event::JoystickMoved) {
                    // TODO: FIX THIS!
                    // maybe getAxisPosition is better?
                    //printf("Joystick moved: %f\n", event.joystickMove.position);
                    if (event.joystickMove.axis == sf::Joystick::Axis::X) {
                        if (event.joystickMove.position < 0) {
                            nes->controller[0].pressButton(Controller::Button::Left);
                            nes->controller[0].releaseButton(Controller::Button::Right);
                            //nes->controller[0].releaseButton(Controller::Button::Up);
                            //nes->controller[0].releaseButton(Controller::Button::Down);
                            b = Controller::Button::Left;
                        }
                        if (event.joystickMove.position > 0) {
                            nes->controller[0].pressButton(Controller::Button::Right);
                            nes->controller[0].releaseButton(Controller::Button::Left);
                            //nes->controller[0].releaseButton(Controller::Button::Up);
                            //nes->controller[0].releaseButton(Controller::Button::Down);
                            b = Controller::Button::Right;
                        }
                    }
                    if (event.joystickMove.axis == sf::Joystick::Axis::Y) {
                        if (event.joystickMove.position < 0) {
                            nes->controller[0].pressButton(Controller::Button::Up);
                            //nes->controller[0].releaseButton(Controller::Button::Left);
                            //nes->controller[0].releaseButton(Controller::Button::Right);
                            nes->controller[0].releaseButton(Controller::Button::Down);
                            b = Controller::Button::Up;
                        }
                        if (event.joystickMove.position > 0) {
                            nes->controller[0].pressButton(Controller::Button::Down);
                            //nes->controller[0].releaseButton(Controller::Button::Left);
                            //nes->controller[0].releaseButton(Controller::Button::Right);
                            nes->controller[0].releaseButton(Controller::Button::Up);
                            b = Controller::Button::Down;
                        }
                    }
                    if (event.joystickMove.position == 0) {
                        nes->controller[0].releaseButton(b);
                        //        nes->controller[0].releaseButton(Controller::Button::Left);
                        //        nes->controller[0].releaseButton(Controller::Button::Right);
                        //        nes->controller[0].releaseButton(Controller::Button::Up);
                        //        nes->controller[0].releaseButton(Controller::Button::Down);
                    }
                }

                if (event.type == sf::Event::JoystickButtonReleased) {
                    //printf("Joystick button %d pressed\n", event.joystickButton.button);
                    switch (event.joystickButton.button) {
                    case 0: // B
                        nes->controller[0].releaseButton(Controller::Button::B);
                        break;
                    case 1: // A
                        nes->controller[0].releaseButton(Controller::Button::A);
                        break;
                    case 8: // Select
                        nes->controller[0].releaseButton(Controller::Button::Select);
                        break;
                    case 9: // Start
                        nes->controller[0].releaseButton(Controller::Button::Start);
                        break;
                    }
                }

                if (event.type == sf::Event::JoystickButtonPressed) {
                    //printf("Joystick button %d pressed\n", event.joystickButton.button);
                    switch (event.joystickButton.button) {
                    case 0: // B
                        nes->controller[0].pressButton(Controller::Button::B);
                        break;
                    case 1: // A
                        nes->controller[0].pressButton(Controller::Button::A);
                        break;
                    case 8: // Select
                        nes->controller[0].pressButton(Controller::Button::Select);
                        break;
                    case 9: // Start
                        nes->controller[0].pressButton(Controller::Button::Start);
                        break;
                    }
                }

                if (event.type == sf::Event::KeyReleased) {
                    switch (event.key.code) {
                    case sf::Keyboard::Enter:
                        nes->controller[0].releaseButton(Controller::Button::Start);
                        break;
                    case sf::Keyboard::Tab:
                        nes->controller[0].releaseButton(Controller::Button::Select);
                        break;
                    case sf::Keyboard::W:
                        nes->controller[0].releaseButton(Controller::Button::Up);
                        break;
                    case sf::Keyboard::A:
                        nes->controller[0].releaseButton(Controller::Button::Left);
                        break;
                    case sf::Keyboard::S:
                        nes->controller[0].releaseButton(Controller::Button::Down);
                        break;
                    case sf::Keyboard::D:
                        nes->controller[0].releaseButton(Controller::Button::Right);
                        break;
                    case sf::Keyboard::K:
                        nes->controller[0].releaseButton(Controller::Button::B);
                        break;
                    case sf::Keyboard::L:
                        nes->controller[0].releaseButton(Controller::Button::A);
                        break;
                    default:
                        break;
                    }
                }

                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    // Controller 1
                    case sf::Keyboard::Enter:
                        nes->controller[0].pressButton(Controller::Button::Start);
                        break;
                    case sf::Keyboard::Tab:
                        nes->controller[0].pressButton(Controller::Button::Select);
                        break;
                    case sf::Keyboard::W:
                        nes->controller[0].pressButton(Controller::Button::Up);
                        break;
                    case sf::Keyboard::A:
                        nes->controller[0].pressButton(Controller::Button::Left);
                        break;
                    case sf::Keyboard::S:
                        nes->controller[0].pressButton(Controller::Button::Down);
                        break;
                    case sf::Keyboard::D:
                        nes->controller[0].pressButton(Controller::Button::Right);
                        break;
                    case sf::Keyboard::K:
                        nes->controller[0].pressButton(Controller::Button::B);
                        break;
                    case sf::Keyboard::L:
                        nes->controller[0].pressButton(Controller::Button::A);
                        break;

                    // UI
                    case sf::Keyboard::F1:
                        cfgDisplayHelp = !cfgDisplayHelp;
                        break;
                    case sf::Keyboard::Num1:
                        cfgDisplayRam = !cfgDisplayRam;
                        break;
                    case sf::Keyboard::Num2:
                        cfgDisplayCpu = !cfgDisplayCpu;
                        break;
                    case sf::Keyboard::Num3:
                        cfgDisplayMapper = !cfgDisplayMapper;
                        break;
                    case sf::Keyboard::Num4:
                        cfgDisplayDisasm = !cfgDisplayDisasm;
                        break;
                    case sf::Keyboard::Num5:
                        cfgDisplayPPU = !cfgDisplayPPU;
                        break;
                    case sf::Keyboard::Num9:
                        targetFPS -= 5;
                        break;
                    case sf::Keyboard::Num0:
                        targetFPS += 5;
                        break;
                    case sf::Keyboard::P:
                        (++selectedPalette) &= 0x07;
                        break;
                    case sf::Keyboard::Up:
                        ram2start += 0x100;
                        break;
                    case sf::Keyboard::PageDown:
                        ram2start += 0x1000;
                        break;
                    case sf::Keyboard::Down:
                        ram2start -= 0x100;
                        break;
                    case sf::Keyboard::PageUp:
                        ram2start -= 0x1000;
                        break;

                    case sf::Keyboard::R:
                        nes->reset();
                        break;
                    case sf::Keyboard::I:
                        nes->cpu.irq();
                        break;
                    case sf::Keyboard::N:
                        nes->cpu.nmi();
                        break;

                    default:
                        break;
                    }
                }
            }

            ImGui::SFML::Update(window, deltaClock.restart());

            ImGui::Begin("Options"); // begin window

            // ImGui::ShowDemoWindow();

            if (ImGui::ColorEdit3("Background color", color)) {
                bgColor.r = static_cast<sf::Uint8>(color[0] * 255.f);
                bgColor.g = static_cast<sf::Uint8>(color[1] * 255.f);
                bgColor.b = static_cast<sf::Uint8>(color[2] * 255.f);
            }

            // // Window title text edit
            // ImGui::InputText("Window title", windowTitle, 255);

            // if (ImGui::Button("Update window title"))
            // {
            //     // this code gets if user clicks on the button
            //     // yes, you could have written if(ImGui::InputText(...))
            //     // but I do this to show how buttons work :)
            //     window.setTitle(windowTitle);
            // }

            //renderTex.clear(sf::Color::Blue);

            window.clear(bgColor);

            // Update and draw NES Screen
            nesTex.update(nes->ppu.GetNesScreen());
            window.draw(nesScreen);

            int x = 10;

            // Draw help text
            // TODO: update!
            if (cfgDisplayHelp) {
                ImGui::Begin("Help");
                // drawString(x, 580, "s = step     r = reset   i = irq  n = nmi  up/down/pgup/pgdn = change ram view");
                // drawString(x, 600, "f = frame  spc = run   9/0 = +/- fps (" + std::to_string((int)targetFPS) + " fps)  ESC = quit");
                // drawString(x, 620, "residualTime: " + std::to_string(residualTime) + "  elapsed: " + std::to_string(elapsed));

                ImGui::Text("space - start/stop emulation");
                ImGui::Text("i - irq");
                ImGui::Text("n - nmi");
                ImGui::Text("r - reset");
                ImGui::Text("1 - show memory editor");
                ImGui::Text("2 - show cpu status");
                ImGui::Text("3 - show mapper status");
                ImGui::Text("4 - show disassembly");
                ImGui::Text("5 - show ppu status");
                ImGui::Text("9 - decrease fps");
                ImGui::Text("0 - increase fps");
                ImGui::Text("esc - quit");
                ImGui::Text(" ");
                ImGui::Text("controller 1:");
                ImGui::Text("wasd - up/down/left/right");
                ImGui::Text("k/l - b/a");
                ImGui::Text(" ");
                ImGui::Text("when emulation is stopped:");
                ImGui::Text("f - advance one frame");
                ImGui::Text("s - step cpu");
                ImGui::End();
            }

            // Draw RAM
            if (cfgDisplayRam) {
                drawRAM(x + 10, 12, 0x0000, 16, 16);
                drawRAM(x + 10, 320, ram2start, 16, 16);
            }

            x += 264;
            // Draw CPU status
            if (cfgDisplayCpu)
                drawCPU(/*x + 1200, 12*/);

            // Draw mapper info
            if (cfgDisplayMapper)
                drawMapper(x + 1200, cSize * 8);

            // Draw disassembly
            if (cfgDisplayDisasm)
                drawDisasm();

            // Draw PPU
            if (cfgDisplayPPU)
                drawPPU(20, 650);

            ImGui::End();
            ImGui::SFML::Render(window);
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

        // printf("[ Disassembling code     ]\n");
        // disasm = nes->cpu.disassemble(0x8000, 0xFFFF);
        printf("[ Preparing Disassembly  ]\n");
        disassembler.disassemble(0x8000, 0xFFFF);

        printf("[ Resetting NES          ]\n");
        nes->reset();
    }
};

int main(int argc, char *argv[])
{
    bool breakpoint = false;
    int breakpointAddress = 0;
    printf("\nNESSY v%s\n\n", VERSION_STRING);

    if (argc < 2) {
        printf("Provide ROM filename.\n");
        exit(0);
    }

    if (argc > 3) {
        std::string s = string(argv[2]);
        if (s == "-b") {
            breakpoint = true;
            breakpointAddress = std::stoi(string(argv[3]), nullptr, 16);
        }
    }

    printf("[ Constructing Machine   ]\n");
    TheNES = make_shared<Machine>();

    printf("[ Constructing Interface ]\n");

    // SFML

    NessyApplication nessy(TheNES, argv[1], breakpoint, breakpointAddress);

    if (nessy.construct(1900, 960, 3, 3)) {
        printf("[ Starting Emulation     ]\n");
        nessy.start();

        nessy.mainLoop();
    } else {
        printf("ERROR during application initialization!\n");
        exit(1);
    }

    ImGui::SFML::Shutdown();
    printf("\n");
    return 0;
}
