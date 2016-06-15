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
 *                         myRF24SPISpySpecialOutput.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to record the SPI traffic towards a RF24 transceiver
 *
*/

#ifndef __myRF24SPISpySpecialOutput_H__
#define __myRF24SPISpySpecialOutput_H__

#include "BK2421.h"

void outSETUP_RETR(uint8_t valueIn) {
	byte arcValue = valueIn & 0b1111;
	byte ardValue = (valueIn >> 4);

	switch (arcValue) {
		case 0:
			Serial.println("Retransmit Disabled");
			break;
		case 1 ... 15:
			Serial.println(String(arcValue) + " retries");
			break;
		default:
			Serial.println("Reserved / not allowed!");
			break;
	}

	int usInt = 250 + ardValue * 250;
	Serial.println("Wait " + String(usInt) + " us");
} // void outSETUP_RETR(uint8_t valueIn) {


void outData(byte bufferIndexIn, byte registerIndexIn) {
	Serial.println(bk2421RegisterNames[registerIndexIn]);

	switch (registerIndexIn) {
		case BK2421_CONFIG: // 00 CONFIG
			for (int i=0; i<8; i++) {
				Serial.println(bk2421CONFIG[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
			}
			break;
		case BK2421_EN_AA: // 01 EN_AA
			for (int i=0; i<8; i++) {
				Serial.println(bk2421ENAA[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
			}
			break;
		case BK2421_EN_RXADDR: // 02 EN_RXADDR
			for (int i=0; i<8; i++) {
				Serial.println(bk2421EN_RXADDR[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
			}
			break;
		case BK2421_SETUP_AW: // 03 SETUP_AW
			if (receiverBuffer[bufferIndexIn][1] < 4) {
				Serial.println(bks2421SETUP_AW[receiverBuffer[bufferIndexIn][1]]);
			} else {
				Serial.println(String(receiverBuffer[bufferIndexIn][1]) + " Reserved / not allowed!");
			}
			break;
		case BK2421_SETUP_RETR: // 04 SETUP_RETR
			outSETUP_RETR(receiverBuffer[bufferIndexIn][1]);
			break;
		case BK2421_RF_CH: // 05 RF_CH
			Serial.println(String(receiverBuffer[bufferIndexIn][1]));
			break;
		case BK2421_RF_SETUP: // 06 RF_SETUP
			for (int i=0; i<8; i++) {
				if (i != 1 && i != 2) {
					Serial.println(bks2421RF_SETUP[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
				} else if (i == 1) { // if (i != 1 && != 2) {
					byte rfPwrIndex = receiverBuffer[bufferIndexIn][1] & 0b00000110;
					rfPwrIndex = (rfPwrIndex >> 1);
					Serial.println("RF_PWR: " + bk2421RF_PWR[rfPwrIndex]);
				} // if (i != 1 && != 2) {
			} // for (int i=0; i<8; i++) {
			break;

		case BK2421_STATUS: // 07 STATUS
			for (int i=0; i<8; i++) {
				if (i < 1 || i > 3) {
					Serial.println(bks2421STATUS[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
				} else if (i == 1) { // if (i != 1 && != 2) {
					byte rx_p_no = receiverBuffer[bufferIndexIn][1] & 0b00001110;
					rx_p_no = (rx_p_no >> 1);
					switch(rx_p_no) {
						case 0 ... 5:
							Serial.println("pipe " + String(rx_p_no) + " available");
							break;
						case 6:
							Serial.println("Not used " + String(rx_p_no));
							break;
						case 7:
							Serial.println("RX FIFO Empty");
							break;
					}
				} // if (i != 1 && != 2) {
			} // for (int i=0; i<8; i++) {
			break;

		// 08 Read Only
		// 09 Read Only
		case BK2421_RX_ADDR_P0: // 0A RX_ADDR_P0
			printBytes(bufferIndexIn, true);
			break;
		case BK2421_RX_ADDR_P1: // 0B RX_ADDR_P1
			printBytes(bufferIndexIn, true);
			break;
		case BK2421_RX_ADDR_P2 ... BK2421_RX_ADDR_P5: // 0C RX_ADDR_P2 ... 0F RX_ADDR_P5
			Serial.println(String(receiverBuffer[bufferIndexIn][1]));
			break;
		case BK2421_TX_ADDR: // 10 TX_ADDR
			printBytes(bufferIndexIn, true);
			break;
		case BK2421_RX_PW_P0 ... BK2421_RX_PW_P5: // 11 RX_PW_P0 ... 16 RX_PW_P5
			if (receiverBuffer[bufferIndexIn][1] <= 32) {
				Serial.println(String(receiverBuffer[bufferIndexIn][1]));
			} else {
				Serial.println(String(receiverBuffer[bufferIndexIn][1]) + " Reserved / not allowed");
			}
			break;
		// 17 FIFO_STATUS Read Only
		// 18 ... 1B N/A ?!?
		case BK2421_DYNPD: // 1C DYNPD
			for (int i=0; i<8; i++) {
				Serial.println(bks2421DYNPD[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
			}
			break;
		case BK2421_FEATURE: // 1D FEATURE
			for (int i=0; i<8; i++) {
				Serial.println(bks2421FEATURE[i] + ": " + String((receiverBuffer[bufferIndexIn][1] >> i) & 1));
			}
			break;
		default:
			Serial.println();
	} // switch (command) {
} // void outData(byte registerIndex) {


void processBuffer(byte bufferIndexIn) {
	uint8_t command = receiverBuffer[bufferIndexIn][0];
	
	switch(command) {
		case BK2421_NOP: // NOP
			Serial.println("NOP");
			break;
		case BK2421_REUSE_TX_PL: // REUSE_TX_PL
			Serial.println("REUSE_TX_PL");
			break;
		case BK2421_FLUSH_RX: // FLUSH_RX
			Serial.println("FLUSH_RX");
			break;
		case BK2421_FLUSH_TX: // FLUSH_TX
			Serial.println("FLUSH_TX");
			break;
		case BK2421_W_TX_PAYLOAD_NO_ACK: // W_TX_PAYLOAD_NO_ACK
			Serial.println("W_TX_PAYLOAD_NO_ACK");
			break;
		case BK2421_W_ACK_PAYLOAD: // W_ACK_PAYLOAD
			Serial.println("W_ACK_PAYLOAD");
			break;
		case BK2421_W_TX_PAYLOAD: // W_TX_PAYLOAD
			Serial.println("W_ACK_PAYLOAD");
			printBytes(bufferIndexIn, true);
			break;
		case BK2421_R_RX_PAYLOAD: // R_RX_PAYLOAD
			Serial.println("R_RX_PAYLOAD");
			break;
		case BK2421_R_RX_PL_WID: // R_RX_PL_WID
			Serial.println("R_RX_PL_WID");
			break;
		case BK2421_ACTIVATE: // ACTIVATE
			Serial.print("ACTIVATE, ");
			switch (receiverBuffer[bufferIndexIn][1]) {
				case 83:
					Serial.println("Switching Bank");
					break;
				case 115:
					Serial.println("Enabling / Disabling Registers");
					break;
				default:
					Serial.println("Unknown sub command: " + String(receiverBuffer[bufferIndexIn][1]));
			}
			break;
		case 32 ... 61:
			Serial.print("W ");
			outData(bufferIndexIn, command - 32);
			break;
		case 0 ... 29:
			Serial.println("R " + bk2421RegisterNames[command]);
//			outData(bufferIndexIn, command);
			break;
		default:
			Serial.println("Unknown Command " + String(command));
	} // switch(command) {
	Serial.println();
} // void processBuffer(byte bufferIndexIn) {

#endif // #ifndef __myRF24SPISpySpecialOutput_H__
