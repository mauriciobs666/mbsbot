#!/usr/bin/bash

# http://www.ladyada.net/learn/avr/avrdude.html

$ARDUINO_DIR/hardware/tools/avrdude -C$ARDUINO_DIR/hardware/tools/avrdude.conf -v -v -patmega328p -cstk500v1 -P$1 -b57600 -D -Uflash:w:$2:i 
