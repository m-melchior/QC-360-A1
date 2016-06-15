/* myRF24SlaveTest
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
 *                         myRF24SlaveTest.ino  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to create some SPI traffic towards a RF24 transceiver
 *
*/

/* 
 * This program is using the Optimized High Speed NRF24L01+ Driver Class
 * An optimized fork of the NRF24L01+ Driver written by J.Coliz <maniacbug@ymail.com>
 * licensed under the GNU GPL v2 license.
 * It can be found in Js' github repository under http://tmrh20.github.io/RF24/index.html.
 * Check the License.txt for more information.
 * The program itself serves no other purpose than to create some transmissions on the SPI interface
 * to be recorded in the myRF24SPISpy tool.
 * See <https://github.com/m-melchior/QC-360-A1/tree/master/src/Arduino/myRF24SPISpy> for more details.
*/

#include <RF24.h>

// had two boards for testing, one UNO and a Mega2560
// and was too lazy to comment / uncomment all the time
#if defined(__AVR_ATmega2560__)
	RF24 myRF24(49, 53);
#elif defined(__AVR_ATmega328P__)
	RF24 myRF24(9, 10);
#endif

void setup() {
	myRF24.begin();
	delay(1000);

	myRF24.setChannel(60);
	delay(1000);
	
	myRF24.setDataRate(RF24_1MBPS);
	delay(1000);

	myRF24.setAutoAck(false);
} // void setup() {

void loop() {
	asm volatile("nop");
} // void loop() {
