/*
 * NESSY - an NES emulator/disassembler/debugger
 *
 * Yeah, that ambitious.
 *
 */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    char *data;
    ifstream file(argv[1], ios::binary);
    streampos begin, end;
    begin = file.tellg();
    file.seekg(0, ios::end);
    end = file.tellg();
    cout << "File size is " << (end - begin) << " bytes. begin = " << begin << " - end = " << end;
}
