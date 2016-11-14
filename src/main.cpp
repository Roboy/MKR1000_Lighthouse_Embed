/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include "SPI.h"

#define INPUT           (0x0) 
#define OUTPUT          (0x1)
#define INPUT_PULLUP    (0x2)
#define INPUT_PULLDOWN  (0x3)

#define LOW             (0x0)
#define HIGH            (0x1)


const int               ledPin =  0;               // the number of the LED pin
int                     ledState = LOW;              // ledState used to set the LED
unsigned long           previousMillis = 0;         // will store last time LED was updated
const long              interval = 1000;            // interval at which to blink (milliseconds)

int main( void )
{
    SystemInit(); 
    pinMode(13, OUTPUT); 
    digitalWrite(13, HIGH); 
    for (;;)
    {
        
    }
    return 0;
}
