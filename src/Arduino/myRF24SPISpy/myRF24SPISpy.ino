/* myRF24SPISpy
 * Copyright (C) 2016 Michael Melchior <MelchiorMichael@web.de>
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
 *                         myRF24SPISpy.ino  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to record the SPI traffic towards a RF24 transceiver
 *
*/

/* 
 * This program is using the standard Arduino SPI library.
 * Check the License.txt for more information.
 * Even though this program has been written to record the SPI traffic towards a RF24 transceiver,
 * I tried to make the tool as versatile as possible. By adjusting the buffer sizes and using
 * the printBytes or printBits routines, this tool could also be used for any other SPI interface.
 * The tool has <totalNumberOfBuffers> buffers with the size of <bufferSize>
 * on each SPI transmission, the <currentBuffer> is being filled with the incoming data
 * each time the buffer is full or a transmission is complete (CSN on <INTERRUPT_PIN> rising), the tool
 * is switching to the next buffer. The buffers filled with data will then be printed and initialized again
*/

#include <SPI.h>
#include "myRF24SPISpy.h"
#include "myRF24SPISpyOutput.h"
#include "myRF24SPISpySpecialOutput.h"

// interrupt routine for SPI messages
ISR (SPI_STC_vect) {
	// store the register first to prevent overwriting in the next call
	uint8_t mySPDR = SPDR;
	// store the stored entry
	receiverBuffer[currentBuffer][currentIndex++] = mySPDR;
	
	// if we have reached the end of the present buffer, switch to the next
	if(currentIndex >= bufferSize) {
		switchBuffer();
	}
}  // ISR (SPI_STC_vect) {


// interrupt routine for end of message (CSN rising again)
void csn() {
	// just switch the buffer
	switchBuffer();
}


// initialize all the buffers
void initBuffers() {
	// loop through all the buffers
	for (int i=0; i<totalNumberOfBuffers; i++) {
		// this is special for the first run
		// as we only erase as many entries as we have to
		lastIndex[i] = bufferSize;
		
		initBuffer(i);
	}

	currentBuffer = 0;
	currentIndex = 0;
} // void initBuffers() {


// initialize a single buffer
void initBuffer(byte bufferNumber) {
	// set as many entries to zero as we have used
	// in the first run, set the lastIndex to the bufferSize
	memset(receiverBuffer[bufferNumber], 0, lastIndex[bufferNumber]);
	lastIndex[bufferNumber] = 0;
	bufferDirtyFlag[bufferNumber] = false;
} // void initBuffer(byte bufferNumber) {


// switch to the next buffer
// used when overflowing the buffer or at the end of a SPI message
void switchBuffer() {
	// first transmission has an empty buffer at the beginning
	if (currentIndex > 0)
		lastIndex[currentBuffer] = currentIndex - 1;
	else
		lastIndex[currentBuffer] = 0;

	// mark the buffer as dirty
	bufferDirtyFlag[currentBuffer] = true;

	// move on to the next buffer
	currentBuffer++;
	currentIndex = 0;

	// if the chosen buffer index is higher than the total number of buffers
	// start from zero
	if (currentBuffer >= totalNumberOfBuffers)
		currentBuffer = 0;
} // void switchBuffer() {


void setup (void) {
	// use high speed serial
	Serial.begin (250000);

	initBuffers();

	// enable SPI interface
	SPCR |= bit(SPE);

	// pin used for end of message interrupt (indicated by CSN rising)
	pinMode(INTERRUPT_PIN, INPUT_PULLUP);
	// after each SPI transmission, the CSN pin should be rising again
	attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), csn, RISING);

	// attach another interrupt to the SPI interface
	SPI.attachInterrupt();

	Serial.println("OK.");
}  // void setup (void) {


// simple principle:
// loop through the buffers
// each time a buffer is marked dirty
// print the buffer
// initialize the printed buffer
void loop (void) {
	// loop through the buffers
	for (int i=0; i<totalNumberOfBuffers; i++) {
		// wait for the buffer to get dirty
		while(!bufferDirtyFlag[i]) {
			asm volatile("nop");
		} // while(!bufferDirtyFlag[i]) {

		// print the buffer
		//printBytes(i, false);
		//printBits(i);
		processBuffer(i);

		// initialize the printed buffer
		initBuffer(i);
	} // for (int i=0; i<totalNumberOfBuffers; i++) {
}  // void loop (void) {
