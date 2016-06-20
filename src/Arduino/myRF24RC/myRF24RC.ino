/* myRF24RC
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
 *                         myRF24RC.ino  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to turn Wii Nunchuck input into quadcopter RC commands
 *
*/

/* 
 * This program is using the Optimized High Speed NRF24L01+ Driver Class
 * An optimized fork of the NRF24L01+ Driver written by J.Coliz <maniacbug@ymail.com>
 * licensed under the GNU GPL v2 license.
 * It can be found in Js' github repository under http://tmrh20.github.io/RF24/index.html.
 * Check the License.txt for more information.
 * This program also uses a simple Wii Nunchuck library written by Michael Melchior <Michael.MelchiorGM@gmail.com>
 * licensed under the GNU GPL v3 license.
 * It is part of this package.
*/

#include "RF24.h"
#include "printf.h"
#include "Nunchuck.h"

// uncomment for Serial debug message
//#define RC_DEBUG

// had two boards for testing, one UNO and a Mega2560
// and was too lazy to comment / uncomment all the time
#if defined(__AVR_ATmega2560__)
	RF24 myRF24(49, 53);
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	RF24 myRF24(9, 10);
#endif

// address and message needed for the pairing
uint8_t pairingAddress[] = {101, 101, 101, 101, 101};
uint8_t pairingMessage[] = {223, 25, 85, 87, 86, 170, 50, 0};

// pipes share the	four most significant bytes
// 					least significant byte is unique for each pipe
// Hex is DF 19 55 57 C1 / C3 / C4 / C5 / C6
// Command: MSBit first to LSBit
// Data: LSByte first to MSByte, MSBit first to LSBit in each byte
uint8_t myAddress[][5] = {{223, 25, 85, 87, 193}, {195}, {196}, {197}, {198}};

// it is possible to change address!!
//uint8_t pairingMessage[] = {123, 123, 123, 123, 86, 170, 50, 0};
//uint8_t myAddress[][5] = {{123, 123, 123, 123, 193}, {195}, {196}, {197}, {198}};

// the message template to be sent out during normal operation
// byte		meaning
// 0		throttle
// 1		yaw
// 2		counter 0
// 3		pitch
// 4		roll
// 5		counter 1
// 6		counter 2
// 7		flag. bit 0-3 for flips, bit 4 for +/- switch (bit 4 set = '-')
uint8_t commandMessage[] = {0, 128, 11, 128, 128, 124, 125, 0};

// the controller values to send out
int throttle = 0;
int yaw = 128;
int pitch = 128;
int roll = 128;
// state of the +/- switch 0=+; 16=-
uint8_t switchState = 0;


// buffer for receiving
const int bufferSize = 8;
uint8_t myBuffer[bufferSize];


// instance for the nunchuck input
Nunchuck myNunchuck;
long nunchuckTimer;
long nunchuckThresh = 20;

bool flipFlag = false;

// values for nunchuck input to controller output calculation
// each "channel" has a minimum, center and maximum as well as a threshold to achieve a neutral or dead zone
// pitch and roll have a factor to set the agility of the aircraft

// min center and max values of my chuck joystick
// joy x 28 128 232
// joy y 32 124 220

uint8_t joyX_In = 0;
// tweak here for joystick x axis
uint8_t joyX_Min = 28;
uint8_t joyX_Center = 128;
uint8_t joyX_Max = 232;

// tweak here for x axis dead zone
uint8_t joyX_Thresh = 10;

uint8_t joyY_In = 0;
// tweak here for joystick y axis
uint8_t joyY_Min = 32;
uint8_t joyY_Center = 124;
uint8_t joyY_Max = 220;

// tweak here for joystick y axis dead zone
uint8_t joyY_Thresh = 15;


// pitching the nunchuck is limited to +/- 45
int pitchIn = 0;
int pitchMin = -45;
int pitchMax = 45;

// tweak here for pitch dead zone
uint8_t pitchThresh = 8;

// tweak here for pitch factor. keep factor below 128!
uint8_t pitchFactor = 127;


// rolling the nunchuck is limited to +/- 60
int rollIn = 0;
int rollMin = -60;
int rollMax = 60;

// tweak here for roll dead zone
uint8_t rollThresh = 8;

// tweak here for roll factor. keep factor below 128!
uint8_t rollFactor = 127;


// initialize the receiver buffer
void initBuffer() {
	memset(myBuffer, 0, bufferSize);
} // void initBuffer() {


// initialize the RF24 transceiver
void initRF24() {
	myRF24.begin();
	
	// needed, since the nRF24L01 is not resetting itself at startup!
	myRF24.stopListening();

	myRF24.setAutoAck(true);

	myRF24.setAddressWidth(5);

	// setting for 10 retries with 500ms limit
	myRF24.setRetries(2, 10);

	myRF24.setChannel(60);

	myRF24.setPayloadSize(8);

	myRF24.openReadingPipe(2, myAddress[1]);
	myRF24.openReadingPipe(3, myAddress[2]);
	myRF24.openReadingPipe(4, myAddress[3]);
	myRF24.openReadingPipe(5, myAddress[4]);
	
	// RF24_PA_MIN RF24_PA_LOW RF24_PA_HIGH RF24_PA_MAX
	myRF24.setPALevel(RF24_PA_HIGH);
	
	myRF24.setDataRate(RF24_1MBPS);

	// undecided if this is really needed here
//	myRF24.toggle_features();

	myRF24.enableDynamicPayloads();
	myRF24.enableDynamicAck();
	myRF24.enableAckPayload();

	myRF24.setCRCLength(RF24_CRC_8);
}


// get the acknowledge payload if any, apparently, there is none coming from the quadcopter
void getResponse() {
	myRF24.startListening();

	long responseTimer = millis();
	while (1) {
		if (myRF24.available()) {
			myRF24.read(myBuffer, bufferSize);
			String outString = "Received: ";

			for (int k=0; k<bufferSize-1; k++) {
				outString += String(myBuffer[k]) + " ";
			} // for (int k=0; k<bufferSize-1; k++) {

			outString += String(myBuffer[bufferSize-1]);

			Serial.println(outString);

			initBuffer();
			break;
		}
		
		if(millis() - responseTimer > 5) { // if (myRF24.available()) {
			break;
		}
	}
	
	myRF24.stopListening();
} // void getResponse() {


// pairing with the quadcopter
// this is a blocking procedure
void pair() {
#ifdef RC_DEBUG
	Serial.print("pairing");
#endif
	myRF24.openWritingPipe(pairingAddress);
	myRF24.openReadingPipe(0, pairingAddress);

	uint8_t heartbeatCounter = 0;
	
	while(1) {
		myRF24.flush_tx();
		if(myRF24.write(pairingMessage, sizeof(pairingMessage))) {
#ifdef RC_DEBUG
			Serial.println(String(millis()) + "success");
#endif
			break;
		} else { // if(myRF24.write(pairingMessage, sizeof(pairingMessage))) {
#ifdef RC_DEBUG
			if (heartbeatCounter++ < 9) {
				Serial.print(".");
			} else { // if (heartbeatCounter++ < 9) {
				Serial.println(".");
				heartbeatCounter = 0;
			} // } else { // if (heartbeatCounter++ < 9) {
#endif
			delay(500);
		} // } else { // if(myRF24.write(pairingMessage, sizeof(pairingMessage))) {
	} // while(1) {

	// get the acknowledge payload
	// not needed, since there doesn't seem to be any payloads coming back
//	getResponse();

	myRF24.openWritingPipe(myAddress[0]);
	myRF24.openReadingPipe(0, myAddress[0]);
} // void pair() {


// send a command to the quadcopter
void sendCommand() {
	if(myRF24.write(commandMessage, sizeof(commandMessage), 0)) {
		// get the acknowledge payload
		// not needed, since there doesn't seem to be any payloads coming back
//		getResponse();
	} else {
		// todo: switch channels when failure rate getting too high?
	}
} // void sendCommand() {


// prepare a command
void getCommandData() {
	// check if we get data
	if(myNunchuck.getData()) {
		// turn joystick X into yaw
		joyX_In = myNunchuck.getJoyX();
		if(joyX_In < joyX_Center - joyX_Thresh) {
			yaw = 128 - map(joyX_In, joyX_Min, joyX_Center - joyX_Thresh, 127, 0);
		} else if (joyX_In > joyX_Center + joyX_Thresh) { // if(joyX_In < joyX_Center - joyX_Thresh) {
			yaw = 128 + map(joyX_In, joyX_Center + joyX_Thresh, joyX_Max, 0, 127);				
		} else { // } else if (joyX_In > joyX_Center + joyX_Thresh) { // if(joyX_In < joyX_Center - joyX_Thresh) {
			yaw = 128;
		} // } else { // } else if (joyX_In > joyX_Center + joyX_Thresh) { // if(joyX_In < joyX_Center - joyX_Thresh) {


		// turn joystick Y into throttle
		joyY_In = myNunchuck.getJoyY();
		if(joyY_In < joyY_Center - joyY_Thresh) {
			throttle -= map(joyY_In, joyY_Min, joyY_Center - joyY_Thresh, 10, 0);
			if(throttle < 0)
				throttle = 0;
		} else if (joyY_In > joyY_Center + joyY_Thresh) { // if(joyY_In < joyY_Center - joyY_Thresh) {
			throttle += map(joyY_In, joyY_Center + joyY_Thresh, joyY_Max, 0, 10);
			if(throttle > 255)
				throttle = 255;
		} // } else if (joyY_In > joyY_Center + joyY_Thresh) { // if(joyY_In < joyY_Center - joyY_Thresh) {


		// turn pitch input into pitch control
		pitchIn = myNunchuck.getPitch();
		if(pitchIn < (-pitchThresh)) {
			if(pitchIn < pitchMin)
				pitchIn = pitchMin;

			pitch = 128 - map(pitchIn, pitchMin, -pitchThresh, pitchFactor, 0);
		} else if(pitchIn > pitchThresh) { // if(pitchIn < (-pitchThresh)) {
			if(pitchIn > pitchMax)
				pitchIn = pitchMax;

			pitch = 128 + map(pitchIn, pitchThresh, pitchMax, 0, pitchFactor);
		} else { // } else if(pitchIn > pitchThresh) { // if(pitchIn < (-pitchThresh)) {
			pitch = 128;
		} // } else { // } else if(pitchIn > pitchThresh) { // if(pitchIn < (-pitchThresh)) {


		// turn roll input into roll control
		rollIn = myNunchuck.getRoll();
		if(rollIn < (-rollThresh)) {
			if(rollIn < rollMin)
				rollIn = rollMin;

			roll = 128 - map(rollIn, rollMin, -rollThresh, rollFactor, 0);
		} else if(rollIn > rollThresh) { // if(rollIn < (-rollThresh)) {
			if(rollIn > rollMax)
				rollIn = rollMax;

			roll = 128 + map(rollIn, rollThresh, rollMax, 0, rollFactor);
		} else { // } else if(rollIn > rollThresh) { // if(rollIn < (-rollThresh)) {
			roll = 128;
		} // } else { // } else if(rollIn > rollThresh) { // if(rollIn < (-rollThresh)) {


		// switch to flip mode
		if (myNunchuck.getC()) {
			flipFlag = true;
		} else { // if (myNunchuck.getC()) {
			flipFlag = false;
		} // } else { // if (myNunchuck.getC()) {


		// panik button: press Z to set throttle to zero
		if (myNunchuck.getZ())
			throttle = 0;

#ifdef RC_DEBUG
		String delimiter = "\t";

		String outString = "";
		outString += String(joyX_In) + delimiter;
		outString += String(joyY_In) + delimiter;
		outString += String(pitchIn) + delimiter;
		outString += String(rollIn) + delimiter;
		outString += String(myNunchuck.getC()) + delimiter;
		outString += String(myNunchuck.getZ()) + delimiter + delimiter;

		outString += String(throttle) + delimiter;
		outString += String(yaw) + delimiter;
		outString += String(pitch) + delimiter;
		outString += String(roll) + delimiter;
		outString += String(flipFlag);
		Serial.println(outString);
#endif

		// put the calculated values in the command message
		commandMessage[0] = throttle;
		commandMessage[1] = yaw;
		commandMessage[3] = pitch;
		commandMessage[4] = roll;
		commandMessage[7] = (flipFlag) ? switchState + 15 : switchState;
		
	} else { // if(myNunchuck.getData()) {
		// todo failsafe settings...?
#ifdef RC_DEBUG
		Serial.println("nunchuck failed");
#endif
	} // } else { // if(myNunchuck.getData()) {
} // void getCommandData() {

void setup() {
	// use high speed serial
	Serial.begin(250000);

	// needed for the "printDetails()"
	printf_begin();

	// init receiver buffer
	initBuffer();

	// init transceiver
	initRF24();

	// print out the radio settings
	myRF24.printDetails();

	// pair with the quadcopter
	// this call is blocking until the quadcopter is paired
	pair();

	// start collecting nunchuck data
	myNunchuck.begin();

	// set the timer for the nunchuck intervals
	nunchuckTimer = millis();

	Serial.println("OK.");
} // void setup() {


// get the data from the nunchuck every <nunchuckThresh> ms
// calculate the controller values
// send the data to the quadcopter
void loop() {
	// check timer for nunchuck input
	if (millis() - nunchuckTimer > nunchuckThresh) {
		// get the command data
		getCommandData();

		// reset nunchuck timer
		nunchuckTimer = millis();
	} // if (millis() - nunchuckTimer > 100) {

	// keep sending commands regardless
	sendCommand();
	delay(5);
} // void loop() {
