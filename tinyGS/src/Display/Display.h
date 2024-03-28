/*
  Display.h - Class responsible of controlling the display
  
  Copyright (C) 2020 -2021 @G4lile0, @gmag12 and @dev_4m1g0

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../ConfigManager/ConfigManager.h"
#include "../Status.h"
#ifdef HELTEC_TRACKER_V1_1
#include "Display_TFT.h"
#include <SPI.h>
#endif
#include "SSD1306.h"                         // https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "OLEDDisplayUi.h"                   // https://github.com/ThingPulse/esp8266-oled-ssd1306

void displayInit();
void displayShowConnected();
void displayShowInitialCredits();
void displayShowApMode();
void displayShowStaMode(bool ap);
void displayUpdate();
void displayTurnOff();
void displayNextFrame();

extern Status status;
