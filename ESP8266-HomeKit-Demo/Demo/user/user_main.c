/*
 * @Author: jack
 * @Date: 2020-03-03 21:05:27
 * @LastEditTime: 2020-03-06 21:18:42
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /ESP8266-HomeKit/ESP8266-HomeKit-Demo/Demo/user/user_main.c
 */
/*
 *
 *  Licensed under the Apache License, Version 2.0 (the "License")
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*****************************************************************************************
 * Welcome to the HomeACcessoryKid hkc demo
 * With a few lines of code we demonstrate the easy setup of your ESP8266 as an accessory.
 * Start defining your accessory in hkc_user_init and execute other pending init tasks.
 * For each Service characteristic a callback function is defined.
 * An ACC_callback will be called in different modes.
 * - mode=0: initialize your service (init)
 * - mode=1: a request for a change  is received on which you could act (write)
 * - mode=2: a request for a refresh is received where you might update  (read)
 * A callback should return QUICKLY, else use a Task as demonstrated below.
 *
 * If something changes from inside, you can use change_value and send_events in return.
 * You use aid and iid to know which characteristic to handle and cJSON for the value.
 *
 * Use iOS10 Home app or Eve or other app to test all the features and enjoy
 * *欢迎来到HomeACcessoryKid hkc演示

*通过几行代码，我们将演示如何轻松设置您的ESP8266作为附件。
*开始在hkc_user_init中定义附件并执行其他挂起的init任务。
*为每个服务特性定义一个回调函数。
*将以不同模式调用ACC_回调。
*-mode=0:初始化服务（init）
*-mode=1：收到更改请求，您可以对其进行操作（写入）
*-mode=2：在可能更新（读取）的位置接收刷新请求
*回调应该很快返回，否则使用如下所示的任务。
*如果从内部发生了变化，可以使用change_value并发送_事件作为回报。
*您可以使用aid和iid来了解要处理的特性，并以cJSON为值。
*使用iOS10家庭应用程序或Eve或其他应用程序测试所有功能并享受
*****************************************************************************************/
 
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"

#include "hkc.h"
#include "gpio.h"
#include "queue.h"
#include "uart.h"
#include "smart.h"

xQueueHandle identifyQueue;

struct  gpio {
    int aid;
    int iid;
} gpio2;



void    led_task(void *arg) //make transfer of gpio via arg, starting as a static variable in led routine
{
    int     i,original;
    cJSON   *value;

    value=cJSON_CreateBool(0); //value doesn't matter
    while(1) {
        vTaskDelay(1500); //15 sec
        original=GPIO_INPUT(GPIO_Pin_2); //get original state
//      os_printf("original:%d\n",original);
        value->type=original^1;
        os_printf("led_task send_events\n");
//       GPIO_OUTPUT(GPIO_Pin_2,original^1); // and toggle
//       change_value(    gpio2.aid,gpio2.iid,value);
        send_events(NULL,gpio2.aid,gpio2.iid);
    }
}

void led(int aid, int iid, cJSON *value, int mode)
{
    GPIO_ConfigTypeDef gpio2_in_cfg;
    switch (mode) {
        case 1: { //changed by gui
            char *out; out=cJSON_Print(value);  os_printf("led %s\n",out);  free(out);  // Print to text, print it, release the string.
            if (value) GPIO_OUTPUT(GPIO_Pin_2,!(value->type));
        }break;
        case 0: { //init
            gpio2_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;         //no interrupt
            gpio2_in_cfg.GPIO_Mode     = GPIO_Mode_Output;              //Output mode
            gpio2_in_cfg.GPIO_Pullup   = GPIO_PullUp_EN;                //improves transitions
            gpio2_in_cfg.GPIO_Pin      = GPIO_Pin_2;                    //Enable GPIO
            gpio_config(&gpio2_in_cfg);                                 //Initialization function
            led(aid,iid,value,1);
            gpio2.aid=aid; gpio2.iid=iid;
            xTaskCreate(led_task,"led",512,NULL,2,NULL);
        }break;
        case 2: { //update
            //do nothing
        }break;
        default: {
            //print an error?
        }break;
    }
}

void identify_task(void *arg)
{
    int i,original;

    os_printf("identify_task started\n");
    while(1) {
        while(!xQueueReceive(identifyQueue,NULL,10));//wait for a queue item
        os_printf("a queue item\n");
        original=GPIO_INPUT(GPIO_Pin_2); //get original state
        for (i=0;i<2;i++) {
            GPIO_OUTPUT(GPIO_Pin_2,original^1); // and toggle
            vTaskDelay(30); //0.3 sec
            GPIO_OUTPUT(GPIO_Pin_2,original^0);
            vTaskDelay(30); //0.3 sec
        }
    }
}

void identify(int aid, int iid, cJSON *value, int mode)
{
    switch (mode) {
        case 1: { //changed by gui
            xQueueSend(identifyQueue,NULL,0);
        }break;
        case 0: { //init
         os_printf("init identify\n");
        identifyQueue = xQueueCreate( 1, 0 );
        xTaskCreate(identify_task,"identify",256,NULL,2,NULL);
        }break;
        case 2: { //update
            //do nothing
        }break;
        default: {
            //print an error?
        }break;
    }
}

extern  cJSON       *root;
void    hkc_user_init(char *accname)
{
    //do your init thing beyond the bear minimum 把你的第一件事做得超出最低限度
    //avoid doing it in user_init else no heap left for pairing 避免在user_in it中执行此操作，否则将没有可用于配对的堆
    cJSON *accs,*sers,*chas,*value;
    int aid=0,iid=0;

    accs=initAccessories();
    
    sers=addAccessory(accs,++aid);
    //service 0 describes the accessory
    chas=addService(      sers,++iid,APPLE,ACCESSORY_INFORMATION_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,accname,NULL);
    addCharacteristic(chas,aid,++iid,APPLE,MANUFACTURER_C,"HacK",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,MODEL_C,"Rev-1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,SERIAL_NUMBER_C,"1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,IDENTIFY_C,NULL,identify);
    //service 1
    chas=addService(      sers,++iid,APPLE,SWITCH_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,"led",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,POWER_STATE_C,"1",led);
    //service 2
/*     chas=addService(      sers,++iid,APPLE,LIGHTBULB_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,"light",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,POWER_STATE_C,"0",NULL);
    addCharacteristic(chas,aid,++iid,APPLE, BRIGHTNESS_C,"0",NULL); */

    char *out;
    out=cJSON_Print(root);  os_printf("%s\n",out);  free(out);  // Print to text, print it, release the string.

//  for (iid=1;iid<MAXITM+1;iid++) {
//      out=cJSON_Print(acc_items[iid].json);
//      os_printf("1.%d=%s\n",iid,out); free(out);
//  }
}

/**
 * @description: Uart0 配置波特率为 115200
 * @param none
 * @return: none
 */
void Uart0_Config(void)
{
     UART_ConfigTypeDef uart_config;
    uart_config.baud_rate = BIT_RATE_115200;
    uart_config.data_bits = UART_WordLength_8b;
    uart_config.parity = USART_Parity_None;
    uart_config.stop_bits = USART_StopBits_1;
    uart_config.flow_ctrl = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
    uart_config.UART_InverseMask = UART_None_Inverse;
    UART_ParamConfig(UART0, &uart_config);
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{   
   
    Uart0_Config();
    os_printf("start of user_init @ %d\n",system_get_time()/1000);
	
    /* need to set opmode before you set config */
    wifi_set_opmode(STATION_MODE);
    xTaskCreate(smartconfig_task, "smartconfig_task", 256, NULL, tskIDLE_PRIORITY+3, NULL);

//use this block only once to set your favorite access point or put your own selection routine
/*    wifi_set_opmode(STATION_MODE); 
    struct station_config *sconfig = (struct station_config *)zalloc(sizeof(struct station_config));
    sprintf(sconfig->ssid, ""); //don't forget to set this if you use it
    sprintf(sconfig->password, ""); //don't forget to set this if you use it
    wifi_station_set_config(sconfig);
    free(sconfig);
    wifi_station_connect(); /**/
    
    //try to only do the bare minimum here and do the rest in hkc_user_init
    // if not you could easily run out of stack space during pairing-setup
    hkc_init("HomeACcessory");
 //   os_printf("end of user_init @ %d\n",system_get_time()/1000);
    os_printf("end of user_init @ %d\n",system_get_time()/1000);
    //测试代码
}

/***********************************************************************************
 * FunctionName : user_rf_cal_sector_set forced upon us by espressif since RTOS1.4.2
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal    B : rf init data    C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
***********************************************************************************/
uint32 user_rf_cal_sector_set(void) {
    extern char flashchip;
    SpiFlashChip *flash = (SpiFlashChip*)(&flashchip + 4);
    // We know that sector size is 4096
    //uint32_t sec_num = flash->chip_size / flash->sector_size;
    uint32_t sec_num = flash->chip_size >> 12;
    return sec_num - 5;
}
