/* myRF24PromiscuousReceiver
 * Copyright (C) 2016 Michael Melchior <Michael.MelchiorGM@gmail.com>
 *
 * This program is inspired by a post of Travis Goodspeed on 
 * intercepting the transmission of a 2.4GHz wireless keyboard with a nrf2410 
 * Check http://travisgoodspeed.blogspot.de/2011/02/promiscuity-is-nrf24l01s-duty.html
 * for more information
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
 *                         myRF24PromiscuousReceiver.ino  -  description
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

#include "RF24.h"
#include "printf.h"

// print mode is a bit over-done, but will be paying off in future versions, 
// when other print modes like printing values and such come in
#define PRINT_MODE_BYTES 0
#define PRINT_MODE_BITS 1
#define PRINT_MODE_COUNT 2

// had two boards for testing, one UNO and a Mega2560
// and was too lazy to comment / uncomment all the time
#if defined(__AVR_ATmega2560__)
	RF24 myRF24(49, 53);
#elif defined(__AVR_ATmega328P__)
	RF24 myRF24(9, 10);
#endif

// buffer for reading incoming messages
const uint8_t maxBufferSize = 32;
uint8_t myBuffer[maxBufferSize];
uint8_t bufferSize = 16;

// channel to listen to
uint8_t channel = 60;

// there are two address modes: 0x55 and 0xAA
boolean addressMode = true;

// there are (presently) two print modes: bits and bytes
uint8_t printMode = 0;

// some flags for the state
boolean isRunning = false;
boolean isListening = false;

// for a heartbeat to show that the program is still running
// when there is no transmissions
boolean hasHeartbeat = false;
uint8_t heartbeatCounter = 0;
long timer;
long timerThreshold = 1000;


// initialize buffer
void initBuffer() {
	memset(myBuffer, 0, bufferSize);
} // void initBuffer() {


void setup() {
	// use high speed serial
	Serial.begin(250000);

	// needed for the "printDetails()"
	printf_begin();
	

	initBuffer();

	// start the radio
	myRF24.begin();


	myRF24.setChannel(channel);

	// disable error control features
	myRF24.disableCRC();

	// disable sending out acknowledge messages
	myRF24.setAutoAck(false);


	myRF24.setDataRate(RF24_1MBPS);


	myRF24.setAddressWidth(3);

	// print out the radio settings
	myRF24.printDetails();

	Serial.println("OK.");
} // void setup() {


// print only zeroes and ones
void printBits() {
	// initialize output string
	String readerStr = "";

	for (int i=0; i < bufferSize; i++) {
		for (int j=0; j<7; j++) {
			readerStr += (((myBuffer[i] << j) & 128)?1:0) + " ";
		} // for (int j=0; j<8; j++) {
		
		// no need for a delimiter at the end of the last bit
		readerStr += (((myBuffer[i] << 7) & 128)?1:0);
	} // for (int i=0; i < bufferSize; i++) {

	// output of the string
	Serial.println(readerStr);
} // void printBits() {


// print the bytes
void printBytes() {
	// initialize output string
	String readerStr = "";

	// loop through the buffer
	for (int i=0; i < bufferSize - 1; i++) {
		// leeding spaces to make things look nicers
		if (myBuffer[i] < 10) {
			readerStr += "  ";
		} else if (myBuffer[i] < 100) { // if (myBuffer[i] < 10) {
			readerStr += " ";
		} // else if (myBuffer[i] < 100) {

		// add a byte and a delimiter
		readerStr += String(myBuffer[i]) + " ";
	} // for (int i=0; i < bufferSize - 1; i++) {
	
	// no need for a delimiter at the end of the last byte
	readerStr += String(myBuffer[bufferSize - 1]);
  
	// output of the string
	Serial.println(readerStr);
} // void printBytes() {


// toggle between the two addresses 0x55 and 0xAA
void toggleAddress() {
	if(isListening) {
		myRF24.stopListening();
	} // if(isListening) {
	
	if(addressMode) {
		// DEC 170
		// BIN 10101010
		myRF24.openReadingPipe(0, 0xAA);
		myRF24.openReadingPipe(1, 0xAA);
	} else { // if(addressMode) {
		// DEC 85
		// BIN 01010101
		myRF24.openReadingPipe(0, 0x55);
		myRF24.openReadingPipe(1, 0x55);	
	} // } else { // if(addressMode) {
	
	addressMode != addressMode;
	
	if(isListening) {
		myRF24.startListening();
	} // if(isListening) {
} // void toggleAddress() {


void loop() {
	// are we in running mode?
	if(isRunning) {
		// are we listening?
		if(!isListening) {
			// if no, then start it
			myRF24.startListening();
			isListening = true;
		} // if(!isListening) {

		// heartbeat, prints a dot every second to show we're still alive, might as well be a blinking LED
		if (hasHeartbeat) {
			// get the present time
			timer = millis();

			// stay here until there is an incoming transmission or the timer is up
			while (!myRF24.available() && ((millis() - timer) < timerThreshold)) {
				asm("nop");
			} // while (!myRF24.available() && ((millis() - timer) < timerThreshold)) {

			// print a dot and after each 10 dots add a new line
			if (heartbeatCounter++ >= 10) {
				Serial.println(".");
				heartbeatCounter = 0;
			} else { // if (heartbeatCounter++ >= 10) {
				Serial.print(".");
			} // } else { // if (heartbeatCounter++ >= 10) {
		} // if (hasHeartbeat) {

		// minimal approach:
		// if there is anything available
		// read it
		// print it
		// initialize buffer
		if (myRF24.available()) {
			// read it
			myRF24.read(myBuffer, bufferSize);

			// print it
			switch(printMode) {
				case(PRINT_MODE_BYTES):
					printBytes();
					break;
				case(PRINT_MODE_BITS):
					printBits();
					break;
			} // switch(printMode) {

			// initialize buffer
			initBuffer();
		} // if (myRF24.available()) {

	} else { // if(isRunning) {
		// are we listening?
		if(isListening) {
			// if yes, then stop it
			myRF24.stopListening();
			isListening = false;
		} // if(isListening) {
	} //  else { // if(isRunning) {
	
	// get some user input
	// a			toggle address between 0x55 and 0xAA
	// b+ / b-		increase / decrease buffer size
	// h			toggle heartBeat
	// p			toggle print mode (bits / bytes)
	// s			start / stop
	if(Serial.available()) {
		// check first char
		char inChar = Serial.read();

		if (inChar == 'a') {
			toggleAddress();

		} else if (inChar == 'b' && Serial.available()) { // if (inChar == 'a') {
			// check second char
			inChar = Serial.read();

			if (inChar == '+') {
				if (bufferSize < maxBufferSize) {
					bufferSize++;
				} // if (bufferSize < maxBufferSize) {
			} else if (inChar == '-') { // if (inChar == '+') {
				if (bufferSize > 0) {
					bufferSize--;
				} // if (bufferSize < maxBufferSize) {
			} // else if (inChar == '-') { // if (inChar == '+') {

		} else if (inChar == 'p') { // if (inChar == 'a') {
			if (printMode < PRINT_MODE_COUNT) {
				printMode++;
			} else { // if (printMode < PRINT_MODE_COUNT) {
				printMode = 0;
			} // } else { // if (printMode < PRINT_MODE_COUNT) {

		} else if (inChar == 'h') {
			hasHeartbeat = !hasHeartbeat;

		} else if (inChar == 's') { // if (inChar == 'a') {
			isRunning = !isRunning;

		} // if (inChar == 'a') {
	} // if(Serial.available()) {
} // void loop() {


// Results:
/*
STATUS		 = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
RX_ADDR_P0-1	 = 0x000055 0x000055
RX_ADDR_P2-5	 = 0xc3 0xc4 0xc5 0xc6
TX_ADDR		 = 0x5519df
RX_PW_P0-6	 = 0x20 0x20 0x20 0x20 0x20 0x20
EN_AA		 = 0x00
EN_RXADDR	 = 0x3f
RF_CH		 = 0x3c
RF_SETUP	 = 0x07
CONFIG		 = 0x07
DYNPD/FEATURE	 = 0x00 0x00
Data Rate	 = 1MBPS
Model		 = nRF24L01+
CRC Length	 = Disabled
PA Power	 = PA_MAX
OK.
 86 242 170  82  86 232 200 171 218 169  82 203  66 130 106 111  73  54 169 105 170 182 226 170 166 213  82 151  90 210  54 173
 10  80 144   0 132   0  11 220 229 171  87 223  95 222 174 170  91  92 170 238 212 253 174 127 175 162 154  78 106 215  61 134
 85  85  87 253 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 127  94 223 125 111 223 255 251 223 223 237 189 239
 85  85  87  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 126 187 191 223 255 174 123 255 222 213 223 189 215
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 190 237 190 251 247 181 125 253 237 110 255 191 127
 85  85 127  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7  91 255 213 247 245 254 183 221 239 221 250 253 255
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 187 126 247 223 254 251 239 186 215 127 253 223 255
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 173 255 191 246 247 235 251 255 207 244 169 213 139
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 187 255 189 239 187 247 253 127 123 189 219 126 234
 85  85  95 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 189 245 223 251 127 239 183 253 221  86 255 191 235
 85  85  95  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7  94 238 219 254 255 191 250 251 111 220 104 157 197
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191 239 213  81 132 154 157 106 146 185 138 147 74
 85  85  95 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 187 251 248  85  81  58  18 132 208 245  43 196 164
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 189 239 255 244 210 200 148 170  41  22  15  68 109
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 190 207 119 246 223 254 255 127 117 222 251 191 181
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 175 191 239 254 255 191 253 181 253 247 119 221 215
 85  85  95 253 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 127 191 255 251 222 250 255  74 247 123 109 223 117
 85  85  87 253 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 126 247 215 223 254 175 173  92  42 215 111 155 175
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 183 239 143 247 118 255 252 223 255 238 255 222 118
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191 253 111 255 222 174 173  41 208  89  45 166 165
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 173 245 255 111 255 213 239 126 255 255 255 125 253
 85  85  95  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 123 251 243 107 239 119 111 171 185 251 123  95 159
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 171 255 255 254 235  62 101  99   9 122 160 181 200
 85  85  87  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 127 254 181  83  53 125 109  53 108 233 205 166 153
 85  85  87  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 109 255 109 254 215 175 251 173 254 223 119 181 255
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191 127 254 235 119 254 187  90 223 127 255 239 254
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 171 111 255 255 246 215 235 125 187 245 123 183 246
 85  85 127 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 181 237 190 175 222 181 255 239 213 118 185  82 116
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 190 221 253 215 253 238 234 105  78 163 154 136 168
 85  85  87  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 125 255 238 221 123 191 175 237 127 247 159 251 171
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 173 255 219 255 189 253 247  94 186 251 255 119 87
 85  85  95  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 127 239 127 233 250 155 127 171 125 251 245 222 223
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191  95  87 183 255 111 222 250 183 215 250 255 247
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 186 255 239 247 218 223 219  95 247 191 251 245 255
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 187 211 254 187 127 109 125 182 173 223 234 173 215
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191 183 127 157 222 215 183 127 234 181 210 189 190
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 187 251 189 223 245 110 253 126 255 233 251 255 251
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191  95 181 215 183  85 207  81 166 113 127   6 118
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 183 250 219  93 241 255 230 222 254 245 247 127 250
 85  85  87  85 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 111 187 190 250 223 247 191 187 250 191 222 255 126
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 187 183 239 191 251 222 254 189 222 223 185 238 187
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 190 253 250  65  34 169   5  90 136 214 196 198 151
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 189 237 187 251 239 125 251 249 118 239 215 245 223
 85  85  87 170 193  87  85  25 223  34 128  64  25  64  64  25  25   0   3 191 251 219 189 171 111 255 233  85 170 180 214 250
 85  85  87 253 130 174 170  51 190  69   0 128  50 128 128  50  50   0   7 123 254 237 215 255 127 253 251 219 215 255 222 126
 85  85  86  85 101 101 101 101 101  35 239 140 170 171 171  85  25   0 113 123 239 255 119 169 105 129  42  10 128   4  42 170
101 101 101 101 101   3  14 174 250 255 255 251 246 181 222 255 127 237 123  59 251 239 111 254 255  90 189 126 105  84 158 178
 65   7   3  76  16 148  36 129  21  82 205  94 157 216 196  87 234 116 190 244 106 181 107 110 189 103  43 169 150 170 145 90
 41  16  89  66 160 176 164 162  82   0 170  67  42 189  53 212 153  85 149 182 105 149 225 126 174 239 106 215 189 181 157 183
 21 250 170 166 184  75  10 202  72   8  69 170 171 150 105  81  37  88  92 171  72  79  70 170  66 147 117  82 182  69 160 170
137   2  64  73  69   1  74 164 174 170 173 214  92 171  84  82 138 170 150  26 174 169 170 106 106  43 110 106 181 117 212 211
  3   4 146 144 224  42  69 116 158 201 151  83  33 103  39 106 168 249  85 189  72 157  74  84  85 148 175  90 202 238  41 37
*/
