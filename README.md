# ThumbDVTest
Test program for NW Digital Radio ThumbDV

Install:
sudo apt-get install python-pyudev python-serial
FTDI EEPROM Programming Requires https://github.com/richardeoin/ftx-prog

Usage:
Run Packet Tx/RX:
sudo python bhh.py

Run Test, then program FTDI EEPROM:
sudo python bhh.y -f "MM/YY" #MM/YY = Assemby Date
