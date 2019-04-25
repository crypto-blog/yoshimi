/*
    RingBuffer.h - all buffering

    Copyright 2019 Will Godfrey

    This file is part of yoshimi, which is free software: you can redistribute
    it and/or modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    yoshimi is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.   See the GNU General Public License (version 2 or
    later) for more details.

    You should have received a copy of the GNU General Public License along with
    yoshimi; if not, write to the Free Software Foundation, Inc., 51 Franklin
    Street, Fifth Floor, Boston, MA  02110-1301, USA.

    Created April 2019
*/

#ifndef RINGBUFF_H
#define FILEMGR_H

#include <atomic>
#include <stdlib.h>

#include "globals.h"

class ringBuff
{
    private:
        std::atomic <uint> readPoint{0};
        std::atomic <uint> writePoint{0};
        uint32_t bufferSize;
        uint32_t mask;
        uint8_t *buffer;
        uint8_t blockSize;
    public:
        ringBuff(uint _bufferSize, uint _blockSize);
        ~ringBuff();
        bool write(uint8_t *writeData);
        bool read(uint8_t *readData);
};

#endif
