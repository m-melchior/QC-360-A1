/* Nunchuck
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
 *                         Nunchuck.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2016 by Michael Melchior
 *   copyright            : (c) 2016 by Michael Melchior
 *   description          : An lightweight library for Wii Nunchuck inputs
 *
*/

/*
 * This library was inspired by the following two libraries:
 * http://playground.arduino.cc/Main/WiiChuckClass
 * https://github.com/todbot/wiichuck_adapter/blob/master/firmware/WiichuckDemo/nunchuck_funcs.h
 * Unfortunately the libraries above were not suitable to the needs of this project
 * so they were stripped and streamlined to fit
*/

#ifndef __Nunchuck_H__
#define __Nunchuck_H__

#include <Arduino.h>
#include <Wire.h>

class Nunchuck {
private:
	uint8_t myBuffer[6];

	uint8_t joyX;
	uint8_t joyY;

	int accX;
	int accY;
	int accZ;

	int accXCal;
	int accYCal;
	int accZCal;

	bool buttonC;
	bool buttonZ;

public:
	Nunchuck();

	void begin(void);

	void requestData(void);

	uint8_t decodeData (uint8_t dataIn);

	bool getData(void);

	bool getC(void);

	bool getZ(void);

	uint8_t getJoyX(void);

	uint8_t getJoyY(void);

	int getAccX(void);

	int getAccY(void);

	int getAccZ(void);
	
	int getPitch();
	
	int getRoll();
};

#endif
