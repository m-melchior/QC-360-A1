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
 *                         myRF24SPISpyOutput.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to record the SPI traffic towards a RF24 transceiver
 *
*/

#ifndef __myRF24SPISpyOutput_H__
#define __myRF24SPISpyOutput_H__

// print the buffer as bytes (decimal!)
void printBytes(uint8_t bufferIndex, boolean skipFirst) {
	// initialize the output string
// swap the comment in order to get a timestamp for each message
//	String readerStr = String(timeStamp[bufferIndex]) + " "; // = "";
	String readerStr = "";

// uncomment for trailing zeroes in each byte
/*  
	if (readerBytes[i] < 10) {
		readerStr += "  ";
	} else if (readerBytes[i] < 100) {
		readerStr += " ";
	}
*/

	// loop through the buffer
	// but only to the item before the last, so no delimiter gets added to the end of the list
	for (int i=(skipFirst) ? 1 : 0; i < lastIndex[bufferIndex]; i++) {
		readerStr += String(receiverBuffer[bufferIndex][i]) + " ";
/*
		// show real register number even when writing (aka register number + 32)
		if (i == 0 && 31 < receiverBuffer[bufferIndex][i]  && receiverBuffer[bufferIndex][i] < 60) {
			readerStr += (receiverBuffer[bufferIndex][i] - 32);
		} else {
			readerStr += receiverBuffer[bufferIndex][i];
		}
		readerStr += " ";
*/
	} // for (int i=(skipFirst) ? 1 : 0; i < lastIndexNumber - 1; i++) {

	// last item of the buffer goes without delimiter at the end
	readerStr += receiverBuffer[bufferIndex][lastIndex[bufferIndex]];

	// print the output String
	Serial.println(readerStr);
} // void printBytes() {


// print the buffer as zeroes and ones
void printBits(uint8_t bufferIndex) {
	// initialize the output string
	String readerStr = "";

	// loop through the buffer
	for (int i=0; i < lastIndex[bufferIndex] + 1; i++) {

		// really bitwise including leading zeroes
		// could've been done with String(input, BIN), but this is missing leading zeros
		for (int j=0; j<8; j++) {
			readerStr += String((receiverBuffer[bufferIndex][i] >> j) & 1) + " ";
		} // for (int j=0; j<8; j++) {

		// add a delimiter at the end if not the last item
		if (i < lastIndex[bufferIndex])
			readerStr += "  ";
	} // for (int i=0; i < tempBufferSize; i++) {
	
	// print the output String
	Serial.println(readerStr);
} // void printBits(uint8_t bufferIndex) {
#endif // #ifndef __myRF24SPISpyOutput_H__
