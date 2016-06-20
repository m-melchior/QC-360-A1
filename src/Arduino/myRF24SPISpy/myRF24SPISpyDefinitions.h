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
 *                         myRF24SPISpyDefinitions.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An application to record the SPI traffic towards a RF24 transceiver
 *
*/

#ifndef __myRF24SPISpyDefinitions_H__
#define __myRF24SPISpyDefinitions_H__

#define RF24_CONFIG					0x00
#define RF24_EN_AA					0x01
#define RF24_EN_RXADDR				0x02
#define RF24_SETUP_AW				0x03
#define RF24_SETUP_RETR				0x04
#define RF24_RF_CH					0x05
#define RF24_RF_SETUP				0x06
#define RF24_STATUS					0x07
#define RF24_OBSERVE_TX				0x08
#define RF24_CD						0x09
#define RF24_RX_ADDR_P0				0x0A
#define RF24_RX_ADDR_P1				0x0B
#define RF24_RX_ADDR_P2				0x0C
#define RF24_RX_ADDR_P3				0x0D
#define RF24_RX_ADDR_P4				0x0E
#define RF24_RX_ADDR_P5				0x0F
#define RF24_TX_ADDR				0x10
#define RF24_RX_PW_P0				0x11
#define RF24_RX_PW_P1				0x12
#define RF24_RX_PW_P2				0x13
#define RF24_RX_PW_P3				0x14
#define RF24_RX_PW_P4				0x15
#define RF24_RX_PW_P5				0x16
#define RF24_FIFO_STATUS			0x17
#define RF24_DYNPD					0x1C
#define RF24_FEATURE				0x1D
#define RF24_ACTIVATE				0x50
#define RF24_R_RX_PL_WID			0x60
#define RF24_R_RX_PAYLOAD			0x61
#define RF24_W_TX_PAYLOAD			0xA0
#define RF24_W_ACK_PAYLOAD			0xA8
#define RF24_W_TX_PAYLOAD_NO_ACK	0xB0
#define RF24_FLUSH_TX				0xE1
#define RF24_FLUSH_RX				0xE2
#define RF24_REUSE_TX_PL			0xE3
#define RF24_NOP					0xFF

String RF24_REGISTER_NAMES[] = {
	"CONFIG",
	"EN_AA",
	"EN_RXADDR",
	"SETUP_AW",
	"SETUP_RETR",
	"RF_CH",
	"RF_SETUP",
	"STATUS",
	"OBSERVE_TX",
	"CD",
	"RX_ADDR_P0",
	"RX_ADDR_P1",
	"RX_ADDR_P2",
	"RX_ADDR_P3",
	"RX_ADDR_P4",
	"RX_ADDR_P5",
	"TX_ADDR",
	"RX_PW_P0",
	"RX_PW_P1",
	"RX_PW_P2",
	"RX_PW_P3",
	"RX_PW_P4",
	"RX_PW_P5",
	"FIFO_STATUS",
	"N/A",
	"N/A",
	"N/A",
	"N/A",
	"DYNPD",
	"FEATURE"
};

String RF24_CONFIG_NAMES[] = {
	"PRIM_RX",
	"PWR_UP",
	"CRCO",
	"EN_CRC",
	"MASK_MAX_RT",
	"MASK_TX_DS",
	"MASK_RX_DR",
	"Reserved"
};
	
String RF24_ENAA_NAMES[] = {
	"ENAA_P0",
	"ENAA_P1",
	"ENAA_P2",
	"ENAA_P3",
	"ENAA_P4",
	"ENAA_P5",
	"Reserved",
	"Reserved"
};

String RF24_EN_RXADDR_NAMES[] = {
	"ERX_P0",
	"ERX_P1",
	"ERX_P1",
	"ERX_P1",
	"ERX_P4",
	"ERX_P5",
	"Reserved",
	"Reserved"
};

String RF24_SETUP_AW_NAMES[] = {
	"Illegal",
	"3 bytes",
	"4 bytes",
	"5 bytes"
};

String RF24_RF_SETUP_NAMES[] = {
	"LNA_HCURR",
	"",
	"",
	"RF_DR",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

String RF24_RF_PWR_NAMES[] = {
	"-10 dBm",
	"-5 dBm",
	"0 dBm",
	"5 dBm"
};

String RF24_STATUS_NAMES[] = {
	"TX_FULL",
	"",
	"",
	"",
	"MAX_RT",
	"TX_DS",
	"RX_DR",
	"RBANK"
};

String RF24_DYNPD_NAMES[] = {
	"DPL_P0",
	"DPL_P1",
	"DPL_P2",
	"DPL_P3",
	"DPL_P4",
	"DPL_P5",
	"Reserved",
	"Reserved"
};

String RF24_FEATURE_NAMES[] = {
	"EN_DYN_ACK",
	"EN_ACK_PAY",
	"EN_DPL",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

#endif // #ifndef __myRF24SPISpyDefinitions_H__
