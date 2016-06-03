/* myRF24Scanner
 * Copyright (C) 2016 Michael Melchior <MelchiorMichael@web.de>
 *
 * This program is based on an example for the RF24 library
 * written by J. Coliz and licensed under GNU GPL v2
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
 *                         myRF24Scanner.ino  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to scan 2.4Ghz Channels for activity
 *
*/

/* 
 * This program is using the Optimized High Speed NRF24L01+ Driver Class
 * An optimized fork of the NRF24L01+ Driver written by J.Coliz <maniacbug@ymail.com>
 * licensed under the GNU GPL v2 license.
 * It can be found in Js' github repository under http://tmrh20.github.io/RF24/index.html.
 * Check the License.txt for more information.
*/
 
#include "printf.h"
#include "RF24.h"

// Uno
RF24 myRF24(9, 10);
// Mega 2560
//RF24 myRF24(49, 53);

// range of channels to scan
const uint8_t minChannels = 0;
const uint8_t maxChannels = 127;
const uint8_t numChannels = maxChannels - minChannels;

// number of samples for each scan
const uint8_t num_samples = 100;

// storing the results of the scan
uint8_t values[numChannels];

// "filter" out interference
const uint8_t threshold = 5;

void setup(void) {
	// use high speed serial
	Serial.begin(250000);

	// needed for the "printDetails()"
	printf_begin();

	// start the radio
	myRF24.begin();

	// disable error control features
	myRF24.disableCRC();

	// disable sending out acknowledge messages
	myRF24.setAutoAck(false);

	// set the data rate, guesses 1MBPS, since this was the only rate with peaks during scans
	myRF24.setDataRate(RF24_1MBPS);

	// print out the radio settings
	myRF24.printDetails();

	Serial.println("OK.");
} // void setup(void) {

void loop(void) {
	// reset memory
	memset(values, 0, sizeof(values));

	// check each channel num_samples times
	for(int i = 0; i < num_samples; i++) {
		// switch through the channels
		for(int j = 0; j < numChannels; j++) {
			myRF24.setChannel(minChannels + j);

			myRF24.startListening();

			delayMicroseconds(128);

			myRF24.stopListening();

			// store the result
			if (myRF24.testCarrier()){
				values[j]++;
			}
		} // for(int j = 0; j < numChannels; j++) {
	} // for(int i = 0; i < num_samples; i++) {

	// output of the results
	for (int channel = 0; channel < numChannels; channel++) {
		// only print those above the threshold
		if(values[channel] > threshold) {
			Serial.println(String(minChannels + channel) + ": " + String(values[channel]));
		} // if(values[channel] > 5) {
	} // for (int channel = 0; channel < numChannels; channel++) {

	Serial.println("***\r");
} // void loop(void) {

// Results:
/*

STATUS		 = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
RX_ADDR_P0-1	 = 0xe7e7e7e7e7 0xc2c2c2c2c2
RX_ADDR_P2-5	 = 0xc3 0xc4 0xc5 0xc6
TX_ADDR		 = 0xe7e7e7e7e7
RX_PW_P0-6	 = 0x00 0x00 0x00 0x00 0x00 0x00
EN_AA		 = 0x00
EN_RXADDR	 = 0x03
RF_CH		 = 0x4c
RF_SETUP	 = 0x07
CONFIG		 = 0x06
DYNPD/FEATURE	 = 0x00 0x00
Data Rate	 = 1MBPS
Model		 = nRF24L01+
CRC Length	 = Disabled
PA Power	 = PA_MAX
OK.
***

***

59: 7
60: 8
61: 7
***

58: 9
59: 8
60: 6
61: 9
62: 7
***

57: 6
58: 7
59: 10
60: 10
61: 6
62: 6
***

*/
