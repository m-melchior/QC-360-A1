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
 *                         Nunchuck.cpp  -  description
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

#include "Nunchuck.h"

Nunchuck::Nunchuck() {
	
}

void Nunchuck::begin() {
	joyX = 0;
	joyY = 0;

	accX = 0;
	accY = 0;
	accZ = 0;
	
	accXCal = 512;
	accYCal = 512;
	accZCal = 512;

	buttonC = false;
	buttonZ = false;
	
	Wire.begin();

	Wire.beginTransmission(0x52);
	Wire.write(0xF0);
	Wire.write(0x55);
	Wire.endTransmission();

	delay(100);

	Wire.beginTransmission(0x52);
	Wire.write(0xFB);
	Wire.write((uint8_t)0x00);
	Wire.endTransmission();
	
	delay(100);

	requestData();
}


void Nunchuck::requestData() {
	Wire.beginTransmission(0x52);

	Wire.write((uint8_t)0x00);

	Wire.endTransmission();
}


bool Nunchuck::getData() {
	bool ret = false;
	uint8_t counter = 0;

	Wire.requestFrom (0x52, 6);

	while (Wire.available ()) {
		myBuffer[counter++] = Wire.read();
	}

	if (counter == 6) {
		joyX = myBuffer[0];
		joyY = myBuffer[1];
		accX = (myBuffer[2] << 2) + ((myBuffer[5] >> 2) & 3);
		accY = (myBuffer[3] << 2) + ((myBuffer[5] >> 4) & 3);
		accZ = (myBuffer[4] << 2) + ((myBuffer[5] >> 6) & 3);

		buttonC = ((myBuffer[5]) & 2) ? false : true;
		buttonZ = ((myBuffer[5]) & 1) ? false : true;

		ret =  true;
	}

	requestData();

	return ret;
}

bool Nunchuck::getC() {
	return buttonC;
}

bool Nunchuck::getZ() {
	return buttonZ;
}

uint8_t Nunchuck::getJoyX() {
	return joyX; 
}

uint8_t Nunchuck::getJoyY() {
	return joyY;
}

int Nunchuck::getAccX()
{
	return accX;
}

int Nunchuck::getAccY()
{
	return accY;
}

int Nunchuck::getAccZ()
{
	return accZ;
}

int Nunchuck::getRoll() {
		return (int)(atan2((accX - accXCal), (accZ - accZCal)) / M_PI * 180.0);
}

int Nunchuck::getPitch() {
		return (int)(atan2((accY - accYCal), (accZ - accZCal)) / M_PI * 180.0);
}
