# nessy

This is an NES (Nintendo Entertainment System) emulator. It started as an implementation of https://github.com/OneLoneCoder/olcNES as I followed along his video series on YouTube about how he made his emulator.

My project quickly took on kind of a life of its own, and while a lot of OLC's code remain there's also a lot of my own code in this project.

I have only compiled it on Linux, but SFML is used for graphics, input, etc., so it's probably portable....

## Features
* Simple memory inspection
* Simple breakpoint support
* CPU debug info / status display
* Mapper debug info / status display
* Code disassembly and stepping through instructions
* All normal opcodes (games using undocumented/weird opcodes probably won't work at this point)
* Crude gamepad support

## Supported mappers
* 001
* 003
* 004
* 007
* 011
* 033 (doesn't work perfectly)
* 066

## Missing/lacking features
* Sound emulation
* Controller emulation isn't perfect
* probably many other (non-essential) things...

## How to build/run
On Arch Linux, install the sfml package, and build the project with 'make debug' or 'make release'

## Keys (for now you need to edit the source code to change these...)
* esc = quit
* space = start/stop emulation
* s = step to next instruction (only works when emulation is stopped)
* f = step to next frame (only works when emulation is stopped)

* wasd = controller 1 up, down, left, right
* k = controller 1 B
* l = controller 1 A
* tab = controller 1 Select
* enter = controller 1 Start
* F1 = display help (not guaranteed to be helpful)
* 1 = display RAM (use up/down/pageup/pagedown to select different pages of RAM to look at)
* 2 = display CPU debug info / status
* 3 = display Mapper debug info / status
* 4 = display disassembly
* 5 = display PPU debug info / status
* r = reset NES
* i = force IRQ
* n = force NMI




# License (OLC-3)
Copyright 2018, 2019 OneLoneCoder.com

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

1. Redistributions or derivations of source code must retain the above 
   copyright notice, this list of conditions and the following disclaimer.

2. Redistributions or derivative works in binary form must reproduce 
   the above copyright notice. This list of conditions and the following 
   disclaimer must be reproduced in the documentation and/or other 
   materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its 
   contributors may be used to endorse or promote products derived 
   from this software without specific prior written permission.
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
