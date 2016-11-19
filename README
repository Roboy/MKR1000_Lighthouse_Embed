Adjust the $MODULE_PATH variable of the 'Makefile' to your project directory path. 
Clone the current WiFi Library for the MKR1000 from: 

'https://github.com/arduino-libraries/WiFi101'

Unzip it, rename it to WiFi101 and move it into the root directory of this project. To avoid double dependencies delete or rename the file 'nm_bsp_arduino_avr.c' (the suffix of this file must not contain .c) in the 'WiFi101/src/bsp/source/' folder.

The code execution on the µC starts off from the 'src/main.cpp' file. In order to compile your code type 'make' in the root directory. Modify the Makefile according to your needs. 
To install the Code on your Arduino MKR1000 connect it via USB, double press the Reset Button and type 'make install'. It should then earse the current flash code and install the binary from the build folder. 

