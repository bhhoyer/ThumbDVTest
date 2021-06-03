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

Modification to ftx_prog to eliminate Continue:

 } else {
    if (verbose) { dumpmem("new eeprom", new, len); }

    if (erase_eeprom == 0) {
      printf("Rewriting eeprom with new contents.\n");
    } else {
      printf("Erasing EEPROM\n");
    }
    /*BHH Removed
    printf("Continue? [y|n]:");
    if (getc(stdin) == 'y')
    */
    {
      ee_write(new, len);

      /* Read it back again, and check for differences */
      if (ee_read_and_verify(new, len) != new_crc ) {

