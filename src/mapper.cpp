
#include "mapper.h"

const char *ImplementationStatusDescriptionStrings[] =
    {"Not working",
     "In development",
     "Working"};

Mapper::Mapper(uint16_t p, uint16_t c)
{
    prgBanks = p;
    chrBanks = c;
}

Mapper::~Mapper()
{
}

bool Mapper::irqState()
{
    return false;
}

void Mapper::irqClear()
{
}

void Mapper::scanline()
{
}

const char *Mapper::implementationStatusDescription(int i)
{
    return ImplementationStatusDescriptionStrings[i];
}
