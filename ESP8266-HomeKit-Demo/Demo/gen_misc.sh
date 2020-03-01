#!/bin/bash
:<<!
******NOTICE******
MUST set SDK_PATH & BIN_PATH firstly!!!
example:
export SDK_PATH=~/esp_iot_sdk_freertos
export BIN_PATH=~/esp8266_bin
!

echo "based on gen_misc.sh version 20150911"
echo ""

if [ ! $SDK_PATH ]; then
    export SDK_PATH=$(dirname $(dirname $(pwd)))
fi
echo "SDK_PATH:"
echo "$SDK_PATH"
echo ""

if [ ! $BIN_PATH ]; then
    export BIN_PATH=$SDK_PATH/bin
fi
echo "BIN_PATH:"
echo "$BIN_PATH"
echo ""

boot=new
app=0
spi_speed=40
spi_mode=QIO
spi_size_map=6

make clean

make BOOT=$boot APP=$app SPI_SPEED=$spi_speed SPI_MODE=$spi_mode SPI_SIZE_MAP=$spi_size_map

date
ls -l $BIN_PATH/eagle.[if]*
echo ../../../esptool/esptool.py --baud 230400 -p /dev/cu.usbserial-* write_flash 0x00000 $BIN_PATH/eagle.flash.bin 0x14000 $BIN_PATH/eagle.irom0text.bin
