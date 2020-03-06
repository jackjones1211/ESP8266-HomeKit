# ESP8266-HomeKit
Iphone 控制ESP8266 IO口 支持ESPtouch Smartconfig配置wifi 苹果HomeKit连接控制GPIO2口，LED灯是低电平开，高电平关。

本案例使用ESP8266_RTOS_SDK 2.0 小黄板 32Bit Flash

ESP8266-HomeKit的制作说明
先决条件
xtensa-lx106-elf-gcc工具链。建议在计算机上安装此方法的方法是安装esp-open-sdk。使用安装make toolchain esptool libhal STANDALONE=n

编译中
转到ESP8266-HomeKit目录并运行deploy.shshell脚本。这将创建文件：
ESP8266_RTOS_SDK / lib / libhkc.a
ESP8266_RTOS_SDK / include / hkc.h
ESP8266_RTOS_SDK $ cd ESP8266-HomeKit
ESP8266-HomeKit $ ./deploy.sh  
如果一切顺利，您应该会看到
...
xtensa-lx106-elf-ar: creating .output/eagle/debug/lib/libhkc.a
rm -f -r _libhkc

########## success ###########
deployed lib/libhkc.a and include/hkc.h
ESP8266-HomeKit$
现在按照ESP8266-HomeKit-Demo wiki的说明进行操作

设定您的WiFi
如果您没有设置WiFi，在苹果app下载ESPtouch APP配置wifi。

编译中
转到ESP8266-HomeKit-Demo目录
ESP8266_RTOS_SDK $ cd ESP8266-HomeKit-Demo
转到演示目录
ESP8266-HomeKit-Demo $ cd演示
演示$ ./gen_misc.sh  
使用所有默认设置或尝试其他设置...
这将在几分钟后创建文件：
No boot needed.
Generate eagle.flash.bin and eagle.irom0text.bin successully in BIN_PATH
eagle.flash.bin-------->0x00000
eagle.irom0text.bin---->0x10000
!!!

把程序下载到小黄板

用法：
引导后，如果设备尚未配对，则会在约25秒内计算出srp-key
之后，服务器启动，并且mulicastdns开始进行广告
默认密码是 031-45-154
该代码将客户端密钥写入扇区0x13000
与原始设备取消配对后，密钥将被销毁，您可以再次配对。
如果出现问题，请转到基础，然后将signature字符串更改为其他内容，您可以从头开始