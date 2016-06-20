/* myRF24SPISpy
 * Copyright (C) 2016 Michael Melchior <Michael.MelchiorGM@gmail.com>
 *
 * It is free software: You can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 *                         myRF24SPISpy.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to record the SPI traffic towards a RF24 transceiver
 *
*/

#ifndef __myRF24SPISpy_H__
#define __myRF24SPISpy_H__

// pin used for switching buffers between messages
#define INTERRUPT_PIN 2

// number of buffers in the ring
const byte totalNumberOfBuffers = 50;
// size of each buffer in the ring
const byte bufferSize = 33;

// the buffer for the SPI comms
uint8_t receiverBuffer[totalNumberOfBuffers][bufferSize];
// buffer to keep the last index (so we don't have to loop through the whole buffer)
volatile uint8_t lastIndex[totalNumberOfBuffers];
// dirty flag to keep track of the buffers that have recently been filled
volatile boolean bufferDirtyFlag[totalNumberOfBuffers];

// index for the current buffer in use
uint8_t currentBuffer;
// index within the current buffer
uint8_t currentIndex;

#endif // #ifndef __myRF24SPISpy_H__
