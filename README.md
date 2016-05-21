# WirelessNBHardware
Hardware explanation and code for WNB.

This is the code for programming the WNB developed as my undergraduate final project, this projects are write in C and should be opened in AtmelStudio. Also there is all the circuit board developed in kiCad.
There are three projects:

-"Blinking led Tarjeta:" This is the first project you should mount to the board, it makes the two LED flicker and that means everithing is well connected on the board.

-"Firmware_TransmisorEEG_BT:" This program the board to transmit the data for the 8 channels, whit this configurations:

--CH1: Test Mode
--CH2: Test Mode
--CH3: Normal Mode
--CH4: Normal Mode
--CH5: Normal Mode
--CH6: Normal Mode
--CH7: Short circuit Mode
--CH8: Short circuit Mode

--Bluetooth data is transmited at a baudrate of 230400Bd and 250msp.

-"Hardware:" This contains all the files for making the board PCB, it contains some minor mistakes that have been found in the develop of this project, but is the version of the board that had been proved.
