# ThumbDVTest
Test program for NW Digital Radio ThumbDV

Install:
sudo apt-get install python-pyudev python-serial

Usage:
Run Packet Tx/RX:
sudo python bhh.py
THEN Plug in ThumbDV

NWDR MANUFACTURING ONLY:
FTDI EEPROM Programming Requires https://github.com/richardeoin/ftx-prog

Run Test, then program FTDI EEPROM:
sudo python bhh.y -f "MM/YY" #MM/YY = Assemby Date
