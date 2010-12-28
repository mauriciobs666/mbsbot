#!/bin/bash

# (C) GPL 2010 - Mauricio Bieze Stefani
# Based on info from http://www.ladyada.net/learn/avr/avrdude.html

if [ ! -x $ARDUINO_DIR/hardware/tools/avrdude ]; then 
    if [ -z $ARDUINO_DIR ]; then
        echo "export ARDUINO_DIR="
        exit
    else
        echo "$ARDUINO_DIR/hardware/tools/avrdude is not an executable"
	exit
    fi
fi

if [ $# -lt 2 ]; then 
    echo "Usage: reflash.sh <PORT> <FILE.HEX>"
    exit
fi

$ARDUINO_DIR/hardware/tools/avrdude -C$ARDUINO_DIR/hardware/tools/avrdude.conf -v -v -patmega328p -cstk500v1 -P$1 -b57600 -D -Uflash:w:$2:i

