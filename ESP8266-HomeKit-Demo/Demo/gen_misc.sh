###
 # @Author: your name
 # @Date: 2020-03-02 22:24:55
 # @LastEditTime: 2020-03-06 20:53:33
 # @LastEditors: your name
 # @Description: In User Settings Edit
 # @FilePath: /ESP8266-HomeKit/ESP8266-HomeKit-Demo/Demo/gen_misc.sh
 ###
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


echo makeflash
#esptool.py --port /dev/tty.SLAB_USBtoUART --baud 115200 write_flash -fm qio -fs 4MB-c1 -ff 40m 0x00000 ./bin/boot_v1.6.bin 0x3fc000 
#./bin/esp_init_data_default.bin 0x3fe000 
#./bin/blank.bin 0x01000 
#./bin/upgrade/user1.4096.new.6.bin
esptool.py --baud 230400 -p /dev/tty.SLAB_USBtoUART write_flash 0x00000 $BIN_PATH/eagle.flash.bin 0x10000 $BIN_PATH/eagle.irom0text.bin 
echo flash 

date
ls -l $BIN_PATH/eagle.[if]*
echo ../../../esptool/esptool.py --baud 230400 -p /dev/tty.SLAB_USBtoUART write_flash 0x00000 ../../bin/eagle.flash.bin 0x10000 ../../bin/eagle.irom0text.bin

